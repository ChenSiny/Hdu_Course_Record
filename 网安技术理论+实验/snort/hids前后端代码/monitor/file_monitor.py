import threading
import time
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler
import os


"""
文件监控模块（FileMonitor）

职责：使用 watchdog（基于 inotify）监控指定文件或目录，将发生的事件记录到内存列表中供外部读取。

设计要点：
- 事件处理交给 _Handler.on_any_event，将事件信息追加到 self.events。
- 提供运行时修改监控路径的能力（set_path），会在需要时停止并重启 observer。
- get_events(limit) 返回最近若干条事件（倒序），并使用锁保护读写一致性。

注意：inotify 可以检测 create/modify/delete/move 等事件，但不能保证捕获每次 read syscall；如果需要 syscall 级别的监控，应使用 audit 或 eBPF。
"""


class _Handler(FileSystemEventHandler):
    def __init__(self, event_list, path):
        super().__init__()
        # 事件将被追加到外部传入的 list 对象
        self.event_list = event_list
        self.path = path

    def on_any_event(self, event):
        # 当发生任何文件系统事件时，记录简要信息到事件列表中
        # 记录字段：time（时间戳），event_type（字符串），is_directory（bool），src_path（触发路径）
        try:
            self.event_list.append({'time': time.time(), 'event_type': event.event_type, 'is_directory': event.is_directory, 'src_path': event.src_path})
        except Exception:
            # 保持鲁棒性：监控不应因为个别事件导致崩溃
            pass


class FileMonitor:
    def __init__(self, path_to_watch=None):
        # 默认监控路径；可在运行时通过 set_path 修改
        self.path = path_to_watch or '/tmp/monitored_file'
        # 存储事件的列表（append-only），外部通过 get_events 读取
        self.events = []
        self.observer = None
        self._lock = threading.Lock()
        self._running = False

    def _start_observer(self):
        # 内部方法：根据当前 self.path 创建并启动 Observer
        handler = _Handler(self.events, self.path)
        self.observer = Observer()
        # 如果 path 是文件，watch 其父目录；否则直接 watch 目录
        watch_path = self.path if os.path.isdir(self.path) else os.path.dirname(self.path) or '.'
        self.observer.schedule(handler, watch_path, recursive=False)
        self.observer.daemon = True
        self.observer.start()
        self._running = True

    def start(self):
        # 启动 observer（如果尚未启动）
        with self._lock:
            if self.observer and self._running:
                return
            try:
                self._start_observer()
            except Exception:
                # 如果启动失败，保持模块可用但 observer 为空
                self.observer = None
                self._running = False

    def stop(self):
        # 停止并清理 observer
        with self._lock:
            if self.observer:
                try:
                    self.observer.stop()
                    self.observer.join(timeout=2)
                except Exception:
                    pass
                self.observer = None
            self._running = False

    def set_path(self, new_path, restart=True):
        """设置新的监控路径。

        参数:
        - new_path: 字符串，新路径（文件或目录）。
        - restart: 如果 True 且 observer 正在运行，则在更改路径后自动重启 observer。
        """
        with self._lock:
            was_running = self._running
            if was_running:
                try:
                    self.observer.stop()
                    self.observer.join(timeout=2)
                except Exception:
                    pass
                self.observer = None
                self._running = False
            self.path = new_path
            if restart and was_running:
                try:
                    self._start_observer()
                except Exception:
                    self.observer = None
                    self._running = False

    def get_path(self):
        # 返回当前监控路径（可能是默认路径或已设置的路径）
        return self.path

    def get_events(self, limit=200):
        # 以线程安全的方式返回最近 limit 条事件（按时间倒序）
        with self._lock:
            return list(self.events[-limit:][::-1])
