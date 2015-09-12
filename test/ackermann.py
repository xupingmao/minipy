from boot import *
from libs.tools.test import testfunc
def ackermann(x, y):
    if y == 0: return 0
    elif x == 0:return y * 2
    elif y == 1:return 2
    else:return ackermann(x-1, ackermann(x, y-1))

for i in range(10):
    testfunc(ackermann, [1, i])
for i in range(10):
    testfunc(ackermann, [2, i])
for i in range(10):
    testfunc(ackermann, [3, i])