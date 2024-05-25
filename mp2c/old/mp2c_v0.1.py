# version: 0.1
# 这个版本是通过语法树来生成C语言代码的
import sys
import os
from mp_parse import *


mp_obj               = "MpObj "
mp_const             = "MpObj const_"
mp_pusharg           = "mp_pusharg("
mp_call              = "mp_call("
mp_num               = "number_obj("
mp_str               = "string_static"
mp_get_glo           = "mp_get_global_by_cstr"
mp_define            = "def_func"
def_native_method    = "def_native_method"
mp_call_native       = "mp_call_native"
mp_call_native_0     = "mp_call_native_0"
mp_call_native_1     = "mp_call_native_1"
mp_call_native_2     = "mp_call_native_2"
mp_insert_arg           = "mp_insert_arg"
func_bool            = "mp_is_true"
func_add             = "obj_add"
func_sub             = "obj_sub"
func_mul             = "obj_mul"
func_div             = "obj_div"
func_mod             = "obj_mod"
func_LT              = "obj_LT"
func_LE              = "obj_LE"
func_GT              = "obj_GT"
func_GE              = "obj_GE"
func_not             = "obj_not"
func_neg             = "obj_neg"
func_get             = "obj_get"
func_get_attr        = "tm2c_get"
func_set             = "obj_set"
func_list            = "argv_to_list"
func_method          = "def_method"
gc_track_local       = "gc_track_local"
gc_pop_locals        = "gc_pop_locals"
mp_None              = "NONE_OBJECT"
FUNC_GET_NUM         = "GET_NUM"

op_bool = [">", ">=", "<", "<+", "==", "!=", "and", "or", "in", "not", "notin"]

BUILTIN_FUNC_MAPPING = {
    "print":     "bf_print",
    "str"  :     "bf_str",
    "newobj":    "bf_newobj",
    "hasattr":   "bf_hasattr",
    "getattr":   "bf_getattr",
    "gettype":   "bf_gettype",
    "raise" :    "bf_raise",
    "Exception": "bf_Exception",
    "len":       "bf_len",
    "str":       "bf_str",
    "mmatch":    "bf_mmatch",
    "float":     "bf_float",
    "int":       "bf_int",
    "chr":       "bf_chr",
    "ord":       "bf_ord",
    "load":      "bf_load",
    "save":      "bf_save",
    "get_mp_local_list": "bf_get_mp_local_list",
}

def escape(text):
    if not istype(text, "string"):
        raise "<function escape> expect a string"
    des = ""
    for c in text:
        if c == '\r':des+='\\r'
        elif c == '\n':des+='\\n'
        elif c == '\b':des += '\\b'
        elif c == '\\':des += '\\\\'
        elif c == '\"':des += '\\"'
        elif c == '\0':des += '\\0'
        elif c == '\t':des += '\\t'
        else:des+=c
    return des

def get_string_def(value):
    return '"' + escape(str(value)) + '"'

def build_native_call(func, *args):
    if len(args) == 0:
        return "{}({});".format(mp_call_native_0, func)
    elif len(args) == 1:
        return "{}({},{});".format(mp_call_native_1, func, ",".join(args))
    elif len(args) == 2:
        return "{}({},{});".format(mp_call_native_2, func, ",".join(args))
    else:
        return "{}({},{},{});".format(mp_call_native, func, len(args), ",".join(args))
        

def is_string_token(token):
    return token.type == "string"

def is_name_token(token):
    return token.type == "name"

def is_number_token(token):
    return token.type == "number"

def is_attr_token(token):
    return token.type == "name" or token.type == "string"

def is_indent_func(func):
    return func is do_if \
        or func is do_for\
        or func is do_while

class Scope:
    def __init__(self, prev):
        self.vars = []
        self.g_vars = []
        self.prev = prev
        self.temp = 0
    def add_global_var(self, name):
        if name not in self.g_vars:
            self.g_vars.append(name)
        
