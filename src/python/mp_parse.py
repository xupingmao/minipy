# -*- coding:utf-8 -*-
# @author xupingmao
# @since 2016
# @modified 2022/06/05 19:31:40
"""Parse minipy code, grammar see minipy.grammar"""

from mp_tokenize import *

if "tm" not in globals():
    from boot import *

_smp_end_list = ['nl', 'dedent']
_skip_op = ['nl', ';']


class AstNode:

    third = None
    pos = [-1,-1]
    children = []

    def __init__(self, type=None, first=None, second=None):
        self.type = type
        self.first = first
        self.second = second
    
class ParserCtx:
    def __init__(self, tokens: list, txt):
        # current token
        self.token = Token("nl", 'nl', None)
        self.eof = Token("eof", 'eof', None)
        tokens.append(self.token)
        tokens.append(self.eof)
        self.tokens = tokens
        self.r = tokens
        self.i = 0
        self.l = len(tokens)
        self.tree = [] # type: list[AstNode]
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

    def last(self):
        return self.tree[-1]

    def add(self, v):
        self.tree.append(v)
        self.last_token = v

    def add_op(self, type):
        right = self.pop()
        left  = self.pop()
        self.add(AstNode(type, left, right))

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
    
    def parse_typing_exp(self):
        exp(self, "factor")
        return self.pop()

def expect_and_next(ctx: ParserCtx, v, msg=None):
    # type: (ParserCtx, any, None|str) -> None
    if ctx.token.type != v:
        error_msg = "expect %r but now is %r::%s" % (v, ctx.token.type, msg)
        compile_error("parse", ctx.src, ctx.token, error_msg)
    ctx.next()

def assert_type(p: ParserCtx, type, msg):
    if p.token.type != type:
        parse_error(p, p.token, msg)
    #p.next()

def add_op(p: ParserCtx, v):
    r = p.tree.pop()
    l = p.tree.pop()
    p.add(AstNode(v, l, r))

def build_op(p: ParserCtx, type):
    right = p.tree.pop()
    left = p.tree.pop()
    return AstNode(type, left, right)


def parse_error(p: ParserCtx, token: Token=None, msg="Unknown"):
    if token != None:
        compile_error('parse', p.src, token, msg)
    else:
        raise("assert_type error")

def baseitem(p: ParserCtx):
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
            expect_and_next(p, ']')
            node.first = p.pop()
        p.add(node)
    elif t == '(':
        p.next()
        exp(p, ',')
        expect_and_next(p, ')')
        # tuple node
        last = p.last()
        if gettype(last) == "list":
            p.pop()
            node = AstNode("tuple", last)
            p.add(node)
    elif t == '{':
        p.next()
        node = AstNode('dict')
        items = []
        while p.token.type != '}':
            exp(p, 'or')
            expect_and_next(p, ':')
            exp(p, 'or')

            v = p.pop()
            k = p.pop()
            items.append([k,v])
            if p.token.type == '}':
                break
            expect_and_next(p, ',')
        if p.token.type == ',':
            p.next()
        expect_and_next(p, '}')
        node.first = items
        p.add(node)

def expr(p):
    return exp(p, "=")

def parse_assign_or_exp(p):
    return exp(p, '=')

def parse_rvalue(p):
    return exp(p, "rvalue")

def parse_var(p):
    token = p.token
    expect_and_next(p, "name")
    p.add(token)

    while True:
        if p.token.type == ".":
            parse_var(p)
            p.add_op("attr")
        elif p.token.type == "[":
            p.next()
            parse_rvalue(p)
            expect_and_next(p, "]")
            p.add_op("get")
        else:
            return p.pop()

def parse_var_list(p):
    """eg var1, var2, var1.attr"""
    vars = [parse_var(p)]
    while p.token.type == ",":
        var = parse_var(p)
        vars.append(var)

    if len(vars) > 1:
        p.add(vars)
    else:
        p.add(vars[0])

def parse_name_list(p):
    # type: (ParserCtx)->list
    """eg name1, name2"""
    name_list = [parse_name(p)]
    while p.token.type == ",":
        p.next()
        token = parse_name(p)
        name_list.append(token)
    return name_list
    
