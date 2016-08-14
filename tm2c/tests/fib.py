import time
time = time.time

def fib(n):
    if n == 1 or n == 2:
        return 1
    return fib(n-1) + fib(n-2)

t1 = time()
print(fib(30))
t2 = time()
print(t2-t1)