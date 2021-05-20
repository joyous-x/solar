#!/usr/bin/python3

import subprocess


def run_apps():
    servers = [
        ["pipenv", "run", "gunicorn", "-c", "config/gunicorn.py", "--worker-class", "sanic.worker.GunicornWorker", "server:app"],
        ["python3", "scheduled_task.py"]
    ]
    procs = []
    for server in servers:
        proc = subprocess.Popen(server)
        procs.append(proc)
    for proc in procs:
        proc.wait()
        if proc.poll():
            exit(0)


if __name__ == '__main__':
    run_apps()