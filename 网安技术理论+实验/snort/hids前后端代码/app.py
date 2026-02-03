import threading
import time
from flask import Flask, render_template, jsonify, request

from monitor.process_monitor import ProcessMonitor
from monitor.net_monitor import NetMonitor
from monitor.file_monitor import FileMonitor
import psutil
import subprocess

app = Flask(__name__)

# Shared monitors (singletons for this simple app)
process_monitor = ProcessMonitor(sample_interval=2)
net_monitor = NetMonitor()
file_monitor = FileMonitor()


@app.route('/')
def index():
    return render_template('index.html')


@app.route('/api/memory')
def api_memory():
    top_n = int(request.args.get('n', 20))
    data = process_monitor.get_top_memory(n=top_n)
    return jsonify(data)


@app.route('/api/network')
def api_network():
    # return aggregated network usage for 10 minutes by process name
    window_minutes = int(request.args.get('minutes', 10))
    data = net_monitor.get_usage_window(minutes=window_minutes)
    return jsonify(data)


@app.route('/api/file_events')
def api_file_events():
    return jsonify(file_monitor.get_events())


@app.route('/api/file_watch', methods=['GET', 'POST'])
def api_file_watch():
    # GET: return current watched path; POST: set new path {"path": "/tmp/foo"}
    if request.method == 'GET':
        return jsonify({'path': file_monitor.get_path()})
    else:
        payload = request.get_json(force=True) or {}
        new_path = payload.get('path')
        if not new_path:
            return jsonify({'error': 'missing path'}), 400
        try:
            file_monitor.set_path(new_path)
            return jsonify({'path': file_monitor.get_path()})
        except Exception as e:
            return jsonify({'error': str(e)}), 500


@app.route('/api/startup')
def api_startup():
    # gather systemd enabled services and PPID=1 processes
    services = []
    try:
        out = subprocess.check_output(['systemctl', 'list-unit-files', '--state=enabled', '--no-legend'], text=True)
        for line in out.splitlines():
            parts = line.split()
            if not parts:
                continue
            name = parts[0]
            try:
                active = subprocess.check_output(['systemctl', 'is-active', name], text=True).strip()
            except Exception:
                active = 'unknown'
            services.append({'name': name, 'active': active})
    except Exception:
        # systemd might not be present; fallback empty
        services = []

    ppid1 = []
    for p in psutil.process_iter(['pid', 'name', 'username', 'ppid']):
        try:
            if p.info.get('ppid') == 1:
                ppid1.append(p.info)
        except Exception:
            pass

    return jsonify({'services_enabled': services, 'ppid1_processes': ppid1})


def start_background():
    process_monitor.start()
    net_monitor.start()
    file_monitor.start()


if __name__ == '__main__':
    # start monitors
    start_background()
    app.run(host='0.0.0.0', port=5000, debug=True)
