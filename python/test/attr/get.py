if str(1.0) == "1.0":
    from time import time
else:
    time = clock

_k1 = 'idx'
_k2 = 'i' + 'dx'
def add(a, n):
    a[_k1] += n
def add2(a, n):
    a[_k2] += n
class T:
    def __init__(self, n):
        self.idx = n
        self.abc = 10
        self.cde = 10
        self.afd = 10000
        self.kkk = 20
_times = 1000000
a = T(0)
t1 = time()
for i in range(_times):
    add(a, 1)
t2 = time()
print('use idx', t2-t1)
t1 = time()
for i in range(_times):
    add2(a, 1)
t2 = time()
print("no idx", t2-t1)