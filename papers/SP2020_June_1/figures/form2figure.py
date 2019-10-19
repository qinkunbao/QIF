#!/usr/bin/env python2
# -*- coding: UTF-8 -*-
import matplotlib

matplotlib.use('pdf')
basicPath = './'
formPath = basicPath + 'form/'
figurePath = basicPath + 'figure/'


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
    plt.figure(figsize=(11, 4.5))
    yax = plt.axes().yaxis
    yax.set_ticks_position('none')
    yax.set_tick_params(pad=10)
    # xax = plt.axes().xaxis
    # xax.set_ticks_position('none')
    # xax.set_tick_params(pad=10)
    dataLength = len(data) - 2
    while data[dataLength] == 0:
        dataLength -= 1
    dataLength += 2
    xtitles = range(dataLength)
    xtitles[-1] = 'Failed'
    plt.xticks(range(dataLength), xtitles)
    data[dataLength-1] = data[-1]
    plt.bar(range(dataLength),
            data[:dataLength], align='center', color='#377ba8')
    plt.xticks(fontsize=22)
    plt.ylabel('Number of Leakages', fontsize=22)
    plt.xlabel('Size of Leakages', fontsize=22)
    plt.tight_layout()
    plt.savefig(figurePath + formName + '.pdf', format='pdf')
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