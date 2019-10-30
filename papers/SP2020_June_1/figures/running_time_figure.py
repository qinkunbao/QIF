#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
import matplotlib
import numpy as np
import csv

matplotlib.use('pdf')
basicPath = './'
formPath = basicPath + 'form/'
figurePath = basicPath + 'result/'


def count(formName):
    with open(formPath + formName + '.csv', 'r') as se_optimized:
        f_csv = csv.reader(se_optimized)
        time = next(f_csv)

    time = time[:-1]

    for i in range(len(time)):
        # time[i] = int(time[i].replace('.', ''))
        # # time[i] += time[i-1]
        # time[i] = time[i]/10e5
        time[i] = float(time[i])

    print(time)
    return time


def plot(formName, line1, line2):
    import matplotlib.pyplot as plt
    plt.figure(figsize=(5, 5))

    plt.plot(range(len(line1)), line1, color='grey')
    plt.plot(range(len(line2)), line2, color='grey')

    plt.xticks(fontsize=9)
    plt.yticks(fontsize=9)
    plt.ylabel('Time (s)', fontsize=11)
    plt.xlabel('Number of Instructions (10e4)', fontsize=11)
    # plt.title(formName.replace('.', ' ', 2), fontsize = 10)
    plt.tight_layout()
    plt.savefig(figurePath + formName.replace('.', '-') +
                '.pdf', format='pdf')
    plt.close('all')


time1 = count('time_openssl1.1.1')
time2 = []
# for i in range(len(time1)-1):
#     time2.append(time1[i + 1] - time1[i])

# print(time2)
# time2 = count('se_non_optimized')
# time2 = []
# time2[0] += 1
plot('running_time', time1, time2)
