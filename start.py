import os
import re


def get_all_files(sub):
    file_list = []

    if not os.path.isdir(sub):
        print "Invalid argument: '" + sub + "' is not a dir or not found"
        exit(-1)

    for dirpath, dirnames, filenames in os.walk(sub):
        for name in filenames:
            if re.match(".*\.txt", name):
                continue
            if re.match(".*\.ref", name):
                sample = os.path.join(dirpath, name)
                file_list.append(sample)
    return file_list


if __name__ == '__main__':

    file_list = get_all_files("results")

    for each_file in file_list:
        cmd = "./SE " + each_file + "> " + each_file + ".txt"
        os.system(cmd) 