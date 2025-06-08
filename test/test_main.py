# -*- coding:utf-8 -*-
# @author xupingmao
# @since 2016
# @modified 2022/01/12 22:59:45
import os
import sys
sys.path.append("src/python")

try:
    import mp_debug
    import mp_init
    print_exc = traceback
except ImportError:
    # python标准环境
    mp_init = None
    mp_debug = None
    from boot import load
    from traceback import print_exc


def exec_file(globals:dict, name: str):
    if mp_init is None:
        if not name.endswith(".py"):
            name += ".py"
        code = load(name)
        return exec(code, globals)
    else:
        return mp_init._import(globals, name)


test_dir = os.path.dirname(__file__)
if test_dir not in sys.path:
    sys.path.append(test_dir)

def testfiles():
    for filename in os.listdir("test/test_case"):
        if not filename.endswith(".py"):
            continue

        basename = filename[:-3]
        fpath   = "test/test_case/" + filename
        modpath = "test/test_case/" + basename
        
        code = load(fpath)
        first_line = code.split("\n")[0]
        if "skip" in first_line:
            print("skip file", fpath)
            continue

        try:
            g = {}
            print("\n\n")
            print("File: %s" % fpath)
            print("-" * 60)
            exec_file(g, modpath)
        except Exception as e:
            print("test failed -- " + fpath)
            print_exc()
            exit(1)
            return
            
    print("="*50)
    print("\n"*2)
    print("all tests passed!!!")
    exit(0)
    
testfiles()