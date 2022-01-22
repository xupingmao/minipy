import time

def timeit(func, *args):
    t1 = time.time()
    ret = func(*args)
    cost_time = (time.time() - t1) * 1000
    print("cost time: %sms" % cost_time)

def fib(n):
    # print("n=",n)
    if n <= 0:
        raise "n<=0, n=%s" % n
    if n == 1 or n == 2:
        # print("n==1 or n==2")
        return 1
    return fib(n-1) + fib(n-2)

def main():
	print("fib(30)=" + str(fib(30)))

timeit(main)