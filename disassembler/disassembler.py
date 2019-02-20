#!/usr/bin/env python2

import sys
import r2pipe
import json
import os

<<<<<<< HEAD
=======

>>>>>>> 87e7a135fdd3fe5ffe1bd6714e2a79056d5304d7
## This script will disassemble the malweare, and find possible functions that actually do the encryption
FUN_SIZE_THRESHOLD = 100
BLOCK_SIZE_THRESHOLD = 40
CALL_THRESHOLD = 2

<<<<<<< HEAD
TMP_DIRECTORIES = './target/'


def serialize(data):
    _data = []
    for block in data:
        _block = {}
        block['end_addr'] = block['addr'] + block['size']
        for k, v in block.items():
            if isinstance(v, int):
                _block[k] = hex(v)
            else:
                _block[k] = v
        _data.append(_block)
    return _data


if not os.path.exists(TMP_DIRECTORIES):
    os.makedirs(TMP_DIRECTORIES)
=======
>>>>>>> 87e7a135fdd3fe5ffe1bd6714e2a79056d5304d7

## Analyze the binary and get the fucntion information
if len(sys.argv) != 2:
    print "Usage python " + sys.argv[0] + " <Target>"
    print "Make sure you run this from the working directory"
    exit(-1)

file_name = str(sys.argv[1])

r = r2pipe.open(file_name)
r.cmd('aaaa')
<<<<<<< HEAD
loc_data = TMP_DIRECTORIES + 'data.json'
r.cmd('aflj~{} > ' + loc_data)

with open(loc_data) as f:
    data = json.load(f)

=======
r.cmd('aflj > data.json')


with open('data.json') as f:
    data = json.load(f)


>>>>>>> 87e7a135fdd3fe5ffe1bd6714e2a79056d5304d7
data = [x for x in data if x['size'] > FUN_SIZE_THRESHOLD]
data = [x for x in data if x['outdegree'] < 2]

for fun in data:
    addr = fun["offset"]
    size = fun["size"]
<<<<<<< HEAD
    fun_name = fun['name']
    fun_cmd = "s " + str(hex(addr))
    r.cmd(fun_cmd)
    disa_fun_cmd = "pdf  > " + TMP_DIRECTORIES + fun_name + '.txt'
    r.cmd(disa_fun_cmd)
    r.cmd('abj~{} > ' + TMP_DIRECTORIES + 'blocks_tmp.json')

    with open(TMP_DIRECTORIES + 'blocks_tmp.json') as f:
=======
    fun_name = fun['name'] + '.txt'
    fun_cmd = "s " + str(hex(addr))
    r.cmd(fun_cmd)
    disa_fun_cmd = "pdf  > " + fun_name
    r.cmd(disa_fun_cmd)
    r.cmd('abj~{} > blocks_tmp.json')

    with open('blocks_tmp.json') as f:
>>>>>>> 87e7a135fdd3fe5ffe1bd6714e2a79056d5304d7
        blocks = json.load(f)

    blocks = [x for x in blocks if x['size'] > BLOCK_SIZE_THRESHOLD]
    for block in blocks:
        block_start = block['addr']
<<<<<<< HEAD
        block_end = block_start + block['size']

    blocks_name = TMP_DIRECTORIES + fun_name + "_blocks.json"

    with open(blocks_name, 'w') as f:
        blocks = serialize(blocks)
        json.dump(blocks, f, indent=4, sort_keys=True)
=======
        block_end = block_start+ block['size']

    blocks_name = fun_name + "_blocks.json"

    with open(blocks_name, 'w') as f:
        json.dump(blocks, f)




>>>>>>> 87e7a135fdd3fe5ffe1bd6714e2a79056d5304d7
