import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import os
from scipy.ndimage import gaussian_filter1d
from matplotlib.ticker import FuncFormatter
import sys
ccr_value = 0.3 # 0.1 0.2 0.3 0.4
X_RANGE = np.arange(0.2, 2.2, 0.2).round(1)  

BASE_PATH = '../'

ccr_value = sys.argv[1] # [0.05, 0.1, 0.2, 0.3]

org_file = f"BB_{ccr_value}.csv"
cbb_file = f"CBB_{ccr_value}.csv"


try:
    df_org = pd.read_csv(os.path.join(BASE_PATH, org_file), header=None)
    df_cbb = pd.read_csv(os.path.join(BASE_PATH, cbb_file), header=None)
except FileNotFoundError as e:
    print(f"file not exits: {e}")

results = {
    'line_data': [],
    'bar_data': {
        'BB': [],
        'CBB': []
    }
}

# 处理线图数据（使用前9个指标列）
for df in [df_org, df_cbb]:
    max_col = min(18, df.shape[1]-1)
    for col in range(10, max_col+1):
        segmented = []
        for x_val in X_RANGE:
            mask = (df[0] >= x_val) & (df[0] < x_val + 0.2)
            if mask.any():
                val = df.loc[mask, col].mean()
                segmented.append(val)
            else:
                segmented.append(np.nan)
        segmented = pd.Series(segmented).ffill().values
        results['line_data'].append(gaussian_filter1d(segmented, sigma=2))

# 处理柱状图数据（使用第2列）
for x_val in X_RANGE:
    mask_org = (df_org[0] >= x_val) & (df_org[0] < x_val + 0.2)
    mask_cbb = (df_cbb[0] >= x_val) & (df_cbb[0] < x_val + 0.2)
    
    bb_val = df_org.loc[mask_org, 2].mean() if mask_org.any() else np.nan
    cbb_val = df_cbb.loc[mask_cbb, 2].mean() if mask_cbb.any() else np.nan
    
    results['bar_data']['BB'].append(bb_val)
    results['bar_data']['CBB'].append(cbb_val)

normalized_ratios = []
for bb_val, cbb_val in zip(results['bar_data']['BB'], results['bar_data']['CBB']):
    if pd.notnull(bb_val) and bb_val != 0:
        normalized_ratios.append(cbb_val / bb_val)
    else:
        normalized_ratios.append(np.nan)
            
bar_width, interval = 0.35, 0.1
fig, ax1 = plt.subplots()
result_bb = results['bar_data']['BB'] #[results['bar_data']['BB'][0], results['bar_data']['BB'][2], results['bar_data']['BB'][4], results['bar_data']['BB'][6], results['bar_data']['BB'][8]]
result_cbb = results['bar_data']['CBB'] #[results['bar_data']['CBB'][0], results['bar_data']['CBB'][2], results['bar_data']['CBB'][4], results['bar_data']['CBB'][6], results['bar_data']['CBB'][8]]
x_bb = np.arange(len(result_bb))

p0 = ax1.bar(x_bb, result_bb, width = bar_width, color="#069af3", edgecolor='black')
p1 = ax1.bar(x_bb + (bar_width+ interval), result_cbb,  width = bar_width,color="#fb5581", edgecolor='black')         
ax1.set_ylim(1000)

ax2 = ax1.twinx()
def percent(x, pos):
    return f'{x*100:.0f}%'  #

# normalized_ratios = [normalized_ratios[0], normalized_ratios[2], normalized_ratios[4], normalized_ratios[6], normalized_ratios[8]]
smoothed_ratios = gaussian_filter1d(normalized_ratios, sigma=2)
ax2.yaxis.set_major_formatter(FuncFormatter(percent))

p2 = ax2.plot(x_bb + (bar_width+ interval) *0.5 , [1] * (10), label="BB (%)", linestyle='-', linewidth=2, color="#1d5dec", marker = '.', markersize = 10)
p3 = ax2.plot(x_bb + (bar_width+ interval) * 0.5, smoothed_ratios, label="CBB (%)", linestyle='--', linewidth=2, color="red", marker = '*', markersize = 10)
ax2.set_ylim(0.85,1.01)


def get_anchor(index, total, top=1.01):
    return ((index + 0.5) / total, top)

a = plt.legend([p1[0], p3[0]], ['CBB','CBB'],
           bbox_to_anchor=get_anchor(0, 2),
           loc='lower center', frameon=False,fontsize=16)

x_label = ['0.2','0.4','0.6','0.8','1.0','1.2','1.4','1.6','1.8','2.0']
ax1.set_ylabel("Time (Second)", fontsize=20)
ax1.set_xlabel("More Compute ← CCR → More IO ", fontsize=20)
# # plt.xlabel("Burst Buffers Size (GB)", fontsize=20)
ax1.set_xticks(x_bb+ (bar_width+ interval) * 0.5, x_label)

ax1.tick_params(axis='x', labelsize=16)
ax1.tick_params(axis='y', labelsize=16)
ax2.tick_params(axis='y', labelsize=16)
plt.legend([p0[0], p2[0]], ['BB', 'BB'],
           bbox_to_anchor=get_anchor(1, 2),
           loc='lower center', frameon=False,fontsize=16)
plt.gca().add_artist(a)
# plt.legend([p3[0],p4[0]], ['CBB','CBB-SZ'],
#            bbox_to_anchor=get_anchor(2, 3),
#            loc='lower center', frameon=False,fontsize=16)

plt.gcf().set_size_inches(8,4.8) 
plt.savefig("BB_workflow" + f"{ccr_value}"+".pdf", bbox_inches='tight')