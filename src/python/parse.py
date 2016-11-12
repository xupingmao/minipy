from tokenize import *

if "tm" not in globals():
    from boot import *


_stm_end_list = ['nl', 'dedent']
_skip_op = ['nl', ';']


def AstNode(type=None, first=None, second=None):
    o = newobj()
    o.type = type
    o.first = first
    o.second = second
    return o
    
class ParserCtx:
    def __init__(self, r, txt):
        self.token = Token("nl", 'nl', None)
        self.eof = Token("eof", 'eof', None)
        r.append(self.token)
        r.append(self.eof)
        self.r = r
        self.i = 0
        self.l = len(r)
        self.tree = []
        self.src = txt
        self.last_token = None

    def next(self):
        if self.i < self.l:
            self.token = self.r[self.i]
            self.i+=1
        else:
            self.token = self.eof

    def pop(self):
        return self.tree.pop()

    def add(self, v):
        self.tree.append(v)
        self.last_token = v

    def visit_block(self):
        self.tree.append('block')
        parse_block(self)
        body = []
        v = self.tree.pop()
        while v != 'block':
            body.append(v)
            v = self.tree.pop()
        body.reverse() # reverse the body
        return body

def expect(ctx, v):
    if ctx.token.type != v:
        compile_error("parse", ctx.src, ctx.token, "expect " + v)
    ctx.next()

def add_op(p, v):
    r = p.tree.pop()
    l = p.tree.pop()
    node = AstNode(v)
    node.first = l
    node.second = r
    p.add(node)


def parse_error(p, token=None, msg="Unknown"):
    if token != None:
        compile_error('parse', p.src, token, msg)
    else:
        raise("assert_type error")

def assert_type(p, type, msg):
    if p.token.type != type:
        parse_error(p, p.token, msg)
    #p.next()

def baseitem(p):
    t = p.token.type
    token = p.token
    if t in ('number', 'string', 'name', 'None'):
        p.next()
        p.add(token)
    elif t == '[':
        p.next()
        node = AstNode("list")
        node.pos = token.pos
        if p.token.type == ']':
            p.next()
            node.first = None
        else:
            exp(p, ',')
            expect(p, ']')
            node.first = p.pop()
        p.add(node)
    elif t == '(':
        p.next()
        expr(p)
        expect(p, ')')
    elif t == '{':
        p.next()
        node = AstNode('dict')
        items = []
        while p.token.type != '}':
            exp(p, 'or')
            expect(p, ':')
            exp(p, 'or')
            v = p.pop()
            k = p.pop()
            items.append([k,v])
            if p.token.type == '}':
                break
            expect(p, ',')
        expect(p, '}')
        node.first = items
        p.add(node)

def expr(p):
    return exp(p, '=')

    
#  Assignment = Lvalue AssignOp Rvalue
#  Lvalue     = Object (Call|Attr)* Attr
#  Object     = Name | Number | String | True | False | None
#  Value      = Object (Call|Attr)*
#  Rvalue     = Value (Op Value)*
#  Attr       = ('.' Name) | ('[' Rvalue ']')
#  Call       = '(' ArgList ')'
#  AssignOp   = '=' 
#  Op         = '+' | '-'
def exp(p, level):
    if level == '=':
        exp(p, ',')
        if p.token.type in ('=', '+=', '-=', '*=', '/=', '%='):
            t = p.token.type
            p.next()
            exp(p, ',')
            add_op(p, t)
    elif level == ',':
        exp(p, 'or')
        while p.token.type == ',':
            p.next()
            if p.token.type == ']':
                break # for list
            exp(p, 'or')
            b = p.pop()
            a = p.pop()
            if gettype(a)=="list":
                a.append(b)
                p.add(a)
            else:
                p.add([a,b])
    elif level == 'or':
        exp(p, 'and')
        while p.token.type == 'or':
            p.next()
            exp(p, 'and')
            add_op(p, 'or')
    elif level == 'and':
        exp(p, 'not')
        while p.token.type == 'and':
            p.next()
            exp(p, 'not')
            add_op(p, 'and')
    elif level == 'not':
        if p.token.type == 'not':
            while p.token.type == 'not':
                p.next()
                exp(p, 'not')
                node = AstNode('not')
                node.first = p.pop()
                p.add(node)
        else:
            exp(p, 'cmp')
    elif level == 'cmp':
        exp(p, '+-')
        while p.token.type in ('>', '<', '==', 'is', '!=', '>=', '<=', 'in', 'notin'):
            t = p.token.type
            p.next()
            exp(p, '+-')
            add_op(p, t)
    elif level == '+-':
        exp(p, 'factor')
        while p.token.type in ('+', '-'):
            t = p.token.type
            p.next()
            exp(p, 'factor')
            add_op(p, t)
    elif level == 'factor':
        fnc = call_or_get_exp
        fnc(p)
        while p.token.type in ('*','/', '%'):
            t = p.token.type
            p.next()
            fnc(p)
            add_op(p, t)

