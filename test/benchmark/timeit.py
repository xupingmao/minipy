import time

def timeit(func, *args):
    print(func, args)
    t1 = time.clock()
    ret = func(*args)
    t2 = time.clock()
    print("run time is ", t2-t1)