# -*- coding:utf-8 -*-
# @author xupingmao
# @since 2016
# @modified 2022/06/05 16:58:37

"""使用说明
dis_code: 反编译代码为字节码
compile:  将Python代码编译成C语言字符串
"""

if "tm" not in globals():
    from boot import *

from mp_parse import *
from mp_opcode import *

_asm_ctx = None
_code_list = None
_ext_code_list = None # extra _code_list for optimize.

_jmp_list = [
    OP_JUMP_ON_FALSE, 
    OP_JUMP_ON_TRUE, 
    OP_POP_JUMP_ON_FALSE, 
    OP_SETJUMP, 
    OP_JUMP,
    OP_NEXT
]


_op_dict = {
    '+':OP_ADD,
    '-':OP_SUB,
    '*':OP_MUL,
    '/':OP_DIV,
    '%':OP_MOD,
    '>' : OP_GT,
    '<' : OP_LT,
    '>=': OP_GTEQ,
    '<=': OP_LTEQ,
    '==': OP_EQEQ,
    'is': OP_EQEQ,
    '!=': OP_NOTEQ,
    'get':OP_GET
    #'notin' : OP_NOTIN,
}
_op_ext_dict = {
    '+=' : OP_ADD,
    '-=' : OP_SUB,
    '*=' : OP_MUL,
    '/=' : OP_DIV,
    '%=' : OP_MOD
}

_begin_tag_list = [-1]
_end_tag_list = [-1]

_tag_cnt = 0
_global_index = 0


def init_pop_value_type_set():
    result = set(["call"])
    for item in _op_dict:
        result.add(item)
    return result

POP_VALUE_TYPE_SET = init_pop_value_type_set()

class Scope:
    def __init__(self):
        self.locals = []
        self.globals = []
        self.temp_vars = []
        self.jmps = 0 # try block count.
        
    def add_global(self, v):
        if v not in self.globals:
            self.globals.append(v)

    def get_new_temp(self):
        temp_size = len(self.temp_vars)
        temp_name = "%" + str(temp_size)
        self.temp_vars.append(temp_name)
        return None
            
class AsmContext:
    def __init__(self):
        self.scope = Scope()
        self.scopes = [self.scope]
        
    def push(self):
        self.scope = Scope()
        self.scopes.append(self.scope)
        
    def pop(self):
        self.scopes.pop()
        
    def add_local(self, v):
        if v.val not in self.scope.locals:
            self.scope.locals.append(v.val)

    def load(self, v):
        # same as store, check scope level first
        if len(self.scopes) == 1:
            emit(OP_LOAD_GLOBAL, v.val)
        # check locals
        elif v.val not in self.scope.locals:
            emit(OP_LOAD_GLOBAL, v.val)
        else:
            idx = self.scope.locals.index(v.val)
            emit(OP_LOAD_LOCAL, idx)
            
    def index_local(self, v):
        if v.val not in self.scope.locals:
            self.scope.locals.append(v.val)
        return self.scope.locals.index(v.val)
        
    def store(self, v):
        # first ,check scope level
        if len(self.scopes) == 1:
            emit(OP_STORE_GLOBAL, v.val)
        # check if in globals defined in the function, 
        # or store as local
        elif v.val not in self.scope.globals:
            idx = self.index_local(v)
            emit(OP_STORE_LOCAL, idx)
        else:
            emit(OP_STORE_GLOBAL, v.val)

def asm_init():
    global _asm_ctx
    global _code_list
    global _ext_code_list

    _asm_ctx = AsmContext()
    _code_list = []
    _ext_code_list = []

def chk_try_block(tag):
    if _asm_ctx.scope.jmps > 0:
        return 0
    _asm_ctx.scope.jmps += 1
    emit(OP_SETJUMP, tag)
    return 1

def exit_try_block():
    _asm_ctx.scope.jmps -= 1
    
def asm_switch__code_list():
    global _code_list
    global _ext_code_list
    _code_list, _ext_code_list = _ext_code_list, _code_list

def asm_get_regs():
    return len(_asm_ctx.scope.locals)

def store_global(glo):
    emit(OP_STORE_GLOBAL, glo.val)

def add_global(v):
    _asm_ctx.scope.globals.append(v.val)
# opcode : op

