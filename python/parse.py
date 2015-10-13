from tokenize import *
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
    p.tree.append(node)


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
            comma_exp(p)
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
            or_exp(p)
            expect(p, ':')
            or_exp(p)
            v = p.pop()
            k = p.pop()
            items.append([k,v])
            if p.token.type == '}':
                break
            expect(p, ',')
        expect(p, '}')
        node.first = items
        p.add(node)
            
def assign_exp(p):
    fnc = comma_exp
    fnc(p)
    if p.token.type in ('=', '+=', '-=', '*=', '/=', '%='):
        t = p.token.type
        p.next()
        fnc(p)
        add_op(p, t)


def comma_exp(p):
    fnc = or_exp
    fnc(p)
    while p.token.type == ',':
        p.next()
        if p.token.type == ']':
            break # for list
        fnc(p)
        add_op(p, ',')
        
def or_exp(p):
    fnc = and_exp
    fnc(p)
    while p.token.type == 'or':
        p.next()
        fnc(p)
        add_op(p, 'or')

def and_exp(p):
    fnc = not_exp
    fnc(p)
    while p.token.type == 'and':
        p.next()
        fnc(p)
        add_op(p, 'and')

def not_exp(p):
    fnc = cmp_exp
    if p.token.type == 'not':
        p.next()
        not_exp(p)
        node = AstNode('not')
        node.first = p.pop()
        p.add(node)
    else:
        fnc(p)

def cmp_exp(p):
    fnc = item
    fnc(p)
    while p.token.type in ('>', '<', '==', '!=', '>=', '<=', 'in', 'notin'):
        t = p.token.type
        p.next()
        fnc(p)
        add_op(p, t)

def item(p):
    fnc = item2
    fnc(p)
    while p.token.type in ('+', '-'):
        t = p.token.type
        p.next()
        fnc(p)
        add_op(p, t)


def item2(p):
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
                    comma_exp(p)
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
expr = assign_exp

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
    if p.token.type == "*":
        p.token.type = 'string'
        node.second = p.token
        p.next()
    else:
        expr(p)
        node.second = p.pop()
        _name_check(p, node.second)
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
        comma_exp(p)
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
    assert_type(p, 'name', 'GlobalException')
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

def parse_for(p):
    parse_for_while(p, 'for')

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
    p.next()
    token = p.token
    expect(p, "name")
    p.add(AstNode("@", token))
    
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
            while p.token.type == ';':p.next()
            if p.token.type in ('nl', 'eof'):break
            else:parse_stm(p)
        skip_nl(p)
            
    
def parse(v):
    r = tokenize(v)
    p = ParserCtx(r, v)
    p.next()
    while p.token.type != 'eof':
        parse_block(p)
    x = p.tree
    # except:
    if x == None:
        p.error()
    return x

def parsefile(fname):
    try:
        return parse(load(fname))
    except Exception as e:
        printf("parse file %s FAIL", fname)

def tk_list_len(tk):
    if tk == None: return 0
    if tk.type == ',':return tk_list_len(tk.first) + tk_list_len(tk.second)
    return 1