def parse_name(p):
    token = p.token
    expect_and_next(p, "name")
    return token

#  This is not LL grammar
#  Assignment = Lvalue '=' Rvalue
#  Lvalue     = Name | (Rvalue Attr)
#  MpObj     = Name | Number | String | True | False | None | '(' Rvalue ')'
#  Rvalue     = Expression
#  Attr       = ('.' Name) | ('[' Rvalue ']')
#  Expression = CommaExp ( ',' CommaExp)*
#  CommaExp   = OrExp ('or' OrExp)*
def exp(p: ParserCtx, level):
    # Low -> High operator
    if level == '=':
        # TODO: parse_var_list(p)
        exp(p, ',') # lvalue will be checked at compile phase
        if p.token.type in ('=', '+=', '-=', '*=', '/=', '%='):
            t = p.token.type
            p.next()
            exp(p, ',')
            add_op(p, t)
    elif level == ',' or level == 'rvalue':
        exp(p, 'or')
        values = None
        while p.token.type == ',':
            if values == None:
                values = [p.pop()]
            p.next()
            if p.token.type == ']':
                break # for list
            exp(p, 'or')
            next_value = p.pop()
            values.append(next_value)
        if values != None:
            p.add(values)
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
        while p.token.type in ('>', '<', '==', 'is', '!=', 
                '>=', '<=', 'in', 'notin', 'isnot'):
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
        while p.token.type in ('*','/','%'):
            t = p.token.type
            p.next()
            fnc(p)
            add_op(p, t)

def parse_arg_list(p: ParserCtx):
    node = AstNode('call', p.pop()) # name
    if p.token.type == ')':
        p.next()
        node.second = None
        return node
    args = []
    while p.token.type != ')':
        if p.token.type == '*':
            p.next()
            exp(p, 'or')
            arg = p.pop()
            args.append(arg)
            node.type = "apply"
            break # just support one star-arg now
        else:
            exp(p, 'or')
            arg = p.pop()
            args.append(arg)
            if p.token.type == ',':
                p.next() # skip ,
    expect_and_next(p, ')')
    node.second = args # argument-list
    # print(args[0].pos[0])
    return node
                    
def call_or_get_exp(p: ParserCtx):
    if p.token.type == '-':
        p.next()
        call_or_get_exp(p)
        node = AstNode('neg', p.pop())
        p.add(node)
    else:
        baseitem(p)
        while p.token.type in ('.','(','['):
            t = p.token.type
            if t == '[':
                p.next()
                first = p.pop()
                second = None
                third = None
                if p.token.type == ':':
                    second = Token("number", 0, p.token.pos)
                else:
                    exp(p, "or")
                    second = p.pop()
                # handler the second slice parameter
                if p.token.type == ':':
                    p.next()
                    if p.token.type == ']':
                        third = Token("None") # end
                    else:
                        exp(p, 'or')
                        third = p.pop()
                expect_and_next(p, ']')
                if third == None:
                    node = AstNode("get", first, second)
                    p.add(node)
                else:
                    node = AstNode("slice", first, second)
                    node.third = third
                    p.add(node)
            elif t == '(':
                p.next()
                node = parse_arg_list(p)
                p.add(node)
            else:
                p.next()
                baseitem(p) # dot
                b = p.pop()
                a = p.pop()
                node = AstNode('attr')
                node.first = a
                node.second = b
                p.add(node)


def _get_path(obj: AstNode):
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

def _path_check(p: ParserCtx, node: AstNode):
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
    # type: (ParserCtx) -> None
    expect_and_next(p, "from")
    expr(p)
    expect_and_next(p, "import")
    node = AstNode("from")
    node.first = p.pop()
    _path_check(p, node.first)
    
    if p.token.type == "*":
        p.token.type = "string"
        node.second = p.token
    else:
        name_list = parse_name_list(p)
        if len(name_list) != 1:
            raise Exception("from statement can only import one attribute")
        node.second = name_list[0]
        
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
def skip_nl(p: ParserCtx):
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
    expect_and_next(p, ':')
    node = AstNode("try", p.visit_block())
    node.pos = pos
    expect_and_next(p, 'except')
    if p.token.type == 'name':
        p.next()
        if p.token.type == ':':
            second = Token('name', '_')
        else:
            expect_and_next(p, 'as')
            second = p.token
            expect_and_next(p, 'name', 'error in try-expression')
        node.second = second
    else:node.second = None
    expect_and_next(p, ':')
    node.third = p.visit_block()
    p.add(node)