def emit(op, val = 0):
    ins = [op, val]
    _code_list.append(ins)
    return ins

def code_pop():
    return _code_list.pop()
    
def emit_def(v):
    emit(OP_DEF, v.val)

# inside function, assignment will be made to locals by default,
# except that a global is declared. so we must save the declared
# globals to a local scope.
# 
def emit_load(v):
    if v == None:
        emit(OP_NONE)
        return;
    t = v.type
    if t == 'string':
        emit(OP_STRING, v.val)
    elif t == 'number':
        emit(OP_NUMBER, v.val)
    elif t == 'None':
        emit(OP_NONE, 0)
    elif t == 'name':
        _asm_ctx.load(v)
    else:
        print('LOAD_LOCAL ' + str(v.val))
        

def find_label(code, val):
    cur = 0
    for ins in code:
        if ins[0] == OP_TAG and ins[1] == val:
            # this `return will disturb the normal `for loop
            return cur
        if ins[0] != OP_TAG:
            cur+=1

def resolve_labels(code):
    code_size = len(code)
    cur = 0
    new_code = []
    for ins in code:
        if ins[0] in _jmp_list:
            pos = find_label(code, ins[1])
            gap = pos - cur
            if gap < 0:ins = [OP_UP_JUMP, -gap]
            else:ins[1] = gap
        if ins[0] != OP_TAG:
            cur+=1
            new_code.append(ins)
    return new_code
    
def optimize(x, optimize_jmp = False):
    last = 0
    tag = 0
    nx = resolve_labels(x)
    return nx
    
def join_code():
    global _code_list
    global _ext_code_list
    return _ext_code_list + _code_list
    
def _gen_code(lst = False):
    global _code_list
    global _ext_code_list

    emit(OP_EOP)
    #x = gen_constants(tagsize) + _code_list
    x = _ext_code_list + _code_list
    # release memory
    _code_list = []
    _ext_code_list = []
    x = optimize(x)
    if lst:return x
    for i in x:
        if i[1] == None:
            print(i)
    return save_as_bin(x)

def gen_code(lst = False):
    global _code_list
    global _ext_code_list

    emit(OP_EOP)
    code = _ext_code_list + _code_list
    # release memory
    _code_list = []
    _ext_code_list = []
    code = optimize(code)
    return code


def def_local(v):
    _asm_ctx.add_local(v)

def get_loc_num():
    return len(_asm_ctx.scope.locals)

def push_scope():
    _asm_ctx.push()

def pop_scope():
    _asm_ctx.pop()

def emit_store(v):
    _asm_ctx.store(v)


def encode_error(token, msg):
    global _ctx
    compile_error("encode", _ctx.src, token, msg)
    
def load_attr(name):
    if name.type == 'name':
        name.type = 'string'
    emit_load(name)
    
def store(t):
    if t.type == 'name':
        emit_store(t)
    elif t.type == 'get':
        encode_item(t.first)
        encode_item(t.second)
        emit(OP_SET)
    elif t.type == 'attr':
        encode_item(t.first)
        load_attr(t.second)
        emit(OP_SET)
    elif t.type == ',':
        # remains unsure.
        store(t.first)
        store(t.second)

def newglo():
    global _global_index
    _global_index += 1
    return Token("name", "#" + str(_global_index-1))

def newtag():
    global _tag_cnt
    _tag_cnt+=1
    return _tag_cnt-1

def jump(p, ins = OP_JUMP):
    emit(ins, p)

def emit_tag(p):
    emit(OP_TAG, p)
    

def build_set(self, key, val):
    node = AstNode('=')
    node.first = val
    set_node = AstNode('get')
    set_node.first = key
    set_node.second = val
    node.first = set_node
    return node

def encode_op(tk):
    encode_item(tk.first)
    encode_item(tk.second)
    emit(_op_dict[tk.type])

def encode_notin(tk):
    encode_in(tk)
    emit(OP_NOT)

def encode_isnot(tk):
    encode_item(tk.first)
    encode_item(tk.second)
    emit(OP_NOTEQ)

def encode_inplace_op(tk):
    encode_item(tk.first)
    encode_item(tk.second)
    emit(_op_ext_dict[tk.type])
    store(tk.first)
    
