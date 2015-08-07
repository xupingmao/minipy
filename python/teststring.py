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