class Env:
    def __init__(self, fname, option):
        self.consts = []
        self.scope = Scope(None)
        self.global_scope = self.scope
        self.indent = 0
        self.fname = fname
        self.mod_name = fname.split(".")[0]
        self.func_defines = []
        self.func_cnt = 0
        self.enable_gc = option.enable_gc
        self.debug = option.debug
        self.record_line = option.record_line
        self.is_module = option.is_module
        self.multi_line = option.multi_line
        self.readable = option.readable
        self.include_list = []
        if option.prefix == None:
            name = fname.split(".")[0]
            self.prefix = get_valid_code_name(name) + "_"
        else:
            self.prefix = option.prefix + "_"

        self.builtin_dict = BUILTIN_FUNC_MAPPING
    
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
        
    def find_all_func_def(self, tree):
        py_func_list = []
        for item in tree:
            type = item.type
            if type == "def":
                name = item.first.val
                py_func_list.append(name)
            elif type == "class":
                self.find_class_func_def(item, py_func_list)
        self.py_func_list = py_func_list
        return py_func_list

    def find_class_func_def(self, item, func_list):
        clz_name = item.first.val
        body = item.second
        for func_def in body:
            if func_def.type == "def":
                name = func_def.first.val
                func_list.append(clz_name+"_"+name)
        func_list.append(clz_name)

    def get_native_func_name(self, name):
        # return self.prefix + "F" + str(self.func_cnt) 
        if name in self.py_func_list:
            return self.prefix + name
        elif name in self.builtin_dict:
            return self.builtin_dict[name]
        return None

    def get_c_func_def(self, name):
        return self.prefix + name

    def get_mod_name(self):
        return self.mod_name
            
    def get_const(self, val):
        if self.readable:
            if istype(val, "string"):
                return "S({})".format(get_string_def(val))
            elif istype(val, "number"):
                return "N({})".format(val)
            elif val == None:
                return "NONE_OBJECT"
            elif val == True:
                return "TRUE_OBJECT"
            elif val == False:
                return "FALSE_OBJECT"
            else:
                return "U({})".format(val)
        elif val not in self.consts:
            self.consts.append(val)
        return self.prefix+ "C" + str(self.consts.index(val))
        
    def get_var_name(self, name):
        return self.prefix + "V" + name

    def get_temp_var_name(self):
        temp = new_temp(self)
        return self.prefix + "V" + temp

    # you must declare temp var in your scope
    def get_temp_ptr_name(self):
        temp = new_temp(self)
        return self.prefix + "P" + temp
        
    def locals(self):
        if self.scope != None:
            return self.scope.vars
            
    def get_globals(self):
        return self.prefix + "0g";
            
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

    def add_global_var(self, name):
        self.scope.add_global_var(name)

    def add_include(self, name):
        self.include_list.append(name)

    def is_global_var(self, name):
        return self.scope == self.global_scope or \
            name in self.scope.g_vars




def get_valid_code_name(fname):
    fname = fname.replace("-", "_")
    fname = fname.replace(".", "_")
    return fname
        

class AstNode:
    def __init__(self, type, first = None, second = None):
        self.type = type
        self.first = first
        self.second = second

def get_py_func_name_str(item):
    return item.first.val

def set_py_func_name(item, name):
    item.first.val = name


def gen_constants_def(env):
    """generate constants definition"""
    if env.readable:
        return "MpObj " + env.get_globals() + ";\n"

    head = ""
    for const in env.consts:
        head += "MpObj " + env.get_const(const) + ";\n";
    head += "MpObj " + env.get_globals() + ";\n"
    return head

def gen_constants_init(env):
    """generate constants init"""
    define_lines = []
    # assign constants
    # define_lines.append("  " + env.get_globals() + "=dict_new();");
    define_lines.append(env.get_globals() + "=dict_new();")
    
    if env.readable:
        return define_lines

    for const in env.consts:
        h = env.get_const(const) + "="
        if gettype(const) == "number":
            body = "number_obj(" + str(const) + ");"
        elif gettype(const) == "string":
            body = "{}({});".format(mp_str, get_string_def(str(const)))
        define_lines.append(h+body)
    return define_lines

