import time

def timeit(func, *args):
    print(func, args)
    t1 = time.time()
    # if "tm" in globals():
    ret = apply(func, args)
    # else:
        # func(*args)
    t2 = time.time()
    print("run time is ", t2-t1)