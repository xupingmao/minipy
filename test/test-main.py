import init

import_func = init._import

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
        
        # test expression
        "test-op",
        "test-for",
        "test-function",
        "test-while",
        "test-lex",
        "test-parse",
        "test-tailcall",
        
        # some algorithms
        "ackermann",
    ]
)