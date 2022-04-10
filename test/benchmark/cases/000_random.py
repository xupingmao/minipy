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

def main(n):
	print("main: n=%d" % n)
	for i in range(n):
		randint_wrap(1,1000)

def test_range(n):
    print("test_range: n=%d" % n)
    for i in range(n):
        pass

timeit(test_range, 100000)
timeit(main, 100000)

