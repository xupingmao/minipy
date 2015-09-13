from pcall import *

def foo(parg, varg = 10):
    return (parg, varg)
    
pcall(foo, [], 1)
pcall(foo, [0], 1)
pcall(foo, [1,2], 1)
pcall(foo, [1,2,3], 1)