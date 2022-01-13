from   test import *
import logging

def cursive_overflow(n):
    if n > 0:
        cursive_overflow(n-1)

result = testfunc(cursive_overflow, [200], None)

logging.info(result)
logging.info(result.exception != None)
if result.exception != None:
    logging.info("result.exception is not None")

value = result.exception != None
if not value:
    logging.info("result.exception is None")

assertTrue(result.exception != None, "result.exception != None")

def test(func, args, expect):
    """
    func: 函数名
    args: 参数列表
    exect: 结果或者异常信息
    """
    try:
        logging.logCall(func, args)
        r = apply(func, args)
        assert r == expect
    except Exception as e:
        assert e == expect

def foo(parg, varg = 10):
    return parg + varg
    
test(foo, [], "ArgError: parg=1,narg=1,given=0")
test(foo, [0], 10)
test(foo, [1,2], 3)
test(foo, [1,2,3], "ArgError: parg=1,narg=1,given=3")

def add(a,b):
    return a+b
    
print("add(*[1,2])=", add(*[1,2]))