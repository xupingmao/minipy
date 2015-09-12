from boot import *

i = 0
list = []
code = 'code = ['
while i < 999:
    code += str(i) + ','
    i+=1
code += '1000]'
code += '\nprint(code)'
save("big-list.py", code)