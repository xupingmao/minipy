from test import *
    

test('test'.ljust, [5], 'test ')
test('test00'.ljust, [5], 'test00')
test('test'.rjust, [5], ' test')
test('test00'.rjust, [5], 'test00')
test('test'.center, [5], ' test')
test('tes'.center, [5], ' tes ')
test('test'.startswith, ['tes'], 1)
test('xyzxxzz'.replace, ['x', 'zz'], 'zzyzzzzzzz')
test('x\r\n'.replace, ['\r', '\n'], 'x\n\n')


