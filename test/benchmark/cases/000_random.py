import time
import random

def timeit(func, *args):
    t1 = time.time()
    ret = func(*args)
    cost_time = (time.time() - t1) * 1000
    print("cost time: %sms" % cost_time)

def main(n):
	print("n=%d" % n)
	for i in range(n):
		random.randint(1,1000)


timeit(main, 100000)