class Generator:

    def __init__(self, env):
        self.env = env
        self.is_module = env.is_module

    def gen_clang_declare_list(self):
        """generate c function def list"""
        lines = ["/* Function Definition */"]
        env = self.env
        for py_func in self.env.py_func_list:
            line = "MpObj {}();".format(env.get_c_func_def(py_func))
            lines.append(line)
        lines.append("/* Function Definition End */")
        return "\n".join(lines) + "\n\n"

    def gen_clang_head(self):
        head  = "#define MP_NO_BIN 1\n"
        head += '#include "../src/vm.c"\n'
        head += '#include "../mp2c/mp2c.c"\n'
        head += "#define S string_new\n"
        head += "#define N number_obj\n"
        return head

    def process(self, lines):
        env = self.env
        define_lines = gen_constants_init(env)
        define_lines.append("mp_def_mod(\"{}\", {});".format(env.get_mod_name(), env.get_globals()))
            
        lines = define_lines + lines
        head  = self.gen_clang_head()

        head += "/* DEFINE START */\n"
        # include list
        for include in env.include_list:
            head += '#include "{}.c"\n'.format(include)

        # define constants
        head += gen_constants_def(env)
        # do vars
        for var in env.locals():
            head += "MpObj " + env.get_var_name(var) + ";\n"
        head += "/* DEFINE END */\n\n"
        
        head += self.gen_clang_declare_list()

        # function define.
        for func_define in env.func_defines:
            head += func_define + "\n\n"
        
        # globals
        head += "/* Module: {} */\n".format(env.mod_name)
        head += "MpObj " + env.prefix + "0(){\n  "
        body = "\n  ".join(lines)+"\n  return NONE_OBJECT;\n}\n"
        code = head + body

        if not self.is_module:
            # main entry
            code += sformat("\nint main(int argc, char* argv[]) {\n  mp_run_func(argc, argv, \"%s\", %s0);\n}\n", env.get_mod_name(), env.prefix)
        self.code = code

    def get_code(self):
        return self.code


def new_temp(env):
    c = env.scope.temp
    env.scope.temp += 1
    name = str(c)
    env.def_var(name)
    return name

def do_assign(item, env, right = None):
    left = item.first
    if istype(left, "list"):
        raise Exception("multi-assignment not implemented")
    if right == None:
        right = do_item(item.second, env)
    if left.type == 'name':
        name = left.val
        if not env.has_var(name):
            env.def_var(name)
        code = env.get_var_name(name) + "=" + right + ";"
        if env.is_global_var(name):
            code += "\n" + "obj_set({}, {}, {});".format(env.get_globals(), \
                do_const(left, env), env.get_var_name(name))
        return code
    elif left.type == ",":
        return "// not implemented"
    elif left.type == "get":
        lfirst = do_item(left.first, env)
        lsecond = do_item(left.second, env)
        return sformat("%s(%s,%s,%s);", func_set, lfirst, lsecond, right)
    elif left.type == "attr":
        # lfirst = Token("string", left.first.val)
        lfirst = do_item(left.first, env)
        return sformat("%s(%s,%s,%s);", func_set, lfirst, do_const(left.second,env), right)

def do_const(item, env):
    val = item.val
    if val not in env.consts:
        env.consts.append(val)
    return env.get_const(val) # "const_" + str(env.consts.index(val))
    
def do_name(item, env):
    """return name as variable"""
    if env.has_var(item.val):
        return env.get_var_name(item.val)
    # item = Token("string", item.val)
    # return "{}({},{})".format(mp_get_glo, env.get_globals(), do_const(item, env))
    # return do_get(item, env)

    # get gloabl variable
    return "{}({},{})".format(func_get_attr, env.get_globals(), get_string_def(item.val))

def do_none(item, env):
    return "NONE_OBJECT"

def do_list0(v, env):
    newlist = []
    cnt = 0
    for item in v:
        code = do_item(item, env)
        newlist.append(code)
        cnt = len(newlist)
    if cnt == 0:
        return sformat("%s(0)", func_list)
    else:
        if env.multi_line:
            return "{}({},{})".format(func_list, cnt, ",\n  ".join(newlist))
        else:
            return sformat("%s(%s,%s)", func_list, cnt, ",".join(newlist)) 

