import time
import random

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
		v += chr(random.randint(a, b))
	return v

def test_random_gen(n):
	print("test_random_gen: n=%d" % n)
	for i in range(n):
		rand_str(5)

def test_dict_set(n):
	print("test_dict_set: n=%d" % n)
	d = dict()
	for i in range(n):
		key = rand_str(5)
		d[key] = 1
	print("len(dict)=%d" % len(d))

def test_dict_get(n):
	print("test_dict_get: n=%d" % n)
	d = dict()
	for i in range(n):
		key = rand_str(5)
		d[key] = 1
	print("len(dict)=%d" % len(d))

timeit(test_random_gen, 100000)
timeit(test_dict_set, 100000)
timeit(test_dict_get, 100000)
