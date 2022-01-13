from test import *

def foo(a, b, c, d):
    if d > 0:
        foo(a, b, c, d-1)
       
testException(foo, [1, 1, 1, 1024], 'mp_eval: frame overflow (127)') 

def assertException():
    assert 1 == 2

testExceptionType(assertException, [], 'AssertionError')