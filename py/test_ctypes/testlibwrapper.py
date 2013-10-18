#!/usr/bin/python3

import ctypes
import os

testlib = ctypes.CDLL(os.getcwd() + "/testlib.so")
testlib.myprint()
