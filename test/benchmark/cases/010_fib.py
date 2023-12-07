# -*- coding:utf-8 -*-
'''
Author: xupingmao
email: 578749341@qq.com
Date: 2023-12-07 22:03:29
LastEditors: xupingmao
LastEditTime: 2023-12-07 22:49:00
FilePath: /minipy/test/benchmark/cases/010_fib.py
Description: 描述
'''
import time

def timeit(func, *args):
    t1 = time.time()
    ret = func(*args)
    cost_time = (time.time() - t1) * 1000
    print("cost time: %sms, ret=%s" % (cost_time, ret))


def fib(n):
    if n == 1 or n == 2:
        return 1
    return fib(n-1) + fib(n-2)

timeit(fib, 30)