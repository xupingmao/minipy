from boot import *

x = newobj()
x.name = 10
times = 1000000

t1 = clock()
for i in range(times):
    x.name = times / 10
t2 = clock()
print('time=',t2-t1)