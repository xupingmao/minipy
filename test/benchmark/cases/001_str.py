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

def rand_str(length):
	v = ""
	a = ord('A')
	b = ord('Z')
	for i in range(length):
		v += chr(randint_wrap(a, b))
	return v

def main(n):
	print("n=%d" % n)
	for i in range(n):
		rand_str(5)


timeit(main, 100000)