def do_list(item, env):
    v = item.first
    cnt = 0
    if v == None:
        cnt = 0
        return sformat("%s(0)", func_list)
    newlist = []
    if istype(v, "list"):
        return do_list0(v, env)
    else:
        newlist.append(do_item(v, env))
        cnt = 1
    if cnt == 0:
        return sformat("%s(0)", func_list)
    else:
        return sformat("%s(%s,%s)", func_list, cnt, ",".join(newlist)) 
    
def do_dict(item, env):
    nodes = item.first
    nodes_list = []
    for node in nodes:
        k = node[0]
        v = node[1]
        node = do_item(k, env) + "," + do_item(v, env)
        nodes_list.append(node)
        # nodes_str += "," + do_item(k, env)
        # nodes_str += "," + do_item(v, env)

    if env.multi_line:
        nodes_str = ",\n  ".join(nodes_list)
    else:
        nodes_str = ",".join(nodes_list)

    return "{}({} {})".format("argv_to_dict", int(len(nodes)/2), nodes_str)

def do_bool(item, env):
    if item.type in op_bool:
        return do_item(item, env)

    value = do_item(item, env)
    if value is None:
        print("do_bool: do_item return None")
        value = "NONE_OBJECT"
    return func_bool + "(" + value + ")"
    
def do_block(list, env, indent=0):
    # indent will be handled by format_block
    lines = []
    for exp in list:
        if exp.type == "string": 
            continue

        line = do_item(exp, env, indent)
        if exp.type == "call":
            line += ";"

        if env.record_line:
            lineno    = get_line_no(exp)
            code_line = env.origin_lines[lineno-1]
            code = 'printf("%s\\n", {});'.format(get_string_def(code_line))
            lines.append(code)

        if line is not None:
            lines.append(line)
    return lines
    
def do_if(item, env, indent=0):
    cond  = do_bool(item.first, env)
    lines = do_block(item.second, env, indent+2)
    third = item.third
    head  = format_indent(indent) + sformat("if(%s) %s", cond, format_block(lines, indent+2))
    if third == None:
        pass
    elif gettype(third) == "list":
        return head + "else" + format_block(do_block(third, env, 2),indent+2)
    else:
        return head + "else " + do_item(third, env)
    return head

def do_for(item, env, indent=0):
    temp     = env.get_temp_var_name()
    temp_ptr = env.get_temp_ptr_name()
    expr     = item.first
    block    = item.second

    names = expr.first
    assert len(names) == 1, "only support 1 item for statement"
    _keyname = names[0].val

    iterator = do_item(expr.second, env)

    env.def_var(_keyname)
    keyname = env.get_var_name(_keyname)
    
    key = do_name(names[0], env);
    init = sformat("%s = iter_new(%s);", temp, iterator)
    init += "\n" + "MpObj* " + temp_ptr + ";";
    get_next = "{} = obj_next({})".format(temp_ptr, temp)
    # head = "while ({} != NULL)".format(temp_ptr)
    key_assignment = "{} = *{};".format(keyname, temp_ptr)

    head = "for({}; {} != NULL; {})".format(get_next, temp_ptr, get_next)
    lines = do_block(block, env)
    lines.insert(0, key_assignment)
    return sformat("%s\n%s %s", init, head, format_block(lines))

    
def do_while(item, env, indent=0):
    cond = do_bool(item.first, env)
    lines = do_block(item.second, env, indent)
    return sformat("while(%s) %s", cond, format_block(lines, indent))

def do_args(item, env):
    if item == None:
        return ""
    elif istype(item, "list"):
        args = "";
        for i in item:
            args += "," + do_item(i, env)
        return args
    return "," + do_item(item, env)

def get_line_no(item):
    if hasattr(item, "pos"):
        return item.pos[0]
    elif hasattr(item, "first"):
        return get_line_no(item.first)
    else:
        return -1
    
def do_call(item, env):
    if item.first.type == "name":
        name = env.get_native_func_name(item.first.val)
        if name:
            if env.debug:
                py_func_name = item.first.val
                fmt = "mp_call_native_debug({}, " + get_string_def(py_func_name) + ",{} {})"
                # fmt = "mp_call_native_debug({}, {}, {} {})"
            else:
                fmt = "mp_call_native({}, {} {})"
        else:
            name = do_item(item.first, env)
            fmt  = "mp_call({},{} {})"
    else:
        name = do_item(item.first, env)
        fmt = "mp_call({}, {} {})"
    if item.second == None:
        n = 0
    elif istype(item.second, "list"):
        n = len(item.second)
    else:
        n = 1
    args = do_args(item.second, env)
    # mp_call(lineno, func, nargs, args)
    return fmt.format(name, n, args)
    # return mp_call + name + "," + + "," + str(n) + args + ")"