# [1, 2, 3] 
# will be parsed like.
#     ^
#    ^  3
#  1  2
def encode_list0(v):
    if v == None: return
    newlist = []
    while v.type == ',':
        newlist.append(v.second)
        v = v.first
    newlist.append(v)
    newlist.reverse()
    for item in newlist:
        encode_item(item)
        emit(OP_APPEND)


def encode_list(v):
    emit(OP_LIST, 0)
    # encode_list0(v.first)
    if v.first == None:
        return 0
    if gettype(v.first) == "list":
        for item in v.first:
            encode_item(item)
            emit(OP_APPEND)
    else:
        encode_item(v.first)
        emit(OP_APPEND)
    return 1

def encode_tuple(tk):
    # print("encode_tuple", getlineno(tk))
    if gettype(tk.first) == "list" and is_const_list(tk.first):
        g = get_const_list(tk.first)
        emit_load(g)
        return 1
    return encode_list(tk)

def encode_comma(tk):
    return encode_item(tk.first) + encode_item(tk.second)

def is_const_list(list):
    for item in list:
        if not hasattr(item, "type"):
            return False
        if item.type not in ("string", "number", "None"):
            return False
    return True

def get_const_list(list):
    asm_switch__code_list()
    g = newglo()
    emit(OP_LIST)
    for item in list:
        encode_item(item)
        emit(OP_APPEND)
    store_global(g)
    asm_switch__code_list()
    return g
    
    
def encode_if(tk):
    # refuse assignment in if condition
    if tk.first.type == '=':
        encode_error(tk.first, 'do not allow assignment in if condition')
    encode_item(tk.first)
    else_tag,end_tag = newtag(), newtag()
    jump(else_tag, OP_POP_JUMP_ON_FALSE)
    # optimize for `if x in const_list`
    encode_item(tk.second)  # TRUE的逻辑
    jump(end_tag)
    emit_tag(else_tag)
    encode_item(tk.third)   # ELSE的逻辑
    emit_tag(end_tag)
    
def encode_assign_to(item_list, n):
    if istype(item_list, "list"):
        if len(item_list) == 1:
            # do nothing, just one object to assign
            pass
        elif n == 1:
            emit(OP_UNPACK, len(item_list))
        else:
            emit(OP_ROT, n)
        for item in item_list:
            store(item)
    else:
        store(item_list)
    
def encode_assign(tk):
    if gettype(tk.second) == "list":
        for item in tk.second:
            encode_item(item)
        n = len(tk.second)
    else:
        encode_item(tk.second)
        n = 1
    encode_assign_to(tk.first, n)
    
def encode_dict(tk):
    items = tk.first
    emit(OP_DICT, 0)
    if items != None:
        for item in items:
            encode_item(item[0])
            encode_item(item[1])
            emit(OP_DICT_SET)
    
def encode_neg(tk):
    if tk.first.type == 'number':
        tk = tk.first
        tk.val = -tk.val
        encode_item(tk)
    else:
        encode_item(tk.first)
        emit(OP_NEG)
        
def encode_not(tk):
    encode_item(tk.first)
    emit(OP_NOT)
    
    
def encode_call(tk):
    encode_item(tk.first)
    if gettype(tk.second) == "list":
        for item in tk.second:
            encode_item(item)
        n = len(tk.second)
    else:
        n = encode_item(tk.second)
    emit(OP_CALL, n)
    
def encode_apply(tk):
    encode_item(tk.first)
    encode_item(tk.second)
    emit(OP_APPLY)

    
def encode_def(tk, in_class = 0):
    emit_def(tk.first)
    push_scope()
    narg = 0
    parg = 0
    varg = 0
    narg_index = 0
    for item in tk.second:
        def_local(item.first)
        # regs.append(item.first)
        if item.type == 'narg':
            narg = 1
            # emit(MP_NARG, index)
            break
        if item.second:
            varg += 1
            encode_item(item.second)
            store(item.first)
        else:
            parg += 1
        narg_index += 1
    line_no = getlineno(tk.first)
    if line_no != None:
        emit(OP_LINE, line_no)
    if not narg:
        emit(OP_LOAD_PARAMS, parg*256 + varg)
    elif parg > 0:
        emit(OP_LOAD_PARG, parg)
    if narg:
        emit(OP_LOAD_NARG, narg_index)
    encode_item(tk.third)
    emit(OP_DEF_END)
    #regs[1] = asm_get_regs()
    #emit_tag(func_end)
    #loc_num_ins[1] = get_loc_num()
    pop_scope()
    if not in_class:
        emit_store(tk.first)
 

