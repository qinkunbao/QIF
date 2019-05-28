#!/usr/bin/env python2

import os
import sys
import pprint
import time
from multiprocessing import Pool
import argparse
import  re

# This script compares candidate (target) loops to a set of reference loops. 
# Output is logged to the two files below. 

START_TIME = int(time.time())
LOG_FILE = "start-py-" + str(START_TIME) + ".log"
SE_LOG = "symbolic-ex-" + str(START_TIME) + ".log"
SIZE_THRESHOLD = 2000


def get_all_files(sub):
    file_list = []

    if not os.path.isdir(sub):
        print "Invalid argument: '" + sub + "' is not a dir or not found"
        exit(-1)

    for dirpath, dirnames, filenames in os.walk(sub):
        for name in filenames:
            if re.match(".*\.ref", name):
                sample = os.path.join(dirpath, name)
                file_list.append(sample)
    return file_list


def start_run(worker_id, ref_list, target_list, log_file):
    total = len(ref_list)*len(target_list)
    count = 0

    fd = open(log_file, 'a', 0)

    log("Worker " + str(worker_id) + " starting, with " + str(total) + " comparisons to make.", 1, fd)

    for ref in ref_list:
        for target in target_list:
            count += 1 
            if os.path.getsize(ref) > 100000:
                log("Warning: skipping large ref file " + ref, 1, fd)
                continue;

            if args.t and abs(os.path.getsize(ref) - os.path.getsize(target)) > SIZE_THRESHOLD:
                #log("Skipping pair of files (" + ref + ", " + target + "), their sizes are too different", 0, fd)
                continue;

            cmd = "./SymbolicExecution " + ref + " " + target + " >> " + SE_LOG
            log(str(worker_id) + ", " + str(count) + ": " + cmd, 0, fd)
            ret = os.system(cmd)
            if ret != 0 :
                log("Command failed: '" + cmd + "' failed with exit code " + str(ret), 1, fd)

            if count % 10 == 0:
                run_time = int( ((time.time()) - START_TIME) )
                log("Worker " + str(worker_id) + " completed " + str(count) + " of " + str(total) + " comparisons (" + str(count/float(total)*100.0) + " percent). Total runtime about " + str(run_time) + " seconds.", 1, fd)

    fd.close()

def log(msg, stdout_too, fd):
    fd.write(msg + "\n")
    if(stdout_too):
        print msg

def chunks(l, n):
    """Yield successive n-sized chunks from l."""
    for i in xrange(0, len(l), n):
         yield l[i:i + n]            

if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    parser.add_argument('ref_list')
    parser.add_argument('target_list')
    parser.add_argument('-t', action="store_true", help="Use size threhold, only compare loops that are similar in size")
    parser.add_argument('-p', type=int, help="Number of processes to start")
    args = parser.parse_args()

    num_process = 8
    if args.p :
        num_process = args.p

    if len(sys.argv) < 3:
        print "Usage python " + sys.argv[0] + " <Reference> <Target> [-f]"
        print "    -f Only compare files with sizes within " + str(SIZE_THREHOLD) + " bytes of each other"
        print "Make sure you run this from the bulid directory"
        exit(-1)

    if not os.path.exists("./SymbolicExecution") or not os.path.isfile("./SymbolicExecution"):
        print "program ./SymbolicExecution not found. Make sure you're running from the build directory"
        exit(-1);

    ref_list = get_all_files(args.ref_list)
    if len(ref_list) == 0:
        print "No references found in " + args.ref_list
        exit(-1)

    target_list = get_all_files(args.target_list)
    if len(target_list) == 0:
        print "No references found in " + args.target_list
        exit(-1)

    fd = open(LOG_FILE, 'a', 0)

    if args.t:
        log("Using size threshold of " + str(SIZE_THRESHOLD) + " bytes before comparing loop files", 1, fd);

    log("Starting run with " + str(len(ref_list)) + " reference loops and " + str(len(target_list)) + " candidate loops (" + str(len(ref_list)*len(target_list)) + ") total", 1, fd)

    if len(target_list) < num_process:
        num_process = len(target_list)
    task = list(chunks(target_list, len(target_list)/num_process + 1))

    log("Using " + str(num_process) + " processes", 1, fd) 

    p = Pool(num_process)
    for i in range(len(task)):
        p.apply_async(start_run, args=(i, ref_list, task[i],LOG_FILE))
        # Note, if this fails; there may be a syntax error in start_run that
        # will silently cause a failure. To debug, change apply_async, to apply
    p.close()
    p.join()

    log("Finished, ran for " + str(int(time.time() - START_TIME)) + " seconds", 1, fd)
    fd.close()

    exit(0)
