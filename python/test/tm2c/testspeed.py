i = 0
j = 0
t1 = clock()
while i < 1000000:
    j = i * 34
    i += 1
t2 = clock()
print("global scope: used time = " + str(t2-t1))
def test():
    i = 0;j = 0
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
