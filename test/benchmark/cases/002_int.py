import time
import random

def timeit(func, *args):
    t1 = time.time()
    ret = func(*args)
    cost_time = (time.time() - t1) * 1000
    print("cost time: %sms" % cost_time)

try:
	randint_wrap = random.randint
except:
	# micropython
	def randint_wrap(a, b):
		return a + random.getrandbits(32) % (b-a)

def rand_int():
	return randint_wrap(1, 100)

def test_add(n):
	print("test_add: n=%d" % n)
	for i in range(n):
		a = rand_int() + rand_int()
	return a

def test_sub(n):
	print("test_sub: n=%d" % n)
	for i in range(n):
		a = rand_int() - rand_int()
	return a

def test_mul(n):
	print("test_mul: n=%d" % n)
	for i in range(n):
		a = rand_int() * rand_int()
	return a

def test_div(n):
	print("test_div: n=%d" % n)
	for i in range(n):
		a = rand_int() / rand_int()
	return a

def test_modulo(n):
	print("test_modulo: n=%d" % n)
	for i in range(n):
		a = rand_int() % rand_int()
	return a

timeit(test_add, 100000)
timeit(test_sub, 100000)
timeit(test_mul, 100000)
timeit(test_div, 100000)
timeit(test_modulo, 100000)