def parse_for_items(p):
    name = p.token
    expect_and_next(p, "name")
    name_list = [name]
    while p.token.type == ",":
        p.next()
        name = p.token
        expect_and_next(p, "name")
        name_list.append(name)
    expect_and_next(p, "in")
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
    expect_and_next(p, ':')
    ast.second = p.visit_block()
    p.add(ast)

def parse_while(p):
    parse_for_while(p, 'while')


def parse_for_while(p: ParserCtx, type):
    ast = AstNode(type)
    p.next()
    expr(p)
    ast.first = p.pop()
    expect_and_next(p, ':')
    ast.second = p.visit_block()
    p.add(ast)

def parse_arg_def(p: ParserCtx):
    expect_and_next(p, '(')
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
        has_varg = False # mark varg is met or not.
        while p.token.type == 'name':
            arg = AstNode("arg")
            arg.first = p.token
            arg.second = None
            p.next()

            if p.token.type == ":":
                # typing hint
                p.next()
                p.parse_typing_exp()

            if has_varg:
                # arg=value
                expect_and_next(p, '=')
                baseitem(p)
                arg.second = p.pop()
            elif p.token.type == '=':
                p.next()
                baseitem(p) # problem
                arg.second = p.pop()
                has_varg = True
            args.append(arg)
            if p.token.type != ',':
                break
            p.next()
        
        if p.token.type == '*':
            # nargs
            p.next()
            assert_type(p, 'name', 'Invalid arguments')
            arg = AstNode("narg", p.token)
            arg.second = None
            args.append(arg)
            p.next()
        
        expect_and_next(p, ')')
        return args

def parse_def(p: ParserCtx):
    p.next()
    assert_type(p, 'name', 'DefException')
    func = AstNode("def")
    func.first = p.token
    p.next()
    func.second = parse_arg_def(p)
    if p.token.type == "->":
        p.next()
        p.parse_typing_exp()
    expect_and_next(p, ':')
    func.third = p.visit_block()
    p.add(func)

def parse_class(p: ParserCtx):
    expect_and_next(p, "class")
    # assert_type(p, 'name','ClassException')
    clazz = AstNode()
    clazz.type = 'class'
    clazz.first = p.token
    expect_and_next(p, "name")

    if p.token.type == '(':
        p.next()
        assert_type(p, 'name', 'ClassException')
        clazz.third = p.token
        p.next()
        expect_and_next(p, ')')
    expect_and_next(p, ':')

    clazz.second = p.visit_block()

    if len(clazz.second) > 0 and clazz.second[0].type == "string":
        doc = clazz.second[0]
        del clazz.second[0]
        clazz.doc = doc

    p.add(clazz)

def parse_stm1(p: ParserCtx, type):
    p.next()
    node = AstNode(type)
    if p.token.type in _smp_end_list:
        node.first = None
    else:
        expr(p)
        node.first = p.pop()
    p.add(node)

            

def parse_if(p: ParserCtx):
    ast = AstNode("if")
    p.next()
    expr(p)
    ast.first = p.pop()
    expect_and_next(p, ':')
    ast.second = p.visit_block()
    ast.third = None
    cur = ast # temp
    temp = cur 
    if p.token.type == 'elif':
        while p.token.type == 'elif':
            node = AstNode("if")
            p.next()
            expr(p)
            expect_and_next(p, ':')
            node.first = p.pop()
            node.second = p.visit_block()
            node.third = None
            cur.third = node
            cur = node
    if p.token.type == 'else':
        p.next()
        expect_and_next(p, ':')
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
    
def parse_multi_assign(p: ParserCtx):
    p.next()
    expr(p) # left
    expect_and_next(p, ']')
    expect_and_next(p, '=')
    expr(p) # right
    add_op(p, "=")


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
    "[":    parse_multi_assign,
    "name": parse_assign_or_exp,
    "number": expr,
    "string": expr,
    "try": parse_try,
    "global": parse_global,
    "del": parse_del,
    ';': parse_skip,
    "@": parse_annotation,
}