def encode_class(tk):
    buildclass = []
    emit(OP_CLASS, tk.first.val)
    for func in tk.second:
        if func.type == "pass": continue
        if func.type != "def":
            encode_error(func, "non-func expression in class is invalid")
        encode_def(func, 1)
        emit_load(tk.first)
        load_attr(func.first)
        emit(OP_SET)
    emit(OP_CLASS_END)

def encode_return(tk):
    if tk.first:
        if gettype(tk.first) == "list":
            # multi return
            emit(OP_LIST)
            for item in tk.first:
                encode_item(item)
                emit(OP_APPEND)
        else:
            encode_item(tk.first)
            if tk.first.type == "call":
                op, value = code_pop()
                emit(OP_TAILCALL, value)
    else:
        # is this necessary ?
        emit_load(None);
    emit(OP_RETURN)

def encode_while(tk):
    start_tag, end_tag = newtag(), newtag()
    # set while stack
    _begin_tag_list.append(start_tag)
    _end_tag_list.append(end_tag)
    ###
    emit_tag(start_tag)
    encode_item(tk.first)
    jump(end_tag, OP_POP_JUMP_ON_FALSE)
    encode_item(tk.second)
    #emit(OP_UP_JUMP, start_tag)
    jump(start_tag)
    emit_tag(end_tag)
    # clear while stack
    _begin_tag_list.pop()
    _end_tag_list.pop()
    
def encode_continue(tk):
    #emit( OP_UP_JUMP, _begin_tag_list[-1] )
    jump(_begin_tag_list[-1])

    
def encode_break(tk):
    jump(_end_tag_list[-1])

def encode_import_one(mod, item):
    # encode_item(Token("name", "_import"))
    encode_item(mod)
    item.type = 'string'
    encode_item(item)
    emit(OP_IMPORT, 2)

def _import_name2str(mod):
    if mod.type == 'attr':
        l = _import_name2str(mod.first)
        r = _import_name2str(mod.second)
        return Token('string', l.val + '/' + r.val)
    elif mod.type == 'name':
        mod.type = 'string'
        return mod
    elif mod.type == 'string':
        return mod
    
def encode_import_multi(mod, items):
    mod = _import_name2str(mod)
    if items.type == ',':
        encode_import_multi(mod, items.first)
        encode_import_multi(mod, items.second)
    else:
        encode_import_one(mod, items)

def encode_from(tk):
    encode_import_multi(tk.first, tk.second)

def _encode_import(item):
    item.type = 'string'
    encode_item(item)
    emit(OP_IMPORT, 1)

def encode_import(tk):
    if tk.first.type == ',':
        tk = tk.first
        _encode_import(tk.first)
        _encode_import(tk.second)
    else:
        _encode_import(tk.first)


def encode_and(tk):
    end = newtag()
    encode_item(tk.first)
    emit(OP_JUMP_ON_FALSE, end)
    encode_item(tk.second)
    emit(OP_AND)
    emit_tag(end)


def encode_or(tk):
    end = newtag()
    encode_item(tk.first)
    emit(OP_JUMP_ON_TRUE, end)
    encode_item(tk.second)
    emit(OP_OR)
    emit_tag(end)
    
def encode_raw_list(list):
    if gettype(list) == "list":
        emit(OP_LIST)
        for item in list:
            encode_item(item)
            emit(OP_APPEND)
        return len(list)
    else:
        encode_item(list)
        return 1

def encode_for(tk):
    start_tag = newtag()
    end_tag = newtag()
    # set for stack
    _begin_tag_list.append(start_tag)
    _end_tag_list.append(end_tag)

    iterator = tk.first.second
    values   = tk.first.first

    ### load index and iterator
    encode_raw_list(iterator)
    emit(OP_ITER) # create a iterator
    emit_tag(start_tag)
    jump(end_tag, OP_NEXT)
    encode_assign_to(values, 1)

    # for-loop body
    encode_item(tk.second)
    jump(start_tag)
    emit_tag(end_tag)
    # clear for stack
    _begin_tag_list.pop()
    _end_tag_list.pop()
    emit(OP_POP) # pop iterator.
    
