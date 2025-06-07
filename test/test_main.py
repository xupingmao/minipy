# -*- coding:utf-8 -*-
# @author xupingmao
# @since 2016
# @modified 2022/01/12 22:59:45
import mp_init
import os
import sys

import_func = mp_init._import

test_dir = os.path.dirname(__file__)
if test_dir not in sys.path:
    sys.path.append(test_dir)

def testfiles():
    suc = False
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
            import_func(g, modpath)
        except Exception as e:
            print("test failed -- " + fpath)
            traceback()
            exit(1)
            return
            
    print("="*50)
    print("\n"*2)
    print("all tests passed!!!")
    exit(0)
    
testfiles()