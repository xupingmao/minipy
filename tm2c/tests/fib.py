import time
import sys
time = time.time

def fib(n):
    if n == 1 or n == 2:
        return 1
    return fib(n-1) + fib(n-2)

def main():
    sn = sys.argv[1]
    n = int(sn)
    t1 = time()
    print(fib(n))
    t2 = time()
    print(t2-t1)

if __name__ == '__main__':
    main()
