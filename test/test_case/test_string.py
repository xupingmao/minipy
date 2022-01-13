from test import *
    

testfunc('test'.ljust, [5], 'test ')
testfunc('test00'.ljust, [5], 'test00')
testfunc('test'.rjust, [5], ' test')
testfunc('test00'.rjust, [5], 'test00')
testfunc('test'.center, [5], ' test')
testfunc('tes'.center, [5], ' tes ')
testfunc('test'.startswith, ['tes'], 1)
testfunc('xyzxxzz'.replace, ['x', 'zz'], 'zzyzzzzzzz')
testfunc('x\r\n'.replace, ['\r', '\n'], 'x\n\n')

# test slice
assertEquals('test'[0:1], "t")
assertEquals('test'[1:2], 'e')
assertEquals('test'[-2:-1], "s")
assertEquals('test'[:] , 'test')
assertEquals('test'[:-1], 'tes')
assertEquals('test'[1:], 'est')

assertEquals('"'.replace('"', '\\"'), '\\"')
assertEquals('abc'.replace("\0", "abc"), "abc")
assertEquals('a\0c'.replace('\0', 'b'), 'abc')

a = "%r" % "a\nb"
print(a, len(a))

b = "'a\nb'"
print(b, len(b))

assertEquals("%r" % "a\nb", "'a\\nb'")
