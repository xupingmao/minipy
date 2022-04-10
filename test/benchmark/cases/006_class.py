# -*- coding:utf-8 -*-
# @author xupingmao
# @since 2022/02/04 22:45:35
# @modified 2022/02/13 18:11:19
# @filename 006_class.py

import time
import random

try:
    randint_wrap = random.randint
except:
    # micropython
    def randint_wrap(a, b):
        return a + random.getrandbits(32) % (b-a)


class TestClass:

    def __init__(self):
        self.value = 0
        self.name = "test"

    def method1(self):
        self.value += 1

def timeit(func, *args):
    t1 = time.time()
    ret = func(*args)
    cost_time = (time.time() - t1) * 1000
    print("cost time: %sms" % cost_time)

def rand_str(length):
    v = ""
    a = ord('A')
    b = ord('Z')
    for i in range(length):
        v += chr(randint_wrap(a, b))
    return v

def rand_int():
    return randint_wrap(1, 100)

def test_random_gen(n):
    print("test_random_gen: n=%d" % n)
    for i in range(n):
        rand_str(5)

def test_class_invoke(n):
    print("test_class_invoke: n=%d" % n)
    d = TestClass()
    for i in range(n):
        d.method1()

    print("d.value = %s" % d.value)

def test_class_get(n):
    print("test_class_get: n=%d" % n)
    d = TestClass()
    for i in range(n):
        t = d.value

def test_class_set(n):
    print("test_class_set: n=%d" % n)
    d = TestClass()
    for i in range(n):
        d.name = rand_str(5)

timeit(test_random_gen, 100000)
timeit(test_class_invoke, 100000)
timeit(test_class_get, 100000)
timeit(test_class_set, 100000)
