import time

def timeit(func, *args):
    t1 = time.time()
    ret = func(*args)
    cost_time = (time.time() - t1) * 1000
    print("cost time: %sms" % cost_time)


def fib(n):
    if n == 1 or n == 2:
        return 1
    return fib(n-1) + fib(n-2)

timeit(fib, 35)