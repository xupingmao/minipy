# encoding=utf-8
import os
import sys

def run_command(command=""):
    ret_code = os.system(command)
    if ret_code != 0:
        sys.exit(ret_code)

def run_debug():
    run_command("make debug-gc")
    run_command("minipy test/test_case/001_test_assignment.py")
    run_command(f"{sys.executable} script/analyze_gc_debug.py")

run_debug()