def parse_stm(p: ParserCtx):
    t = p.token.type
    if t not in stmt_map:
        parse_error(p, p.token, "Unknown Expression")
    else:
        return stmt_map[t](p)


def parse_block(p: ParserCtx):
    skip_nl(p)
    if p.token.type == 'indent':
        p.next()
        while p.token.type != 'dedent':
            parse_stm(p)
            skip_nl(p)
        p.next()
    elif p.token.type == "eof":
        # 空文件
        return
    else:
        parse_stm(p)
        while p.token.type == ';':
            if p.token.type in ('nl', 'eof'):
                break
            else:
                parse_stm(p)
        skip_nl(p)
            
    
def parse(content: str):
    """解析入口"""
    tokens = tokenize(content)
    p = ParserCtx(tokens, content)
    p.next()
    try:
        while p.token.type != 'eof':
            parse_block(p)
        x = p.tree
        # except:
        if x == None:
            parse_error(p, p.token, "tree is None")
        return x
    except Exception as e:
        # print(e, v)
        compile_error("parse", content, p.token, str(e))
        raise(e)

def xml_item(type, value):
    return "<" + type + ">" + ("%r" % value) + "</" + type + ">"

def xml_start(type):
    return "<" + type + ">"

def xml_close(type):
    return "</" + type + ">"

def xml_line_head(n):
    return " " * n

def print_ast_line_pos(tree: AstNode):
    if hasattr(tree, "pos"):
        pos = tree.pos
        # skip invalid pos
        if pos is None:
            return
        if len(pos) == 0:
            return

        line = tree.pos[0]
        line_str = str(line)
        space = 4 - len(line_str)
        printf("<!--" + line_str.ljust(4) + '-->')
    else:
        printf("<!--****-->")

def print_ast_line(n, node):
    print_ast_line_pos(node)
    print(xml_line_head(n), xml_item(node.type, node.val))

def print_ast_block_start(n, node):
    print_ast_line_pos(node)
    print(xml_line_head(n), xml_start(node.type))

def print_ast_block_close(n, node):
    print_ast_line_pos(node)
    print(xml_line_head(n), xml_close(node.type))

# MP_TEST
def print_ast_obj(tree: AstNode, n=0):
    if tree == None:
        return
    if gettype(tree) == "list":
        return print_ast_list(tree, n)

    # literal
    if tree.type in ("number", "string", "None", "name"):
        print_ast_line(n, tree)
        return

    # statement
    if tree.type == "name":
        print_ast_line(n, tree)
    else:
        print_ast_block_start(n, tree)

    if hasattr(tree, "first"):
        print_ast(tree.first, n+2)
    if hasattr(tree, "doc"):
        print_ast(tree.doc, n+2)
    if hasattr(tree, "second"):
        print_ast(tree.second, n+2)
    if hasattr(tree, "third"):
        print_ast(tree.third, n+2)

    print_ast_block_close(n, tree)

def print_ast_list(tree, n=0):
    print_ast_line_pos(tree)
    print(xml_line_head(n), "<block>")
    for item in tree:
        print_ast_obj(item, n+2)
    print_ast_line_pos(tree)
    print(xml_line_head(n), "</block>")

def print_ast(tree, n=0):
    return print_ast_obj(tree, n)

def parsefile(fname):
    try:
        return parse(load(fname))
    except Exception as e:
        printf("parse file %s FAIL", fname)

def tk_list_len(tk: AstNode):
    if tk == None: return 0
    if tk.type == ',':return tk_list_len(tk.first) + tk_list_len(tk.second)
    return 1



class ArgReader:
    """参数阅读器,使用了迭代器模式"""

    def __init__(self, args: "list[str]"):
        self.args = args
        self.index = -1
        self.max_index = len(args) - 1

    def has_next(self):
        return self.index < self.max_index

    def next(self):
        self.index += 1
        return self.args[self.index]


if __name__ == "__main__":
    import sys
    tree = parsefile(sys.argv[1])
    print_ast(tree)

# MP_TEST_END

