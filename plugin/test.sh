#!/bin/bash
clang -Xclang -load -Xclang ../cmake_clang/build/lib/libplugin.so -Xclang -plugin -Xclang print-fns  -c test/testprintfuncname.c
