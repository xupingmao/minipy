from pcall import *

def test(func, args, expect):
    try:
        r = apply(func, args)
        assert r == expect
    except Exception as e:
        assert e == expect

def foo(parg, varg = 10):
    return parg + varg
    
test(foo, [], "ArgError,parg=1,varg=1,given=0")
test(foo, [0], 10)
test(foo, [1,2], 3)
test(foo, [1,2,3], "ArgError,parg=1,varg=1,given=3")
