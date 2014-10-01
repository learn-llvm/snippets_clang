#!/usr/bin/python3

import os
import shutil

myfile = '/home/hongxu/repos/snippets_clang/tooling/clang_check/CMakeLists.txt'

current_dir = os.path.dirname(os.path.realpath(__file__))
for f in os.listdir(current_dir):
    if f.endswith(".cc"):
        prefix = f[:-3]
        if not os.path.isfile(prefix):
            os.mkdir(prefix)
        shutil.move(f, os.path.join(current_dir, prefix))
        shutil.copy(myfile, os.path.join(current_dir, prefix))

for f in os.listdir(current_dir):
    if os.path.isdir(f):
        shutil.copy(myfile, f)
