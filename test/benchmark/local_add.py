from timeit import *

def local_add(times):
    for i in range(times):
        z = i + 1
    
timeit(local_add, 1000000)