def encode_global(tk):
    add_global(tk.first)
    
def encode_try(tk):
    exception = newtag()
    end = newtag()
    if not chk_try_block(exception):
        encode_error(tk, "do not support recursive try")
    encode_item(tk.first)
    emit(OP_CLR_JUMP)
    jump(end)
    emit_tag(exception)
    if tk.second != None:
        emit(OP_LOAD_EX)
        store(tk.second)
    else:
        emit(OP_POP)
    encode_item(tk.third)
    emit_tag(end)
    exit_try_block()
    
def do_nothing(tk):
    pass

def encode_del(tk):
    item = tk.first
    if item.type != 'get' and item.type != 'attr':
        encode_error(item, 'require get or attr expression')
    encode_item(item.first)
    if item.type == 'attr':
        load_attr(item.second)
    else:
        encode_item(item.second)
    emit(OP_DEL)
    
def encode_annotation(tk):
    token = tk.first
    if token.val == "debugger":
        emit(OP_DEBUG)
    
def encode_attr(tk):
    tk.second.type = 'string'
    encode_item(tk.first)
    encode_item(tk.second)
    emit(OP_GET)
    
def encode_slice(tk):
    encode_item(tk.first)
    encode_item(tk.second)
    encode_item(tk.third)
    emit(OP_SLICE)
    
def encode_in(tk):
    encode_item(tk.first)
    encode_item(tk.second)
    emit(OP_IN)


_encode_dict = {
    'if': encode_if,
    '=': encode_assign,
    'tuple': encode_tuple,
    ',': encode_comma,
    'dict': encode_dict,
    'call': encode_call,
    'apply': encode_apply,
    'neg': encode_neg,
    'not': encode_not,
    'list':encode_list,
    'def':encode_def,
    'del':encode_del,
    'class':encode_class,
    'return':encode_return,
    'while':encode_while,
    'continue': encode_continue,
    'break':encode_break,
    'from':encode_from,
    'import':encode_import,
    'and':encode_and,
    'or':encode_or,
    'for':encode_for,
    'global':encode_global,
    'name':emit_load,
    'number':emit_load,
    'string':emit_load,
    'None':emit_load,
    'True':emit_load,
    'False':emit_load,
    'try':encode_try,
    'pass':do_nothing,
    'notin':encode_notin,
    'isnot':encode_isnot,
    'attr':encode_attr,
    'slice': encode_slice,
    'in': encode_in,
    '@':encode_annotation,
}

for k in _op_dict:
    _encode_dict[k] = encode_op
for k in _op_ext_dict:
    _encode_dict[k] = encode_inplace_op

def getlineno(tk):
    if hasattr(tk, 'pos'):
        return tk.pos[0]
    elif hasattr(tk, 'first'):
        return getlineno(tk.first)
    return None
    
def encode_item(tk):
    """encode ast item.
    @param tk ast token
    @return opstack height
    """
    if tk == None:
        return 0
    # encode for statement list.
    if gettype(tk) == 'list':
        return encode_block(tk)

    r = _encode_dict[tk.type](tk)
    if r != None:
        return r
    return 1

def need_pop_value(type):
    return type in POP_VALUE_TYPE_SET

def encode_block(tk):
    assert gettype(tk) == "list"
    for i in tk:
        # comment 
        if i.type == 'string':
            continue
        lineno = getlineno(i)
        if lineno != None: 
            emit(OP_LINE, lineno)
        encode_item(i)
        if need_pop_value(i.type):
            emit(OP_POP)

def encode(content):
    global _tag_cnt
    global _bin 
    global _bin_ext
    _tag_cnt = 0
    _global_index = 0
    r = parse(content)
    encode_item(r)

class EncodeCtx:
    """编码器上下文
    TODO: 改造成完整的编码器
    """
    def __init__(self, src_code):
        self.src = src_code
        self.code_list = []

    def set_file_name(self, filename):
        name = filename.split(".")[0]
        emit(OP_FILE, name)

    def compile(self):
        self.ast = encode(self.src)
        return self.ast

    def gen_code(self):
        code = join_code()
        return code

