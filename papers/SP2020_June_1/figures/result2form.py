#!/usr/bin/env python2
# -*- coding: UTF-8 -*-

import os
import sys

basicPath = './'
resultPath = basicPath + 'form/'


def data2form(finame, foname):
    fi = open(finame, 'r')
    buf = fi.readline()
    fo = open(foname, 'w')
    fo.write('| File | Line Num | Function | Leaked bits | Type | Location |\n' +
             '| :--: | :------: | :------: | :---------: | :--: | :------: |\n')
    while buf[:8] != 'DETAILS:':
        while buf[:8] != 'Address:':
            buf = fi.readline()

        if buf[17:22] == 'Monte':
            leakage = 'Failed'
            ltype = ''
        else:
            leakage = buf[24:26]
            ltype = buf[buf.find('Ty') + 6:buf.find('Ty') + 8]

        buf = fi.readline()

        lfile = buf[buf[13:].find(
            ':') + 15: buf[buf[13:].find(':') + 15:].find(' ') + buf[13:].find(':') + 15]
        lnumber = buf[-4:-1]
        fo.write('|' + lfile + '|' + lnumber + '||' +
                 leakage + '|' + ltype + '||\n')

        buf = fi.readline()

    fo.close()
    fi.close()


for enc in ['AES', 'DES', 'RSA']:
    # mbedTLS
    for version in ['2.5', '2.15.1']:
        finame = 'result_' + enc + '-mbedTLS-' + version + 'Inst_data.txt'
        foname = resultPath + enc + '.mbedTLS.' + version
        data2form(finame, foname)

    # openssl
    for version in ['0.9.7', '1.0.2f', '1.0.2k', '1.1.0f', '1.1.1']:
        finame = 'result_' + enc + '-openssl-' + version + 'Inst_data.txt'
        foname = resultPath + enc + '.openssl.' + version
        data2form(finame, foname)
