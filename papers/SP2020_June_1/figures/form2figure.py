#!/usr/bin/env python3
# -*- coding: UTF-8 -*-
import matplotlib
import numpy as np

matplotlib.use('pdf')
basicPath = './'
formPath = basicPath + 'form/'
figurePath = basicPath + 'result/'


def count(formName):
    form = open(formName, 'r')
    buf = form.readline()
    buf = form.readline()
    buf = form.readline()
    while (buf):
        if buf.split('|')[4] != 'Failed':
            data[int(buf.split('|')[4])] += 1
        else:
            data[-1] += 1
        buf = form.readline()

    form.close()


def plot(formName):
    import matplotlib.pyplot as plt
    plt.figure(figsize=(5, 5))
    yax = plt.axes().yaxis
    yax.set_ticks_position('none')
    # yax.set_tick_params(pad=10)
    xax = plt.axes().xaxis
    # xax.set_ticks_position('none')
    # xax.set_tick_params(pad=10)
    dataLength = len(data) - 2
    while data[dataLength] == 0:
        dataLength -= 1
    dataLength += 2

    xtitles = list(range(dataLength))
    if 'RSA' in formName:
        if 'openssl' in formName:
            plt.ylim(0, 80)
            # plt.xlim(0.5, 15.5)
            plt.xlim(-0.5, 15.5)

            for i in range(len(xtitles)):
                if (i % 2 == 0):
                    xtitles[i] = ''

            plt.yticks([10, 30, 50, 70])

    xtitles[-1] = '*'
    plt.xticks(range(dataLength), xtitles)

    failedLable = plt.axes().get_xticklabels()
    failedLable[-1].set_rotation(270)
    data[dataLength-1] = data[-1]
    plt.bar(range(dataLength),
            data[:dataLength], align='center', color='grey', width=0.8)
    # plt.bar(range(1, dataLength),
    #         data[1:dataLength], align='center', color='grey', width=0.8)
    plt.xticks(fontsize=18)
    plt.yticks(fontsize=18)
    plt.ylabel('Number of Leakages', fontsize=22)
    plt.xlabel('Leakage Amount (bits)', fontsize=22)
    # plt.title(formName.replace('.', ' ', 2), fontsize = 10)
    plt.tight_layout()
    plt.savefig(figurePath + formName.replace('.', '-') +
                '.pdf', format='pdf')
    plt.close('all')


for enc in ['AES', 'DES', 'RSA']:
    # mbedTLS
    for version in ['2.5', '2.15.1']:
        formName = enc + '.mbedTLS.' + version
        data = [0] * 16
        count(formPath + formName)
        plot(formName)

    # openssl
    for version in ['0.9.7', '1.0.2f', '1.0.2k', '1.1.0f', '1.1.1']:
        formName = enc + '.openssl.'+version
        data = [0] * 16
        count(formPath + formName)
        plot(formName)
