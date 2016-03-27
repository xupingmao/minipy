buf = []
def process(c):
    #printf("%s:%s\n", c, chr(c))
    global buf
    buf.append(c)
    if len(buf) == 4:
        print(buf)
        buf = []

readFile("test1.py",  process)
print(buf)

for x in range(10):
    print(x)

if x in (1,2,3):
    print(x)