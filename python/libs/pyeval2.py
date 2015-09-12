from encode import *

def getValue(g,a):
    if a.type=='name' or a.type=='string':return g[a.val]
    elif a.type=='get':return getValue(getValue(g,a.first),a.second)
def getKey(g,k):
    if k.type=='name':return g[k]
    if k.type=='number':return int(k.val)
def setValue(g,a,b):
    v = evalAst(g,b)
    if a.type == 'name':g[a.val]=v;return v
    elif a.type == 'get':
        g = getValue(g, a.first)
        a = getKey(g, a.second)
        g[a] = v;return v
def buildTuple(g,a,b):
    a=evalAst(g,a)
    b=evalAst(g,b)
    if istype(a,'list'):return a+[b]
    if istype(b,'list'):return [a]+b
    return [a,b]
def _buildArguments(g, args):
    if args == None: return args
    if args.type == ',':
        return buildTuple(g, args.first, args.second)
    return [evalAst(g, args)]
def evalAst(g, ast):
    if istype(ast,'list'):
        for item in ast:
            v = evalAst(g,item)
        return v
    if ast.type == 'name':return g[ast.val]
    elif ast.type == 'number':return ast.val
    elif ast.type == 'string':return ast.val
    elif ast.type == 'dict':return {}
    elif ast.type == 'list':
        x = evalAst(g, ast.first);
        if not istype(x,'list'):return [x]
        else:return x
    elif ast.type == ',':return buildTuple(g, ast.first, ast.second)
    elif ast.type == 'get':
        return evalAst(g,ast.first)[evalAst(g,ast.second)]
    elif ast.type == '$':
        func = evalAst(g, ast.first)
        args = _buildArguments(g, ast.second);
        return apply(func, args)
    elif ast.type == '=':
        return setValue(g, ast.first, ast.second)
    elif ast.type == '+':
        return evalAst(g, ast.first)+evalAst(g,ast.second)
    elif ast.type == '-':
        return evalAst(g, ast.first)-evalAst(g,ast.second)
    elif ast.type == '*':
        return evalAst(g, ast.first)*evalAst(g,ast.second)
    elif ast.type == '/':
        return evalAst(g, ast.first)/evalAst(g,ast.second)
    elif ast.type == 'try':
        try:
            evalAst(g, ast.first)
        except Exception as e:
            evalAst(g, ast.third)

def pyeval2(src, g):
    return evalAst(g, parse(src))