def format_indent(indent):
    return " " * indent

def format_block(lines, indent=0):
    text = "{\n";
    for line in lines: 
        if line != None:
            text += format_indent(indent) + line + "\n"
    return text + format_indent(indent - 2) + "}"
    
def do_getargs(list, env, indent):
    r = []
    for item in list:
        if item.type == "varg":
            node = AstNode("=", item.first, item.second)
            code = do_assign(node, env)
            r.append(code)
        line = do_assign(item, env, "mp_take_arg()")
        r.append("  " + line)
    return r
    
def do_def(item, env, obj=None):
    env.new_scope()

    name = item.first.val
    args = do_getargs(item.second, env, 2)
    lines = args + do_block(item.third, env, 2)
    cname = env.get_c_func_def(name)
    if obj!=None:
        obj.name = cname
        obj.constname = env.get_const(name)
    locs = env.locals()
    vars = ["  // Function: " + name]
    for var in locs:
        vars.append("  MpObj " + env.get_var_name(var) + ";")
    vars.append("  MpObj ret = {};".format(mp_None))

    # gc handle
    if env.enable_gc:
        for var in locs:
            vars.append("{}(&{});".format(gc_track_local, env.get_var_name(var)))

    lines.append("func_end:")

    if env.enable_gc:
        lines.append("gc_pop_locals({});".format(len(locs)))
    # return
    lines.append("  return ret;")
    func_define = "MpObj " + cname + "() " + format_block(vars+lines, 0)

    env.exit_scope(func_define)

    return sformat("%s(%s,%s,%s);", 
        mp_define, env.get_globals(), env.get_const(name), cname)
    
def do_return(item, env):
    indent = env.indent
    # free tracked locals
    ret = do_item(item.first, env, 0)
    if ret == "":
        # print("do_return_1: indent={}".format(indent))
        return format_indent(indent) + "goto func_end;";
    else:
        # print("do_return_2: indent={}".format(indent))
        return "ret = {};\n{}goto func_end;".format(ret, format_indent(indent))
    
def do_class(item, env):
    class_define = do_assign(item, env, "dict_new()")
    class_name = do_name(item.first, env)
    clz_name_str = item.first.val
    obj = newobj()
    lines = []
    env.new_scope()
    lines.append("MpObj d = dict_new();");
    init_func = None

    for class_item in item.second:
        type = class_item.type
        if type == "pass": continue
        assert type == "def"
        def_name = get_py_func_name_str(class_item)
        method_name = clz_name_str + "_" + def_name
        set_py_func_name(class_item, method_name)
        if def_name == "__init__":
            init_func = env.get_c_func_def(method_name)
        do_def(class_item, env)
        constname = env.get_const(def_name)
        # lines.append(sformat("%s(%s,%s,%s);", func_method, class_name, constname, env.get_c_func_def(def_name)))
        lines.append("{}(d, {}, {});".format(def_native_method, constname, env.get_c_func_def(method_name)))

    if init_func != None:
        lines.append("{}(d);".format(mp_insert_arg))
        lines.append("{}();".format(init_func))

    lines.append("return d;");
    class_def = "MpObj {} () {} ".format(env.get_c_func_def(clz_name_str), format_block(lines))
    env.exit_scope(class_def);

    # text = class_define + "\n"
    # for line in lines:
    #     text += line + "\n"
    return "{}({},{},{});".format(mp_define, env.get_globals(), env.get_const(clz_name_str), env.get_c_func_def(clz_name_str))
    

def do_try(item, env):
    # ignore except block
    body = item.first
    lines = do_block(body, env)
    return "\n".join(lines)

