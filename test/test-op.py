from test import *

assert 1 + 1 == 2
assert [1,2] + [3,4] == [1,2,3,4]
assert 'test' + 'world' == 'testworld'

assert 10 - 1 == 9
assert 1.1 * 2 == 2.2
assert 10 / 5 == 2

a, b = 1,2
assert a == 1
assert b == 2
print(a,b)

a, b, c = 4, 5, 6
assert a == 4
assert b == 5
assert c == 6
print(a,b,c)

a = [1,2,3]
assertEquals(a[1:2], [2])
assertEquals(a[0:-1], [1,2])

