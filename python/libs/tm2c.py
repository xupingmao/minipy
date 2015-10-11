from parse import *
import sys
from repl import *

tm_obj = "Object "
tm_const = "Object const_"

tm_pusharg = "tm_pusharg("
tm_call = "tm_call("

tm_num = "tm_number("
tm_str = "tm_string("
tm_get_glo = "tm_getglobal("
tm_define = "tm_define"

func_bool = "tm_bool"
func_add = "tm_add"
func_sub = "tm_sub"
func_mul = "tm_mul"
func_div = "tm_div"
func_mod = "tm_mod"
func_cmp = "tm_cmp"

op_bool = [">", ">=", "<", "<+", "==", "!=", "and", "or", "in", "not"]

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
    def __init__(self, fname, prefix = None):
        self.consts = []
        self.scope = Scope(None)
        self.indent = 0
        self.fname = fname
        self.func_defines = []
        self.func_cnt = 0
        if prefix == None:
            self.prefix = fname
        else:
            self.prefix = prefix
    
    def new_scope(self):
        scope = Scope(self.scope)
        self.scope = scope
        
    def exit_scope(self, func_define):
        if self.scope != None:
            self.scope = self.scope.prev
        else:
            raise "fatal error"
        self.func_defines.append(func_define)
        self.func_cnt += 1
        
    def get_cname(self):
        return self.prefix + "_f_" + str(self.func_cnt)
            
    def getconst(self, val):
        if val not in self.consts:
            self.consts.append(val)
        return self.prefix+ "_c_" + str(self.consts.index(val))
        
    def getname(self, name):
        return self.prefix + "_v_" + name
        
    def locals(self):
        if self.scope != None:
            return self.scope.vars
            
    def getglobals(self):
        return self.prefix + "_g";
            
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

def do_assign(item, env, indent = 0, right = None):
    left = item.first
    if right == None:
        right = do_item(item.second, env)
    if left.type == 'name':
        name = left.val
        if not env.has_var(name):
            env.def_var(name)
        return env.getname(name) + "=" + right + ";"
    elif left.type == ",":
        return "// not implemented"

def do_const(item, env):
    val = item.val
    if val not in env.consts:
        env.consts.append(val)
    return env.getconst(val) # "const_" + str(env.consts.index(val))
    
def do_name(item, env):
    if env.has_var(item.val):
        return env.getname(item.val)
    item = Token("string", item.val)
    return tm_get_glo + env.getglobals() + "," + do_const(item, env) + ")"

def do_bool(item, env):
    if item.type in op_bool:
        return do_item(item, env)
    return func_bool + "(" + do_item(item, env) + ")"
    
def do_block(list, env, indent):
    lines = []
    for exp in list:
        line = do_item(exp, env, indent + 2)
        if exp.type == "call": line += ";"
        lines.append(line)
    return lines
    
def do_if(item, env, indent):
    cond = do_bool(item.first, env)
    lines = do_block(item.second, env, indent)
    return sformat("if(%s) %s" , cond, format_block(lines, indent))
    
def do_while(item, env, indent):
    cond = do_bool(item.first, env)
    lines = do_block(item.second, env, indent)
    return sformat("while(%s) %s", cond, format_block(lines, indent))

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
    
def format_block(lines, indent):
    text = "{\n";
    for line in lines: 
        if line != None:
            text += line + "\n"
    return text + indent * " " + "}\n"
    
def do_getargs(list, env, indent):
    r = []
    for item in list:
        if item.type == "varg":
            node = AstNode("=", item.first, item.second)
            code = " " * indent + do_assign(node, env, indent)
            r.append(code)
        line = " " * indent + do_assign(item, env, indent, "tm_getarg()")
        r.append(line)
    return r
    
def do_def(item, env, indent=0):
    env.new_scope()
    name = item.first.val
    args = do_getargs(item.second, env, 2)
    lines = args + do_block(item.third, env, 0)
    cname = env.get_cname()
    locs = env.locals()
    vars = []
    for var in locs:
        vars.append("  Object " + env.getname(var) + ";")
    func_define = "Object " + cname + "() " + format_block(vars+lines, 0)
    env.exit_scope(func_define)
    return sformat("%s(%s, %s, %s);", tm_define, env.getglobals(), env.getconst(name), cname)
    
def do_return(item, env, indent=0):
    return "return " + do_item(item.first, env, 0) + ";"
    
def do_op(item, env, func):
    return func + "(" + do_item(item.first, env) + "," + do_item(item.second, env) + ")";

def do_or(item, env):
    return do_item(item.first, env) + "||" + do_item(item.second, env)
    
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

def do_ne(item, env):
    return "(" + do_op(item, env, "!tm_equals") + ")"
    
def do_eq(item, env):
    return do_op(item, env, "tm_equals")
    
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
    "if": do_if,
    "while": do_while,
    "def": do_def,
    "return": do_return,
}

_handlers2 = {
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
    "!=": do_ne,
    "==": do_eq,
    "+=": do_inplace_add,
    "-=": do_inplace_sub,
    "or": do_or,
    "call": do_call,
}

def do_item(item, env, indent = 0):
    func = None
    if item.type in _handlers:
        func = _handlers[item.type]
        code = func(item, env, indent)
    elif item.type in _handlers2:
        func = _handlers2[item.type]
        code = func(item, env)
    if func != None:
        if indent > 0: code = " " * indent + code
        return code
    #if item.type == "def":
        #name = item.first
        #do_args(name, item.second)
        #do_body(name, item.third)

# env
# consts, globals, scopes.

        
def tm2c(fname, src, prefix=None):
    tree = parse(src)
    #env = {"vars": [], "consts": [], "funcs": [], "globals":[]}
    fname = fname.replace(".", "_")
    env = Env(fname, prefix)
    

    lines = do_block(tree, env, 0)
    _lines = []
    # assign constants
    _lines.append("  " + env.getglobals() + "=dict_new();");
    for const in env.consts:
        h = env.getconst(const) + "="
        if gettype(const) == "number":
            body = tm_num + str(const) + ");"
        elif gettype(const) == "string":
            body = tm_str + '"' + str(const) + '");'
        _lines.append("  " + h + body)
        
    lines = _lines + lines
        
    head = '''#include "include/tm.h"
#include "tm.c"
'''
    head += "/* DEFINE START */\n"
    # define constants
    for const in env.consts:
        head += "Object " + env.getconst(const) + ";\n";
    head += "Object " + env.getglobals() + ";\n"
    # do vars
    for var in env.locals():
        head += tm_obj + env.getname(var) + ";\n"
    head += "/* DEFINE END */\n"
    
    # function define.
    for func_define in env.func_defines:
        head += func_define + "\n"
    
    # global 
    head += "void " + env.prefix + "_main(){\n"
    code =  head + "\n".join(lines)+"\n}\n"
    code += sformat("\nint main(int argc, char* argv[]) {\n  run_py_func(argc, argv, %s_main);\n}\n", env.prefix)
    return code
    
if __name__ == "__main__":
    name = sys.argv[1]
    path = "../test/tm2c/" + name
    text = load(path)
    code = tm2c(name, text, "a")
    print(code)