def do_op(item, env, func, number_op=None):
    """ generate operation code of C
        this function will do optimization to number operator
    """
    if env.readable and number_op is not None:
        if is_number_token(item.first):
            left = str(item.first.val)
            right = "{}({})".format(FUNC_GET_NUM, do_item(item.second, env))
            return left + number_op + right
        elif is_number_token(item.second):
            left = "{}({})".format(FUNC_GET_NUM, do_item(item.first, env))
            right = str(item.second.val)
            return left + number_op + right
    return func + "(" + do_item(item.first, env) + "," + do_item(item.second, env) + ")";

def do_or(item, env):
    if env.multi_line:
        op_str = "\n  ||"
    else:
        op_str = "||"
    return "(" + do_item(item.first, env) + op_str + do_item(item.second, env) + ")"
    
def do_and(item, env):
    if env.multi_line:
        op_str = "\n  &&"
    else:
        op_str = "&&"
    return "(" + do_item(item.first, env) + op_str + do_item(item.second, env) + ")"

    
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
    return do_op(item, env, func_LT)
    
def do_gt(item, env):
    return do_op(item, env, func_GT)
    
def do_le(item, env):
    return do_op(item, env, func_LE)
    
def do_ge(item, env):
    return do_op(item, env, func_GE)

def do_ne(item, env):
    return "(" + do_op(item, env, "!mp_is_equals", "!=") + ")"
    
def do_eq(item, env):
    return do_op(item, env, "mp_is_equals", "==")
    
def do_not(item, env):
    value = do_item(item.first, env)
    if value is None:
        raise Exception("do_not: value is None")
    if not value.startswith("mp_is_true"):
        return "!mp_is_true(" + value + ")"
    return sformat("!(%s)", value)

def do_notin(item, env):
    return do_op(item, env, "mp_is_not_in")

def do_neg(item, env):
    return sformat("%s(%s)", func_neg, do_item(item.first, env))
    
def do_get(item, env):
    if env.readable:
        if is_string_token(item.second):
            left = do_item(item.first, env)
            right = get_string_def(item.second.val)
            return '{}({},{})'.format(func_get_attr, left, right)
    return do_op(item, env, func_get)
    
def do_attr(item, env):
    item.second.type = "string" # handle attr as string
    return do_get(item, env)

def do_in(item, env):
    # TODO a in a,b,c
    return do_op(item, env, "obj_is_in")

def do_is(item, env):
    return do_op(item, env, "obj_is")

def do_isnot(item, env):
    return do_op(item, env, "obj_isnot")

def do_slice(item, env):
    value = do_item(item.first, env)
    left  = do_item(item.second, env)
    right = do_item(item.third, env)
    return "obj_slice({},{},{})".format(value, left, right)
    
def do_inplace_op(item, env, op):
    item2 = AstNode(op, item.first, item.second)
    tk = AstNode("=", item.first, item2)
    return do_assign(tk, env)
    
def do_inplace_add(item, env):
    return do_inplace_op(item, env, "+")
    
def do_inplace_sub(item, env):
    return do_inplace_op(item, env, "-")
    
def do_inplace_mul(item, env):
    return do_inplace_op(item, env, "*")
    
def do_inplace_div(item, env):
    return do_inplace_op(item, env, "/")
    
def do_inplace_mod(item, env):
    return do_inplace_op(item, env, '%')


def do_import(item, env):
    mod = do_const(item.first, env)
    return "mp_import({}, {});".format(env.get_globals(), mod)

def do_from(item, env):
    mod = do_const(item.first, env)
    # modname = item.first.val
    # env.add_include(modname)
    # return build_native_call(modname+"_Iinit")
    return "mp_import_all({}, {});".format(env.get_globals(), mod)    

def do_global(item, env):
    g_name = item.first.val
    env.add_global_var(g_name)
    # name = do_const(item.first, env)
    code = "// global " + g_name
    # code += "obj_set(%s, %s, NONE_OBJECT);".format(env.get_globals(), name)
    return code

def do_break(item, env):
    return "break;"

def do_continue(item, env):
    return "continue;"

def do_pass(item, env):
    return ""

