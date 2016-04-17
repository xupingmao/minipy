

def newlist(size):
    list = []
    for i in range(size):
        list.append(i)
    return list

for i in range(10):
    n = pow(10, i)
    print("%s: list(%s)".format(i+1, n))
    x = newlist(n)