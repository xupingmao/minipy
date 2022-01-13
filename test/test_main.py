# -*- coding:utf-8 -*-
# @author xupingmao
# @since 2016
# @modified 2022/01/12 22:59:45
import mp_init
import os

import_func = mp_init._import

def testfiles(filenamelist):
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
            print("")
            print("=" * 50)
            print("| ", modpath)
            print("=" * 50)
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
    
testfiles(
    [
        # test base object
        "test-string",
        "test-list",
        "test-dict",
        "test-assignment",
        
        # test expression
        "test-op",
        "test-in",
        "test-for",
        "test-function",
        "test-while",
        "test-lex",
        "test-parse",
        "test-tailcall",
        "test-try-simple",
        "test-sys",
        # some algorithms
        "ackermann",
    ]
)