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
    time[0] = int(time[0].replace('.', ''))
    for i in range(1, len(time)):
        time[i] = int(time[i].replace('.', ''))
        time[i] += time[i-1]

    for i in range(len(time)):
        time[i] = time[i]/10e5

    print(time)
    return time


def plot(formName, line1, line2):
    import matplotlib.pyplot as plt
    plt.figure(figsize=(5, 5))

    plt.plot(range(len(line1)), line1, color='grey')
    plt.plot(range(len(line2)), line2, color='grey')

    plt.xticks(fontsize=18)
    plt.yticks(fontsize=18)
    plt.ylabel('Time (s)', fontsize=22)
    plt.xlabel('Number of Instructions (10e4)', fontsize=22)
    # plt.title(formName.replace('.', ' ', 2), fontsize = 10)
    plt.tight_layout()
    plt.savefig(figurePath + formName.replace('.', '-') +
                '.pdf', format='pdf')
    plt.close('all')


time1 = count('se_optimized')
time2 = count('se_non_optimized')
plot('running_time', time1, time2)
