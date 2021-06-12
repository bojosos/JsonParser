#!/usr/bin/env python3

import subprocess
import os
import time
import sys

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

start = time.time()
complete = subprocess.run('clang++ -O2 json.cpp test.cpp parser.cpp -o parser', shell=True)
if complete.stderr is not None:
   print('Compilation failed')
   exit(0)
print(bcolors.HEADER + 'Compilation took %f seconds' % (time.time() - start))

start = time.time()

test_count = 0

for root, dirs, files in os.walk("tests"):
    for name in files:
        test_count += 1
        complete = subprocess.check_output(['./parser', './tests/' + name], stderr=subprocess.STDOUT)
        if not complete or name.startswith('n_'):
            if name.startswith('n_'):
                print(bcolors.OKGREEN + 'n', end='')
            else:
                print(bcolors.OKGREEN + 'y', end='')
        else:
            print(bcolors.FAIL + 'e', end='')
            if len(sys.argv) > 1 and sys.argv[1] == '--more':
                print(bcolors.FAIL + 'test %s failed' % ("tests/" + name))
                ### print program output vs test
                file = open('./tests/' + name).read()
                print(file)
                complete = subprocess.run('./parser ' + './tests/' + name + ' ' + '--output', shell=True)

print()
print(bcolors.HEADER + "Ran %d tests in %f seconds" % (test_count, time.time() - start))
