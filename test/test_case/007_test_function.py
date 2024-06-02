# -*- coding:utf-8 -*-
'''
Author: xupingmao
email: 578749341@qq.com
Date: 2022-02-12 11:48:21
LastEditors: xupingmao
LastEditTime: 2024-06-02 12:02:20
FilePath: /minipy/test/test_case/007_test_function.py
Description: 描述
'''
from   test import *
import logging

def cursive_overflow(n):
    if n > 0:
        cursive_overflow(n-1)

result = testfunc(cursive_overflow, [200], None)

logging.info("result:", result)
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
        if r != expect:
            error_msg = "expect %s but see %s" % (expect, r)
            print(error_msg)
            raise Exception(error_msg)
    except Exception as e:
        print(e)
        assert e == expect, "exception occurs"

def foo(parg, varg = 10):
    return parg + varg
    
test(foo, [], "ArgError: parg=1,varg=1,given=0,int=257")
test(foo, [0], 10)
test(foo, [1,2], 3)
test(foo, [1,2,3], "ArgError: parg=1,varg=1,given=3,int=257")

def add(a,b):
    return a+b
    
print("add(*[1,2])=", add(*[1,2]))