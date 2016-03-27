

# a = [1,2,3]

# def mret():
#     return 1,2,3

# for a in [1,2,3]:
#     print(a)

# for b in (1,2,3):
#     print(b)

# while i < 1:
#     print(i)

# print(a,b,c,d)

# a = 10
# print(a)

def test_add(a, b, d = 10):
    hello = a + b
    hello = hello + d
    x = a + b + d
    return hello + d

def test_assign(a,b,c):
    a = 10
    b = a
    global g
    g = a
    g = a + b
    return g

def test_call():
    call0()
    call1(1)
    call2(1,2)
    call3(1,2,3)
    call4(1,2,3,4)
    call5(1,2,3,4,5)
    # call6(1,2,3,4,5,6)

def test_while():
    while a < 10:
        print(a)
        a = a + 1
        if a == 1:
            continue
        else:
            break
        print(a,b)

def test_set():
    obj.name = "test";
    obj.friend.age = 10;

def test_raise():
    raise 1
    raise 1,2

def test_list():
    a = [1,2,3]
    c = [-1]
    return [a,b,c]

def test_dict():
    a = {'name': "hello", age:10}
    a['friend'] = {}