if str(1.0) == "1.0":
    from time import time
else:
    time = clock

def add(a, n):
    a.idx += n
    
class T:
    def __init__(self, n):
        self.idx = n
        self.abc = 10
        self.cde = 10
        self.afd = 10000
        self.kkk = 20
        
a = T(0)
t1 = time()
for i in range(10000):
    add(a, 1)
t2 = time()
print(t2-t1)