def _compile(src, filename, des = None):
    global _ctx
    # lock here
    asm_init()
    _ctx = EncodeCtx(src)
    _ctx.set_file_name(name)
    _ctx.compie()
    return _ctx.gen_code()

def escape_for_compile(s):
    s = str(s)
    s = s.replace("\\", "\\\\")
    s = s.replace("\r", "\\r")
    s = s.replace("\n", "\\n")
    s = s.replace("\0", "\\0")
    return s
    
def compile_to_list(src, filename):
    global _ctx
    # lock here
    asm_init()
    _ctx = EncodeCtx(src)
    # name = filename.split(".")[0]
    emit(OP_FILE, filename)
    encode(src)
    _ctx = None
    code = gen_code()
    return code

def compile(src, filename, des = None):
    global _ctx
    # lock here
    asm_init()
    _ctx = EncodeCtx(src)
    name = filename.split(".")[0]
    emit(OP_FILE, name)
    encode(src)
    _ctx = None
    code = gen_code()
    dest = ''
    for item in code:
        # there is no # in CJK charsets, so it is better to split the sequence
        if item[1] == 0:
            dest += str(item[0]) + '\n'
        else:
            dest += str(item[0]) + '#' + escape_for_compile(item[1])+'\n'
    return dest;
  
def convert_to_cstring(filename, code, const_name=None):
    code = code.replace("\\", "\\\\")
    code = code.replace('"', '\\"')
    code = code.replace("\n", "\\n")
    code = code.replace("\0", "\\0")

    if const_name == None:
        const_name = filename.split(".")[0] + "_bin"

    cstring = "const char* " + const_name + "=";
    cstring += '"'
    cstring += code
    cstring += '";'
    return cstring
    
    
def _compilefile(filename, des = None):
    return _compile(load(filename), filename, des)
    
def compilefile(filename, des = None):
    return compile(load(filename), filename, des)

def split_instr(instr):
    size = len(instr)
    list = []
    i = 0
    while i < size:
        op = instr[i]
        v = uncode16(instr[i+1], instr[i+2])
        i+=3
        list.append([ord(op), v])
    return list

def to_fixed(num, length):
    return str(num).rjust(length).replace(' ', '0')

def dis_code(code, return_str = False, fname = "<string>"):
    if return_str == True:
        result = []

    ins_list = compile_to_list(code, fname)
    for index, item in enumerate(ins_list):
        op = int(item[0])
        line = "%s %s %r" % (to_fixed(index+1,4), opcodes[op].ljust(22), item[1])
        # line = to_fixed(index+1, 4) + ' ' + opcodes[op].ljust(22) + str(item[1])

        if return_str:
            result.append(line)
        else:
            print(line)

    if return_str:
        return "\n".join(result)

def dis(path, return_str = False):
    code = load(path)
    return dis_code(code, return_str, path)

# MP_TEST
def main():
    import sys
    ARGV = sys.argv

    if len(ARGV) < 2:
        print("usage: %s filename    : compile python to c code" % ARGV[0])
        print("       %s -p filename : print code" % ARGV[0])
    elif len(ARGV) == 2:
        # execute python file
        import repl
        import mp_opcode
        opcodes = mp_opcode.opcodes
        code = compilefile(ARGV[1])
        code = convert_to_cstring(ARGV[1], code)
        print(code)
    elif len(ARGV) >= 3:
        args = dict()
        parser = ArgReader(ARGV[1:])
        while parser.has_next():
            item = parser.next()
            if item == "-p":
                assert parser.has_next(), "expect filename"
                filename = parser.next()
                code = compilefile(filename)
                print(code)
                return

            if item == "-dis":
                assert parser.has_next(), "expect filename"
                filename = parser.next()
                dis(filename)
                return

            if item == "-const_name":
                assert parser.has_next()
                args["const_name"] = parser.next()
                continue

            if item == "-c_code":
                args["c_code"] = True
                continue

            if "c_code" in args:
                filename = item
                const_name_default = filename.split(".")[0] + "_bin"
                const_name = args.get("const_name", const_name_default)

                code = compilefile(filename)
                code = convert_to_cstring(filename, code, const_name)
                print(code)
            else:
                compile(item, "#test")

if __name__ == "__main__":
    main()

# MP_TEST_END

