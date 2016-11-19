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
assertEquals('test'[-2:-1], "t")
assertEquals('test'[:] , 'test')
assertEquals('test'[:-2], 'tes')
assertEquals('test'[1:], 'est')