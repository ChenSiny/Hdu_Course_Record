import threading
import time
from collections import deque, defaultdict
import socket
import os
import re

"""
网络监控模块（NetMonitor）

职责：使用 scapy 抓包，将观察到的数据包尝试归因到系统中的某个进程（pid），并按时间序列记录 (timestamp, pid, bytes)，以便后续按时间窗口统计各进程流量。

实现思路：
- 抓包回调 _packet_cb 提取 IP/TCP/UDP 四元组 (src_ip, src_port, dst_ip, dst_port)。
- 解析 /proc/net/{tcp,udp} 获取每条连接项对应的 inode。
- 扫描 /proc/<pid>/fd 中的 socket:[inode] 链接建立 inode -> pid 的映射。
- 通过 inode->pid 映射把数据包归因到 pid，并追加到 self.records（deque）。

注意与限制：
- 该方法依赖 /proc 的可访问性（在容器或受限环境中可能失败）。
- 频繁扫描 /proc 会有性能开销，模块中使用了简单的缓存和刷新策略（见 _refresh_inode_map）。
- 抓包和读取其他进程 /proc/*/fd 通常需要 root 权限。
"""

try:
    from scapy.all import sniff, IP, TCP, UDP
except Exception:
    sniff = None
    IP = TCP = UDP = None


def _parse_proc_net(protocol='tcp'):
    # 解析 /proc/net/{tcp|udp}，返回每行的本地/远端地址与 inode 列表
    # 返回格式: [{'local_ip':..., 'local_port':..., 'rem_ip':..., 'rem_port':..., 'inode':..., 'proto':...}, ...]
    path = f'/proc/net/{protocol}'
    if not os.path.exists(path):
        return []
    conns = []
    with open(path, 'r') as f:
        lines = f.readlines()[1:]
    for line in lines:
        parts = line.split()
        if len(parts) < 10:
            continue
        local, rem = parts[1], parts[2]
        inode = parts[9]
        def _hexipport(s):
            ip_hex, port_hex = s.split(':')
            ip = socket.inet_ntoa(bytes.fromhex(ip_hex)[::-1])
            port = int(port_hex, 16)
            return ip, port
        try:
            lip, lport = _hexipport(local)
            rip, rport = _hexipport(rem)
        except Exception:
            continue
        conns.append({'local_ip': lip, 'local_port': lport, 'rem_ip': rip, 'rem_port': rport, 'inode': inode, 'proto': protocol})
    return conns


def _build_inode_pid_map():
    # 扫描 /proc 下每个 pid 的 fd 目录（/proc/<pid>/fd），读取符号链接中形如 "socket:[inode]" 的项
    # 并构造 inode -> pid 的映射字典返回。
    inode_map = {}
    pid_dirs = [d for d in os.listdir('/proc') if d.isdigit()]
    for pid in pid_dirs:
        fd_dir = os.path.join('/proc', pid, 'fd')
        try:
            for fd in os.listdir(fd_dir):
                full = os.path.join(fd_dir, fd)
                try:
                    target = os.readlink(full)
                    m = re.match(r'socket:\[(\d+)\]', target)
                    if m:
                        inode = m.group(1)
                        inode_map[inode] = int(pid)
                except Exception:
                    continue
        except Exception:
            continue
    return inode_map


class NetMonitor:
    def __init__(self):
        self._running = False
        self._thread = None
        # deque of (ts, pid, bytes)
        self.records = deque()
        self._lock = threading.Lock()
        # cached maps
        self._inode_pid = {}
        self._last_map_time = 0

    def start(self):
        if sniff is None:
            print('scapy is not available; network monitoring disabled')
            return
        if self._running:
            return
        self._running = True
        self._thread = threading.Thread(target=self._run_sniff, daemon=True)
        self._thread.start()

    def _refresh_inode_map(self):
        now = time.time()
        if now - self._last_map_time > 5:
            self._inode_pid = _build_inode_pid_map()
            self._last_map_time = now

    def _find_pid_for_packet(self, src_ip, src_port, dst_ip, dst_port):
        # 在 /proc/net/{tcp,udp} 中查找匹配四元组的连接项，并根据其 inode 查找对应 pid
        for proto in ('tcp', 'udp'):
            conns = _parse_proc_net(proto)
            for c in conns:
                # match either direction
                if (c['local_ip'] == src_ip and c['local_port'] == src_port and c['rem_ip'] == dst_ip and c['rem_port'] == dst_port) or (
                    c['local_ip'] == dst_ip and c['local_port'] == dst_port and c['rem_ip'] == src_ip and c['rem_port'] == src_port):
                    inode = c['inode']
                    pid = self._inode_pid.get(inode)
                    if pid:
                        return pid
        return None

    def _packet_cb(self, pkt):
        try:
            if not pkt.haslayer(IP):
                return
            ip = pkt[IP]
            l = None
            sport = dport = None
            if pkt.haslayer(TCP):
                l = 'tcp'
                sport = pkt[TCP].sport
                dport = pkt[TCP].dport
            elif pkt.haslayer(UDP):
                l = 'udp'
                sport = pkt[UDP].sport
                dport = pkt[UDP].dport
            else:
                return
            src = ip.src
            dst = ip.dst
            # 尝试刷新 inode->pid 映射（有时间缓存，避免频繁扫描）
            self._refresh_inode_map()
            # 尝试按照 (src, sport, dst, dport) 定位 pid
            pid = self._find_pid_for_packet(src, sport, dst, dport)
            if pid is None:
                # try reverse
                pid = self._find_pid_for_packet(dst, dport, src, sport)
            if pid is not None:
                size = len(pkt)
                with self._lock:
                    self.records.append((time.time(), int(pid), int(size)))
                    # keep a sanity cap to avoid memory blowup
                    while len(self.records) > 200000:
                        self.records.popleft()
        except Exception:
            return

    def _run_sniff(self):
        # run scapy sniff in blocking call
        sniff(prn=self._packet_cb, store=False)

    def get_usage_window(self, minutes=10):
        cutoff = time.time() - minutes * 60
        sums = defaultdict(int)
        with self._lock:
            # cleanup old
            while self.records and self.records[0][0] < cutoff:
                self.records.popleft()
            for ts, pid, size in self.records:
                sums[pid] += size

        # convert pid->process name where possible
        out = []
        for pid, b in sums.items():
            name = None
            try:
                import psutil
                p = psutil.Process(pid)
                name = p.name()
            except Exception:
                name = str(pid)
            out.append({'pid': pid, 'name': name, 'bytes': b})
        out.sort(key=lambda x: x['bytes'], reverse=True)
        return out
