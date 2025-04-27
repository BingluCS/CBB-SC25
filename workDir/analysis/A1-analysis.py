import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import sys
from pathlib import Path

from mpl_toolkits.axes_grid1.inset_locator import inset_axes, mark_inset

data_cbb, data_zlib = [], []
app = sys.argv[1]

path = Path(f'../{app}Dir/{app}_cpu-nocomp.log0')
if path.exists():
    data_cbb = pd.read_csv(path, header=None)
else:
    print(f"{path} not exists")

data_cbb_sums = (data_cbb.sum(axis=1)) 
data_cbb_sums = data_cbb_sums / 8

path = Path(f'../{app}Dir/{app}_cpu-zlib.log0')
if path.exists():
    data_zlib = pd.read_csv(path, header=None)
else:
    print(f"{path} not exists")


data_zlib_sums = (data_zlib.sum(axis=1))
data_zlib_sums = data_zlib_sums / 8

x_cbb = np.arange(0,len(data_cbb_sums)/5,0.2)
x_zlib = np.arange(0,len(data_zlib_sums)/5,0.2)

target_index = range(len(data_zlib_sums))
data_cbb_sums = data_cbb_sums.reindex(target_index, fill_value=0)

p0 = plt.plot(x_zlib, data_cbb_sums, label='CBB', color="red")
p1 = plt.plot(x_zlib, data_zlib_sums, label='Zlib', color="#1d5dec")


a = plt.legend([p0[0]], ['CBB'],bbox_to_anchor=(-0.01, 1.01), borderaxespad=0,frameon=False,loc=3,fontsize=20)
plt.legend([p1[0]], ['Zlib'],bbox_to_anchor=(1.01, 1.01), borderaxespad=0,frameon=False,loc=4,fontsize=20)
plt.gca().add_artist(a)


plt.gcf().set_size_inches(8,4.8) 
plt.xticks(fontsize=20)
plt.yticks(fontsize=20)
plt.xlabel('Time (Second)', fontsize=20)
plt.ylabel('CPU Utilization (%)', fontsize=20)
plt.savefig(app + "_CPU.pdf", bbox_inches='tight')