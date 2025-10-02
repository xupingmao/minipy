import sys
import os

if "./mp2c" not in sys.path:
    sys.path.append("./mp2c")


def execute_cmd(params):
    os.system(sys.executable + params)

execute_cmd(" src/python/mp_encode.py -const_name minipyc_bin -c_code mp2c/minipyc.py > mp2c/gen/minipyc_bin.h")
execute_cmd(" src/python/mp_encode.py -const_name mp2c_bin -c_code mp2c/mp2c.py > mp2c/gen/mp2c_bin.h")
os.system("gcc -DTM_USE_CACHE -O2 -o build/minipy-aot mp2c/minipyc.c -lm")