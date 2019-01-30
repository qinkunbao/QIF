#!/usr/bin/env python2

import sys
import r2pipe
import json
import os


## This script will disassemble the malweare, and find possible functions that actually do the encryption
FUN_SIZE_THRESHOLD = 100
BLOCK_SIZE_THRESHOLD = 40
CALL_THRESHOLD = 2


## Analyze the binary and get the fucntion information
if len(sys.argv) != 2:
    print "Usage python " + sys.argv[0] + " <Target>"
    print "Make sure you run this from the working directory"
    exit(-1)

file_name = str(sys.argv[1])

r = r2pipe.open(file_name)
r.cmd('aaaa')
r.cmd('aflj > data.json')


with open('data.json') as f:
    data = json.load(f)


data = [x for x in data if x['size'] > FUN_SIZE_THRESHOLD]
data = [x for x in data if x['outdegree'] < 2]

for fun in data:
    addr = fun["offset"]
    size = fun["size"]
    fun_name = fun['name'] + '.txt'
    fun_cmd = "s " + str(hex(addr))
    r.cmd(fun_cmd)
    disa_fun_cmd = "pdf  > " + fun_name
    r.cmd(disa_fun_cmd)
    r.cmd('abj~{} > blocks_tmp.json')

    with open('blocks_tmp.json') as f:
        blocks = json.load(f)

    blocks = [x for x in blocks if x['size'] > BLOCK_SIZE_THRESHOLD]
    for block in blocks:
        block_start = block['addr']
        block_end = block_start+ block['size']

    blocks_name = fun_name + "_blocks.json"

    with open(blocks_name, 'w') as f:
        json.dump(blocks, f)