_handlers = {
    "if":     do_if,
    "while":  do_while,
    "def":    do_def,
    "return": do_return,
    "class":  do_class,
    "for":    do_for,
    "try":    do_try,

    # items
    "number": do_const,
    "string": do_const,
    "name": do_name,
    "None": do_none,
    "list": do_list,
    "dict": do_dict,
    "=": do_assign,
    "+": do_add,
    "-": do_sub,
    "*": do_mul,
    "/": do_div,
    "%": do_mod,
    "<": do_lt,
    ">": do_gt,
    ">=": do_ge,
    "<=": do_le,
    "!=": do_ne,
    "==": do_eq,
    "+=": do_inplace_add,
    "-=": do_inplace_sub,
    "*=": do_inplace_mul,
    "/=": do_inplace_div,
    "%=": do_inplace_mod,
    "or": do_or,
    "and": do_and,
    "not": do_not,
    "notin" : do_notin,
    "neg": do_neg,
    "call": do_call,
    "get": do_get,
    "attr": do_attr,
    "in": do_in,
    "is": do_is,
    "isnot": do_isnot,
    "slice": do_slice,

    "import": do_import,
    "from": do_from,
    "global": do_global,
    "break": do_break,
    "continue": do_continue,
    "pass": do_pass,
}

def do_item(item, env, indent = 0):
    func = None
    env.token  = item
    env.indent = indent

    if item == None:
        return ""
    if istype(item, "list"):
        return do_list0(item, env);
    
    if not hasattr(item, "type"):
        repl_print(item, 1, 4)
        raise "Unknown type AstNode"

    if item.type in _handlers:
        func = _handlers[item.type]
        if is_indent_func(func):
            # print("do_item: is_indent_func")
            code = func(item, env, indent)
        else:
            code = func(item, env)

    if func != None:
        # if indent > 0: code = " " * indent + code
        return code

    raise Exception("do_item: no handler found, item.type=" + item.type)

# env
# consts, globals, scopes.

def do_program(tree, env, indent):
    return do_block(tree, env, 0)

    try:
        return do_block(tree, env, 0)
    except Exception as e:
        compile_error(env.fname, env.src, env.token, e)

def mp2c(fname, src, option):
    tree     = parse(src)
    words    = fname.split(".")
    mod_name = words[0]
    env      = Env(fname, option)
    env.src  = src
    env.origin_lines = src.split("\n")

    init_main        = AstNode("=")
    init_main.first  = Token("name", "__name__")
    init_main.second = Token("string", "__main__")
    init__name__     = do_item(init_main, env)

    env.find_all_func_def(tree)

    lines = do_program(tree, env, 0)
    lines = [init__name__] + lines

    generator = Generator(env)
    generator.process(lines)
    return generator.get_code()

def print_help():
    print("Usage")
    print("   python mp2c.py [options] filename")
    print()
    print("1/0 options")
    print("  -h(elp)      : print help information")
    print("  -debug       : add debug information")
    print("  -record_line : record line information")
    print("  -is_module   : compile to c module without main entry")
    print("  -readable    : generate more reabale c file")
    print()
    print("K/V options")
    print("  -prefix      : set c source prefix")

class CmdOption:
    pass

def get_opt(options):
    if len(options) == 0:
        print_help()
        exit(1);
    opt             = CmdOption()
    opt.enable_gc   = False
    opt.debug       = False
    opt.record_line = False
    opt.prefix      = None
    opt.multi_line  = False ## generate list/dict of multi-line style
    opt.is_module   = False ## if this is true, will not generate main function
    opt.readable    = False ## generate readable C file

    if len(options) == 1 and options[0] in ["-help", "--help", "-h", "--h"]:
        print_help();
        sys.exit(0);
    for arg in options:
        if arg.startswith("-prefix="):
            opt.prefix = arg.split("=")[1]
        elif arg[0] == '-':
            key = arg[1:]
            setattr(opt, key, True)
        else:
            opt.name = arg
            break
    return opt
    
if __name__ == "__main__":
    opt    = get_opt(sys.argv[1:])
    name   = opt.name
    path   = name
    text   = load(path)
    pathes = path.split('/')

    if len(pathes) > 1:
        name = pathes[-1]

    mod  = name.split(".")[0]
    code = mp2c(name, text, opt)

    target_file = "build/" + mod + ".c"
    save(target_file, code)
    print("saved to", target_file)
    # save("bin/" + mod + ".c", code)

