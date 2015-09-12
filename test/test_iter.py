from libs.tools.dis import dis_txt
from libs.tools.pyeval import pyeval

src = '''
for x in ITER():
    print(x)
def next(self):
    print(self)
    if self.idx < self.max:
        idx = self.idx
        self.idx += 1
        return idx
    raise'''
print("=" * 40)    
src = '''
i = range(2, 4)
for y in i:
    print(y)'''
#pyeval(src, globals(), 1)
print("=" * 40)
def iter(i):
    list = []
    for y in i:
        list.append(y)
    print(list)
    return list
iter(range(3))
iter(range(2, 3))
iter(range(10, 0, -1))