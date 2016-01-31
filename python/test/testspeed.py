if str(1.0) == '1.0':
    from boot import *


i = 0
j = 0
t1 = clock()
while i < 1000000:
    j = i * 34
    i += 1
t2 = clock()
print("global scope: used time = " + str(t2-t1))
def test():
    i = 0
    j = 0
    t1 = clock()
    while i < 1000000:
        j = i * 34
        i +=1
    t2 = clock()
    print("local scope: used time = " + str(t2-t1))
test()

def fib(n):
    if n == 0 or n == 1: return 1
    return fib(n-1) + fib(n-2)
t1 = clock()
print(fib(30))
t2 = clock()
print("fib(30)", t2-t1)

def fib2(n):
    i = 0
    a = 1
    b = 1
    while i < n:
        tmp = a
        a = b
        b = tmp + b
        i += 1
    return a

t1 = clock()
print(fib2(30))
t2 = clock()
print("fib2(30)", t2-t1)