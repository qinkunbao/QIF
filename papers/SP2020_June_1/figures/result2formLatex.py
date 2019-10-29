#!/usr/bin/env python3
# -*- coding: UTF-8 -*-

import os
import sys

basicPath = './'
resultPath = basicPath + 'form/latex/'

# TODO: get form name


def data2form(finame, foname):
    print(foname + '\n')

    fi = open(finame, 'r')
    buf = fi.readline()
    fo = open(foname, 'w')
    fo.write('\\begin{table}[h]\n' +
             '\\centering\n' +
             '\\caption{}\\label{fig:}\n' +
             '\\begin{tabular}{clrrr}\n\\hline\n')

    fo.write('\\textbf{File} & ' +
             '\\textbf{Line Num} & ' +
             '\\textbf{Function} & ' +
             '\\textbf{Leakedbits} & ' +
             '\\textbf{Type} ' +
             '\\\\\\hline\n')
    #  '\\textbf{Location} ' + '\\\\\\hline\n')

    while buf and buf[:8] != '--------':
        buf = fi.readline()

    # buf = fi.readline()

    while buf:
        while buf and buf[:8] != 'Address:':
            buf = fi.readline()

        # leakage = ''
        # ltype = ''
        # lfile = ''
        # lnumber = ''
        # lfunction = ''

        if buf[17:22] == 'Monte':
            leakage = 'Failed'
            ltype = ''
        else:
            leakage = buf[24:26]
            ltype = buf[buf.find('Ty') + 6:buf.find('Ty') + 8]

        buf = fi.readline()

        lfile = buf[buf[13:].find(
            ':') + 15: buf[buf[13:].find(':') + 15:].find(' ') + buf[13:].find(':') + 15]
        lfile = lfile.replace('_', '\\_')

        lnumber = buf[-4:-1]

        buf = fi.readline()

        lfunction = buf[16:16+buf[16:].find(' ')]
        lfunction = lfunction.replace('_', '\\_')

        fo.write(lfile + '&' + lnumber + '&' + lfunction + '&' +
                 leakage + '&' + ltype + '\\\\\n')
        #  leakage + '&' + ltype + '& \\\\\n')

        # print(lfile + '&' + lnumber + '&' + lfunction + '&' +
        #       leakage + '&' + ltype + '& \\\\\n')

        buf = fi.readline()

    fo.write('\\hline\n' + '\\end{tabular}\n' + '\end{table}')
    fo.close()
    fi.close()
    print(foname + ' done.\n')


for enc in ['AES', 'DES', 'RSA']:
    # mbedTLS
    for version in ['2.5', '2.15.1']:
        finame = 'result_' + enc + '-mbedTLS-' + version + 'Inst_data.txt'
        foname = resultPath + enc + '.mbedTLS.' + version + '.tex'
        data2form(finame, foname)

    # openssl
    for version in ['0.9.7', '1.0.2f', '1.0.2k', '1.1.0f', '1.1.1']:
        finame = 'result_' + enc + '-openssl-' + version + 'Inst_data.txt'
        foname = resultPath + enc + '.openssl.' + version + '.tex'
        data2form(finame, foname)
