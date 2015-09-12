from boot import *

i = 0
list = []
code = 'code = ['
while i < 200:
    code += str(i) + ','
    i+=1
code += '200]'

save("big-list.py", code)