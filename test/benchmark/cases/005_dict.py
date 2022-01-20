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

def main(n):
	print("n=%d" % n)
	d = dict()
	for i in range(n):
		key = rand_str(5)
		d[key] = 1
	print("len(dict)=%d" % len(d))


timeit(main, 100000)

