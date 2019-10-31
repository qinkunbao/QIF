#!/usr/bin/env python3
# -*- coding: UTF-8 -*-

import os
import sys

basicPath = './'
inputPath = basicPath + 'result_OCT17/'
resultPath = basicPath + 'form/latex/'


def data2form(finame, foname, enc, library, version):
    sys.stdout.write(foname)
    sys.stdout.flush()

    fi = open(finame, 'r')
    buf = fi.readline()
    fo = open(foname, 'w')
    fo.write('\\begin{table}[h!]\n' +
             '\\centering\\tiny\\scriptsize\n' +
             '\\renewcommand{\\baselinestretch}{0.96}\\selectfont\n' +
             '\\caption{Leakages in ' + enc +
             ' implemented by ' + library + ' ' + version + '}'
             '\\label{tab:' + enc + library + version + '}\n' +
             '%\\resizebox{\\columnwidth}{!}{\n' +
             '\\begin{tabular}{lrlrr}\n\\hline\n')

    fo.write('\\textbf{File} & ' +
             '\\textbf{Line No.} & ' +
             '\\textbf{Function} & ')

    if enc == "RSA" and library == "openssl" and version == "1.1.1":
        fo.write('\\hspace*{-20em}')

    fo.write('\\textbf{\# Leaked Bits} & ' +
             '\\textbf{Type} ' +
             '\\\\\\hline\n')
    #  '\\textbf{Location} ' + '\\\\\\hline\n')

    while buf and buf[:8] != '--------':
        buf = fi.readline()

    # buf = fi.readline()

    while buf and buf[:8] != 'Address:':
        buf = fi.readline()

    while buf:
        if buf[17:22] == 'Monte':
            leakage = '*'
            ltype = ''
        else:
            leakage = buf[24:26]
            ltype = buf[buf.find('Ty') + 6:buf.find('Ty') + 8]

        buf = fi.readline()

        lfile = buf[buf[13:].find(
            ':') + 15: buf[buf[13:].find(':') + 15:].find(' ') + buf[13:].find(':') + 15]
        lfile = lfile.replace('_', '\\_')

        lnumber = buf[buf.find(' ', len(buf) - 6):-1]

        buf = fi.readline()

        lfunction = buf[16:16+buf[16:].find(' ')]
        lfunction = lfunction.replace('_', '\\_')

        fo.write(lfile + '&' + lnumber + '&' + lfunction + '&' +
                 leakage + '&' + ltype + '\\\\\n')
        #  leakage + '&' + ltype + '& \\\\\n')

        # buf = fi.readline()
        while buf and buf[:8] != 'Address:':
            buf = fi.readline()

    fo.write('\\hline\n' +
             '\\end{tabular}\n%}\n' +
             '\\renewcommand{\\baselinestretch}{1.0}\\selectfont\n' +
             '\\end{table}\n')

    fo.close()
    fi.close()

    sys.stdout.write(' done.\n')
    sys.stdout.flush()


for enc in ['AES', 'DES', 'RSA']:
    # mbedTLS
    for version in ['2.5', '2.15.1']:
        finame = inputPath+'result_' + enc + '-mbedTLS-' + version + 'Inst_data.txt'
        foname = enc + '.mbedTLS.' + version
        foname = resultPath + foname.replace('.', '-') + '.tex'
        data2form(finame, foname, enc, 'mbedTLS', version)

    # openssl
    for version in ['0.9.7', '1.0.2f', '1.0.2k', '1.1.0f', '1.1.1']:
        finame = inputPath+'result_' + enc + '-openssl-' + version + 'Inst_data.txt'
        foname = enc + '.openssl.' + version
        foname = resultPath + foname.replace('.', '-') + '.tex'
        data2form(finame, foname, enc, 'openssl', version)
