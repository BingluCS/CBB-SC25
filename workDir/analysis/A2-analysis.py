import re
import sys
import numpy as np
import pandas as pd
import os

app = sys.argv[1]
Type  = ['nocomp', 'cbb', 'zlib', 'szcbb']
pattern_input = r"restart time = (\d+\.\d+)\s+seconds"
pattern_meta = r"deal = (\d+\.\d+)\s+seconds"
pattern_out = r"real write time = (\d+\.\d+)\s+seconds"
pattern_prefetch = r'Prefetch Time elapsed: (\d+\.\d+) seconds'
pattern_flush = r'Flush Time elapsed: (\d+\.\d+) seconds'
pattern_dir = r"dir time = (\d+\.\d+)\s+seconds"
pattern_nyx0 = r"Write h5plotfile time =\s+(\d+\.\d+)"
patteern_nyx1 = r"Write plotfile time =\s+(\d+\.\d+)"
pattern_total= r'Total Time\s*: (\d+\.\d+)'

increment = 25
if(app == 'ferrox'):
    pattern_total= r'Total run time\s+(\d+\.\d+)'
elif (app == 'nyx'):
    pattern_total= r'Run time =\s+(\d+\.\d+)'
    pattern_input = r'read CPU time:\s+(\d+\.\d+)'
    increment = 10
elif (app == 'maestroex'):
    pattern_input = r"Restart time = (\d+\.\d+)\s+seconds"
    increment = 10


all = []
time_csv = []
tim = 0
for t in Type:
    for i in range(0, 11):
        filename =  '../' + app + 'Dir/' + app + '-' + t + '-' + str(i)
        if os.path.exists(filename):
            with open(filename, "r") as file:
                data = file.read().split('\n')
        else:
            continue
        keys = ['app', 'size', 'type', 'read', 'write+compression', 'unuse', 'compute', 'prefetch', 'flush', 'total']
        time_list = {'in_time': 0, 'out_time': 0, 'unuse_time': 0, 'compute_time': 0, 
        'prefetch_time' : 0, 'flush_time' : 0, 'total': 0}

        for line in data:
            if re.search(pattern_meta, line):
                match = re.search(pattern_meta, line)
                time_list['out_time'] += float(match.group(1))
                
            elif re.search(pattern_out, line):
                match = re.search(pattern_out, line)
                time_list['out_time'] += float(match.group(1))
                #step_num += 1

            elif re.search(pattern_dir, line):
                match = re.search(pattern_dir, line)
                time_list['out_time'] += float(match.group(1))
                tim += float(match.group(1))
            elif re.search(pattern_nyx0, line) and app =='nyx' :
                match = re.search(pattern_nyx0, line)
                time_list['out_time'] -= float(match.group(1))
            
            elif re.search(patteern_nyx1, line) and app =='nyx':
                match = re.search(patteern_nyx1, line)
                time_list['out_time'] += float(match.group(1))

            elif re.search(pattern_input, line):
                match = re.search(pattern_input, line)
                time_list['in_time'] += float(match.group(1))

            elif re.search(pattern_prefetch, line):
                match = re.search(pattern_prefetch, line)
                time_list['prefetch_time'] += float(match.group(1))

            elif re.search(pattern_flush, line):
                match = re.search(pattern_flush, line)
                time_list['flush_time'] += float(match.group(1))

            elif re.search(pattern_total, line):
                match = re.search(pattern_total, line)
                time_csv.append(app)
                time_csv.append(i * increment)
                time_csv.append(t)
                time_list['total'] = float(match.group(1))
                for key, value in time_list.items():
                    if(key == 'compute_time'):
                        time_list[key] = time_list['total'] - time_list['in_time'] - time_list['out_time'] - time_list['prefetch_time'] - time_list['flush_time']
                    elif key == 'in_time':
                        if app == 'maestroex':
                            time_list[key] = time_list[key]
                        elif app == 'nyx' :
                            time_list[key] = time_list[key] / 1000_000
                        else :
                            time_list[key] = time_list[key] - time_list['prefetch_time']
                    time_csv.append(time_list[key])
                for key, value in time_list.items():
                    time_list[key] = 0

time_csv = np.array(time_csv).reshape(-1, 10).T
time_csv = pd.DataFrame(dict(zip(keys, time_csv)))
time_csv.to_csv(app + '.csv', index=False)