def call_or_get_exp(p):
    if p.token.type == '-':
        p.next()
        call_or_get_exp(p)
        node = AstNode('neg', p.pop())
        p.add(node)
    else:
        baseitem(p)
        while p.token.type in ('.','(','['):
            t = p.token.type
            p.next()
            if t == '[':
                expr(p)
                expect(p, ']')
                add_op(p, 'get')
            elif t == '(':
                node = AstNode('call', p.pop())
                if p.token.type == ')':
                    p.next()
                    node.second = None
                else:
                    exp(p, ',')
                    expect(p, ')')
                    node.second = p.pop()
                p.add(node)
            else:
                baseitem(p)
                b = p.pop()
                a = p.pop()
                node = AstNode('attr')
                node.first = a
                node.second = b
                p.add(node)

def _get_path(obj):
    t = obj.type
    if t == 'get':
        return _get_path(obj.first) + '/' \
            + _get_path(obj.second)
    elif t == 'name':
        return obj.val
    elif t == 'string':
        return obj.val
    else:
        raise

def _path_check(p, node):
    if node.type == 'attr':
        _path_check(p, node.first)
        _path_check(p, node.second)
    elif node.type == 'name':
        return True
    else:
        parse_error(p, node, 'import error')

def _name_check(p, node):
    if node.type == ',':
        _name_check(p, node.first)
        _name_check(p, node.second)
    elif node.type == 'name':
        return True
    else:
        parse_error(p, node, 'import error' + node.type)

def parse_from(p):
    p.next()
    expr(p)
    expect(p, "import")
    node = AstNode("from")
    node.first = p.pop()
    _path_check(p, node.first)
    if p.token.type != "*":
        raise Exception("expect * at from statement")
    p.token.type = 'string'
    node.second = p.token
    p.next()
    # _name_check(p, node.second)
    p.add(node)

def parse_import(p):
    p.next()
    expr(p)
    node = AstNode('import')
    node.first = p.pop()
    p.add(node)

# count = 1
def skip_nl(p):
    while p.token.type in _skip_op:
        p.next()

def call_node(fnc, args):
    node = AstNode('call')
    node.first = fnc
    node.second = args
    return node
    
def parse_inner_func(p):
    fnc = p.token
    fnc.type = 'name'
    p.next()
    if p.token.type == 'nl':
        args = None
    else:
        exp(p, ',')
        args = p.pop()
    p.add(call_node(fnc, args))

def parse_del(p):
    p.next()
    expr(p)
    x = AstNode('del', p.pop())
    p.add(x)

def parse_global(p):
    p.next()
    node = AstNode('global')
    assert_type(p, 'name', 'Global_exception')
    node.first = p.token
    p.add(node)
    p.next()

def parse_pass(p):
    tk = p.token
    p.next()
    node = AstNode(tk.type)
    node.pos = tk.pos
    p.add(node)

def parse_try(p):
    pos = p.token.pos # save position
    p.next()
    expect(p, ':')
    node = AstNode("try", p.visit_block())
    node.pos = pos
    expect(p, 'except')
    if p.token.type == 'name':
        p.next()
        expect(p, 'as')
        if p.token.type != 'name':
            p.error('error in try-expression')
        node.second = p.token
        p.next()
    else:node.second = None
    expect(p, ':')
    node.third = p.visit_block()
    p.add(node)

def parse_for_items(p):
    name = p.token
    expect(p, "name")
    name_list = [name]
    while p.token.type == ",":
        p.next()
        name = p.token
        expect(p, "name")
        name_list.append(name)
    expect(p, "in")
    expr(p)
    node = AstNode("in")
    node.first = name_list
    node.second = p.pop()
    return node


def parse_for(p):
    # parse_for_while(p, "for")
    ast = AstNode("for")
    p.next()
    node = parse_for_items(p)
    ast.first = node
    expect(p, ':')
    ast.second = p.visit_block()
    p.add(ast)

def parse_while(p):
    parse_for_while(p, 'while')


def parse_for_while(p, type):
    ast = AstNode(type)
    p.next()
    expr(p)
    ast.first = p.pop()
    expect(p, ':')
    ast.second = p.visit_block()
    p.add(ast)

def parse_arg(p):
    expect(p, '(')
    if p.token.type == ')':
        p.next()
        return []
    else:
        args = []
        '''
        1. name,
        2. name = expr,
        3. *name
        '''   
        varg = 0 # mark varg is met or not.
        while p.token.type == 'name':
            arg = AstNode("arg")
            arg.first = p.token
            arg.second = None
            p.next()
            if varg == 1:
                expect(p, '=')
                baseitem(p)
                arg.second = p.pop()
            elif p.token.type == '=':
                p.next()
                baseitem(p) # problem
                arg.second = p.pop()
                varg = 1
            args.append(arg)
            if p.token.type != ',':break
            p.next()
        if p.token.type == '*':
            p.next()
            assert_type(p, 'name', 'Invalid arguments')
            arg = AstNode("narg", p.token)
            arg.second = None
            args.append(arg)
            p.next()
        expect(p, ')')
        return args

