import time
time = time.time

def fib(n):
    if n == 1 or n == 2:
        return 1
    return fib(n-1) + fib(n-2)

def main():
    sn = input("n=")
    n = int(sn)
    t1 = time()
    print(fib(n))
    t2 = time()
    print(t2-t1)

if __name__ == '__main__':
    main()
