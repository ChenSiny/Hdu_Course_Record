import threading
import time
import psutil
from collections import defaultdict

"""
进程内存监控模块（ProcessMonitor）

职责：周期性采样系统进程的内存使用情况，按进程名进行聚合并保存聚合结果。

设计要点：
- 使用后台线程定期调用 psutil 遍历所有进程并累计每个进程名的 RSS（常驻内存）和占比。
- 结果以字典形式保存在 self.aggregated 中，供外部通过 get_top_memory() 读取。
- 聚合是按进程名（p.name()）进行的，可能会把不同用户或不同路径但同名的进程合并。

线程/并发注意：
- 采样线程在更新 self.aggregated 时使用一个简单的 threading.Lock() 保护读写一致性。
"""


class ProcessMonitor:
    def __init__(self, sample_interval=2):
        self.sample_interval = sample_interval
        self._running = False
        self._thread = None
        self._lock = threading.Lock()
        # aggregated by process name => {'mem_rss': int, 'mem_percent': float, 'pids': [..]}
        self.aggregated = {}

    def _sample(self):
        # 后台采样循环：只要 self._running 为 True 就不断采样
        while self._running:
            by_name = defaultdict(lambda: {'mem_rss': 0, 'mem_percent': 0.0, 'pids': []})
            for p in psutil.process_iter(['pid', 'name', 'memory_info', 'memory_percent']):
                try:
                    info = p.info
                    name = info.get('name') or str(info.get('pid'))
                    mem = info.get('memory_info')
                    rss = mem.rss if mem else 0
                    by_name[name]['mem_rss'] += rss
                    by_name[name]['mem_percent'] += (info.get('memory_percent') or 0.0)
                    by_name[name]['pids'].append(info.get('pid'))
                except Exception:
                    continue

            with self._lock:
                self.aggregated = {
                    name: {'mem_rss': v['mem_rss'], 'mem_percent': v['mem_percent'], 'pids': v['pids']}
                    for name, v in by_name.items()
                }

            time.sleep(self.sample_interval)

    def start(self):
        if self._running:
            return
        self._running = True
        self._thread = threading.Thread(target=self._sample, daemon=True)
        self._thread.start()

    def stop(self):
        self._running = False
        if self._thread:
            self._thread.join(timeout=2)

    def get_top_memory(self, n=20):
        # 返回按 mem_rss 降序排序的 top n 条目
        # 返回值为列表，元素形如 {'name': ..., 'mem_rss': ..., 'mem_percent': ..., 'pids': [...]}
        with self._lock:
            items = [(name, data) for name, data in self.aggregated.items()]
        items.sort(key=lambda x: x[1]['mem_rss'], reverse=True)
        out = []
        for name, data in items[:n]:
            out.append({'name': name, 'mem_rss': data['mem_rss'], 'mem_percent': data['mem_percent'], 'pids': data['pids']})
        return out
