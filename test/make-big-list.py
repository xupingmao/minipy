i = 0
list = []
code = 'code = ['
while i < 1000:
    list.append(str(i))
    i+=1
code += ','.join(list) + ']'
code += '\nprint(code)'

save("big-list.py", code)