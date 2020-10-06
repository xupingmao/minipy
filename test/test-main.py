# -*- coding:utf-8 -*-
# @author xupingmao
# @since 2016
# @modified 2020/10/02 10:31:29
import mp_init
import_func = mp_init._import

def testfiles(filenamelist):
    suc = False
    for filename in filenamelist:
        try:
            g = {}
            print("")
            print("=" * 50)
            print("| ", filename)
            print("=" * 50)
            import_func(g, filename)
        except Exception as e:
            traceback()
            print("test failed -- " + filename)
            return
            
    print("="*50)
    print("\n"*2)
    print("all tests passed!!!")
    
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