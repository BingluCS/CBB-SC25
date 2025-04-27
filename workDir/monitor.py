import sys
import psutil
import mpi4py
from mpi4py import MPI
import time
import subprocess
from mpi4py import MPI
cpu_utilization = []
def get_cpu_utilization(pid):
    try:
        p = psutil.Process(pid)
    except psutil.NoSuchProcess:
        print(f"No process with PID {pid}")
        sys.exit(1)
    cpu_utilization = []
    while p.is_running() and p.status() != psutil.STATUS_ZOMBIE:
        cpu = psutil.cpu_percent(interval=0.2, percpu=True)
        cpu_utilization.append(cpu)
        # mem = p.memory_info().rss / 1024 / 1024  # 转换为 MB
        # timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        # f.write(f"{timestamp}\t{cpu:.2f}\t{mem:.2f}\n")
        # f.flush()
    return cpu_utilization

def save_cpu_utilization_data(cpu_utilization_data, filename):
    rank = MPI.COMM_WORLD.Get_rank()
    with open(filename + str(rank), 'w') as file:
        for utilization in cpu_utilization_data:
            file.write(','.join(map(str, utilization)) + '\n')


if len(sys.argv) != 3:
    print("Usage: python monitor.py <pid> <logfile>")
    sys.exit(1)

cpu_utilization_data = get_cpu_utilization(int(sys.argv[1]))
save_cpu_utilization_data(cpu_utilization_data, sys.argv[2])
