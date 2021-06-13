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
complete = subprocess.run('clang++ -Wno-switch -O2 json.cpp utils.cpp test.cpp parser.cpp -o parser', shell=True)
if complete.stderr is not None:
   print('Compilation failed')
   exit(0)
print(bcolors.HEADER + 'Compilation took %f seconds' % (time.time() - start))

def fail(name):
    print(bcolors.FAIL + 'e', end='')
    if len(sys.argv) > 1 and sys.argv[1] == '--more':
        print(bcolors.FAIL + 'test %s failed' % ("tests/" + name))
        ### print program output vs test
        file = open('./tests/' + name).read()
        print(file)
        complete = subprocess.run('./parser ' + './tests/' + name + ' ' + '--output', shell=True)

start = time.time()
test_count = 0

for root, dirs, files in os.walk("tests"):
    for name in files:
        test_count += 1
        try:
            complete = subprocess.check_output(['./parser', './tests/' + name], stderr=subprocess.STDOUT, universal_newlines=True)
            if not complete or name.startswith('n_'):
                if name.startswith('n_'):
                    print(bcolors.OKGREEN + 'n', end='')
                else:
                    print(bcolors.OKGREEN + 'y', end='')
            else:
                fail(name)
        except:
            fail(name)
print()
print(bcolors.HEADER + "Ran %d tests in %f seconds" % (test_count, time.time() - start))

start = time.time()
complete = subprocess.run('clang++ -Wno-switch -O2 interpreter.cpp utils.cpp json.cpp testcmds.cpp parser.cpp -o testcmds', shell=True)
if complete.stderr is not None:
   print('Compilation failed')
   exit(0)
print(bcolors.HEADER + 'Compilation took %f seconds' % (time.time() - start))

try:
    output = subprocess.check_output('./testcmds', stderr=subprocess.STDOUT, shell=True, timeout=3, universal_newlines=True)
except subprocess.CalledProcessError as exc:
    print(bcolors.FAIL + 'Testcmds execution could not complete. ' + str(exc))
else:
    search_compact = open('tests/search.json', 'r').read()
    search_compact_correct = open('tests/search_compact_correct.json').read()
    if search_compact != search_compact_correct:
        print(bcolors.FAIL + 'Search results do not match.')
    else:
        print(bcolors.OKGREEN + 'Search results match.')
    save_compact = open('tests/save.json', 'r').read()
    save_compact_correct = open('tests/save_compact_correct.json').read()
    if save_compact != save_compact_correct:
        print(bcolors.FAIL + 'Save results do not match.')
    print(bcolors.OKGREEN + 'Save results match.')
os.remove('testcmds')
os.remove('parser')
os.remove('tests/search.json')
os.remove('tests/save.json')