def parse_def(p):
    p.next()
    assert_type(p, 'name', 'DefException')
    func = AstNode("def")
    func.first = p.token
    p.next()
    func.second = parse_arg(p)
    expect(p, ':')
    func.third = p.visit_block()
    p.add(func)

def parse_class(p):
    p.next()
    assert_type(p, 'name','ClassException')
    clazz = AstNode()
    clazz.type = 'class'
    clazz.first = p.token
    p.next()
    if p.token.type == '(':
        p.next()
        assert_type(p, 'name', 'ClassException')
        p.third = p.token
        p.next()
        expect(p, ')')
    expect(p, ':')
    clazz.second = p.visit_block()
    p.add(clazz)

def parse_stm1(p, type):
    p.next()
    node = AstNode(type)
    if p.token.type in _stm_end_list:
        node.first = None
    else:
        expr(p)
        node.first = p.pop()
    p.add(node)

            

def parse_if(p):
    ast = AstNode("if")
    p.next()
    expr(p)
    ast.first = p.pop()
    expect(p, ':')
    ast.second = p.visit_block()
    ast.third = None
    cur = ast # temp
    temp = cur 
    if p.token.type == 'elif':
        while p.token.type == 'elif':
            node = AstNode("if")
            p.next()
            expr(p)
            expect(p, ':')
            node.first = p.pop()
            node.second = p.visit_block()
            node.third = None
            cur.third = node
            cur = node
    if p.token.type == 'else':
        p.next()
        expect(p, ':')
        cur.third = p.visit_block()
    p.add(temp)

def parse_return(p):
    parse_stm1(p, 'return')

def parse_annotation(p):
    token = p.token
    p.next()
    p.add(AstNode("@", token))
    
def parse_skip(p):
    p.next()
    
stmt_map = {
    "from": parse_from,
    "import": parse_import,
    "def": parse_def,
    "class": parse_class,
    "for": parse_for,
    "while": parse_while,
    "if": parse_if,
    "return": parse_return,
    "raise":parse_inner_func,
    "assert": parse_inner_func,
    "break": parse_pass,
    "continue": parse_pass,
    "pass": parse_pass,
    "name": expr,
    "number": expr,
    "string": expr,
    "try": parse_try,
    "global": parse_global,
    "del": parse_del,
    ';': parse_skip,
    "@":parse_annotation,
}


def parse_stm(p):
    t = p.token.type
    if t not in stmt_map:
        parse_error(p, p.token, "Unknown Expression")
    else:
        stmt_map[t](p)


def parse_block(p):
    skip_nl(p)
    if p.token.type == 'indent':
        p.next()
        while p.token.type != 'dedent':
            parse_stm(p)
            skip_nl(p)
        p.next()
    else:
        parse_stm(p)
        while p.token.type == ';':
            if p.token.type in ('nl', 'eof'):break
            else:parse_stm(p)
        skip_nl(p)
            
    
def parse(content):
    r = tokenize(content)
    p = ParserCtx(r, content)
    p.next()
    try:
        while p.token.type != 'eof':
            parse_block(p)
        x = p.tree
        # except:
        if x == None:
            p.error()
        return x
    except Exception as e:
        # print(e, v)
        compile_error("parse", content, p.token, str(e))
        raise(e)

# TM_TEST
def print_ast_obj(tree, n=0):
    if tree == None:
        return
    if tree.type in ("number", "string", "None"):
        print(" " * n, tree.val)
        return
    if tree.type == "name":
        print(" " * n, tree.type , tree.val)
    else:
        print(" " * n, tree.type)
    if hasattr(tree, "first"):
        print_ast(tree.first, n+2)
    if hasattr(tree, "second"):
        print_ast(tree.second, n+2)
    if hasattr(tree, "third"):
        print_ast(tree.third, n+2)

def print_ast_list(tree, n=0):
    print(" " * n, "[]")
    for item in tree:
        print_ast_obj(item, n+2)

def print_ast(tree, n=0):
    if gettype(tree) == "list":
        print("     ")
        return print_ast_list(tree, n)
    if hasattr(tree, "pos"):
        line = tree.pos[0]
        line_str = str(line)
        space = 4 - len(line_str)
        printf(space*' ' + line_str + ':')
    else:
        print("     ")
    return print_ast_obj(tree, n)

def parsefile(fname):
    try:
        return parse(load(fname))
    except Exception as e:
        printf("parse file %s FAIL", fname)

def tk_list_len(tk):
    if tk == None: return 0
    if tk.type == ',':return tk_list_len(tk.first) + tk_list_len(tk.second)
    return 1

if __name__ == "__main__":
    import sys
    tree = parsefile(sys.argv[1])
    print_ast(tree)

# TM_TEST_END

