

def newlist(size):
    list = []
    for i in range(size):
        list.append(i)
    return list

for i in range(5):
    n = pow(10, i)
    print("%s: list(%s)" % (i+1, n))
    x = newlist(n)

