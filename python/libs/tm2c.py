from parse import *

tm_obj = "Object "
tm_const = "Object const_"

tm_pusharg = "tm_pusharg("
tm_call = "tm_call("

tm_num = "tm_number("
tm_str = "tm_string("
tm_get_glo = "tm_get_glo("

func_bool = "tm_bool("
func_add = "tm_add("
func_sub = "tm_sub("
func_mul = "tm_mul("
func_div = "tm_div("
func_mod = "tm_mod("
func_cmp = "tm_cmp("

op_bool = [">", ">=", "<", "<+"]

class AstNode:
    def __init__(self, type, first = None, second = None):
        self.type = type
        self.first = first
        self.second = second
        
class Scope:
    def __init__(self, prev):
        self.vars = []
        self.prev = prev
        
class Env:
    def __init__(self):
        self.consts = []
        self.scope = Scope(None)
    
    def new_scope(self):
        scope = Scope(self.scope)
        self.scope = scope
        
    def exit_scope(self):
        if self.scope != None:
            self.scope = self.scope.prev
            
    def locals(self):
        if self.scope != None:
            return self.scope.vars
            
    def has_var(self, name):
        scope = self.scope
        while scope != None:
            if name in scope.vars:
                return True
            scope = scope.prev
        return False
        
    def def_var(self, name):
        if self.has_var(name):
            pass
        else:
            self.scope.vars.append(name)

def do_assign(item, env):
    left = item.first
    right = do_item(item.second, env)
    if left.type == 'name':
        name = left.val
        if not env.has_var(name):
            env.def_var(name)
        return left.val + "=" + right + ";"
    elif left.type == ",":
        return "// not implemented"

def do_const(item, env):
    val = item.val
    if val not in env.consts:
        env.consts.append(val)
    return "const_" + str(env.consts.index(val))
    
def do_name(item, env):
    if env.has_var(item.val):
        return item.val
    item = Token("string", item.val)
    return tm_get_glo + do_const(item, env) + ")"

def do_bool(item, env):
    if item.type in op_bool:
        return do_item(item, env)
    return func_bool + do_item(item, env) + ")"
    
def do_while(item, env):
    cond = do_bool(item.first, env)
    lines = []
    for exp in item.second:
        lines.append(do_item(exp, env))
    return "while(" + cond + "){\n" + "\n".join(lines)+"\n}"

def do_args(item, env):
    if item == None:return ""
    if item.type == ',':
        return do_args(item.first, env) + "," + do_args(item.second, env)
    return do_item(item, env)
    
def do_call(item, env):
    name = do_item(item.first, env)
    n = tk_list_len(item.second)
    args = do_args(item.second, env)
    if len(args) != 0:
        args = "," + args
    return tm_call + name + "," + str(n) + args + ")"
    
def do_op(item, env, func):
    return func + do_item(item.first, env) + "," + do_item(item.second, env) + ")";

def do_mul(item, env):
    return do_op(item, env, func_mul)
    
def do_div(item, env):
    return do_op(item, env, func_div)
    
def do_mod(item, env):
    return do_op(item, env, func_mod)
    
def do_add(item, env):
    return do_op(item, env, func_add)
    
def do_sub(item, env):
    return do_op(item, env, func_sub)
    
def do_lt(item, env):
    return do_op(item, env, func_cmp) + "<0"
    
def do_gt(item, env):
    return do_op(item, env, func_cmp) + ">0"
    
def do_le(item, env):
    return do_op(item, env, func_cmp) + "<=0"
    
def do_ge(item, env):
    return do_op(item, env, func_cmp) + ">=0"

def do_inplace_add(item, env):
    item2 = AstNode("+", item.first, item.second)
    tk = AstNode("=", item.first, item2)
    return do_assign(tk, env)
    
def do_inplace_sub(item, env):
    item2 = AstNode("-", item.first, item.second)
    tk = AstNode("=", item.first, item2)
    return do_assign(tk, env)
    
_handlers = {
    "=" : do_assign,
    "number": do_const,
    "string": do_const,
    "name": do_name,
    "+": do_add,
    "-": do_sub,
    "*": do_mul,
    "/": do_div,
    "<": do_lt,
    ">": do_gt,
    ">=": do_ge,
    "<=": do_le,
    "+=": do_inplace_add,
    "-=": do_inplace_sub,
    "while": do_while,
    "call": do_call,
}

def do_item(item, env):
    if item.type in _handlers:
        return _handlers[item.type](item, env)
    #if item.type == "def":
        #name = item.first
        #do_args(name, item.second)
        #do_body(name, item.third)

# env
# consts, globals, scopes.

        
def tm2c(fname, src):
    tree = parse(src)
    lines = []
    #env = {"vars": [], "consts": [], "funcs": [], "globals":[]}
    env = Env()
    for item in tree:
        line = do_item(item, env)
        lines.append(line)
    defines = []
    
    # do consts
    for const in env.consts:
        head = tm_const + str(env.consts.index(const)) + "="
        if gettype(const) == "number":
            body = tm_num + str(const) + ");"
        elif gettype(const) == "string":
            body = tm_str + '"' + str(const) + '");'
        defines.append(head + body)
    # do vars
    for var in env.locals():
        defines.append(tm_obj + var + ";")
    lines = defines + lines
    head="void " + fname + "_main(){\n"
    return head + "\n".join(lines)+"\n}"
    
code = '''
x = 10;
y = 30;
z = x * y;
i = 1;
zz = "hello,world"
while i < 10:
    z = x + i
    i += 1
    z -= 10
    xy = zz * 2
'''
code = load("../test/testspeed.py")
if __name__ == "__main__":
    code = tm2c("test", code)
    print(code)