#from boot import *
from parse import parse

string = loadlib('libs/object/string.py')
# print(string)
quote = string.quote

ops_list = [
        'from', '+', '-', '*', '/', '%', ',' ,'=', 
        '+=', '-=', '/=', '*=', 'get',
        "==", "!=", ">", "<", ">=", "<=", "and",
         "or", "for","while", "in", "notin", "import"]

three_list = ["if", "choose", "def", 'try']
two_list = ['+', '-', '*', '/', '%', ',' ,
        '+=', '-=', '/=', '*=', 'get',
        "==", "!=", ">", "<", ">=", "<=", "and",
         "or" , 'in', 'notin',"arg", 'class', 'while', 'for', 'from']
one_list = ['neg', 'pos', 'not', 'dict', 'return', 'narg', 'import']
n_list = ['call', 'list']

def f(n, v, pre = ""):
    rs = ''
    if v == None:
        rs = 'None'
    elif istype(v, "list"):
        s = ''
        for i in v:
            s += '\n' + f(n+2, i)
        rs = s
    elif v.type == 'string':
        rs = quote(v.val)
    elif v.type == 'number':
        rs = quote(v.val)
    elif v.type == 'name':
        rs = v.val
    elif v.type == 'None':
        rs = v.val
    elif v.type == '=':
        rs = '=\n' + f(n+2, v.first) +'\n'+ f(n+2, v.second)
    elif v.type == 'call':
        rs = f(0, v.first, 'call:') + f(n+2, v.second)
    elif v.type == 'list':
        rs = 'list'+f(n+2, v.first)
    elif v.type in one_list:
        rs = v.type + '\n' + f(n+2, v.first)
    elif v.type in two_list:
        rs = v.type + '\n' + f(n+2, v.first) + '\n' + f(n+2, v.second)
    elif v.type in three_list:
        rs = v.type+'\n' + f(n+2, v.first, 'first => ') + "\n" + f(n+2,v.second,'second => ') + '\n' + f(n+2, v.third, 'third => ')
    else:
        rs = quote(v)
    #if v != None:print(v)
    return ' ' * n + pre + rs
def printast0(tree):
    if not istype(tree, 'list'):
        print("parameter is not a list")
        print(tree)
        return
    #print(self.tree)

    for i in tree:
        s = f(0, i)
        print(s)
# tree = parse(open('parse.py').read())

def simple_show(tree):
    if not istype(tree, "list"):
        print(tree)
    else:
        for i in tree:
            simple_show(i)

def _parse(f):
    tree = parse(load(f))
    # print(tree)
    printast0(tree)

def printast(src):
    tree = parse(src)
    printast0(tree)
if __name__ == '__main__':
    _parse(ARGV[1])