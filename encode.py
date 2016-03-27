from boot import *
from parse import *
from tmcode import *


def load_const(const):
    emit(LOAD_CONSTANT, getConstIdx(const.val))

class Scope:
    def __init__(self):
        self.locals = []
        self.globals = []
        self.jmps = 0 # try block count.
        
    def def_global(self, v):
        if v not in self.globals:
            self.globals.append(v)
            
class Names:
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
            idx = getConstIdx(v.val)
            emit(LOAD_GLOBAL, idx)
        # check locals
        elif v.val not in self.scope.locals:
            idx = getConstIdx(v.val)
            emit(LOAD_GLOBAL, idx)
        else:
            idx = self.scope.locals.index(v.val)
            emit(LOAD_LOCAL, idx)
            
    def indexlocal(self, v):
        if v.val not in self.scope.locals:
            self.scope.locals.append(v.val)
        return self.scope.locals.index(v.val)
        
    def store(self, v):
        # first ,check scope level
        if len(self.scopes) == 1:
            idx = getConstIdx(v.val)
            emit(STORE_GLOBAL, idx)
        # check if in globals defined in the function, 
        # or store as local
        elif v.val not in self.scope.globals:
            idx = self.indexlocal(v)
            emit(STORE_LOCAL, idx)
        else:
            idx = getConstIdx(v.val)
            emit(STORE_GLOBAL, idx)

names = None
out = None
out_ext = None # extra out for optimize.
def asm_init():
    global names
    global out
    global out_ext

    names = Names()
    out = []
    out_ext = []

def asm_try(tag):
    if names.scope.jmps > 0:
        return 0
    names.scope.jmps += 1
    emit(SETJUMP, tag)
    return 1

def asm_try_exit():
    names.scope.jmps -= 1
    
def asm_switch_out():
    global out
    global out_ext
    out, out_ext = out_ext, out

def asm_get_regs():
    return len(names.scope.locals)

def store_glo(glo):
    idx = getConstIdx(glo.val)
    emit(STORE_GLOBAL, idx)

def def_global(v):
    names.scope.globals.append(v.val)
# opcode : op

def emit(op, val = 0):
    ins = [op, val]
    out.append(ins)
    return ins
    
def emit_def(v):
    idx = getConstIdx(v.val)
    emit(TM_DEF, idx)

# inside function, assignment will be made to locals by default,
# except that a global is declared. so we must save the declared
# globals to a local scope.
# 
def emit_load(v):
    if v == None:
        emit(LOAD_NONE)
        return;
    t = v.type
    if t == 'string' or t == 'number':
        load_const(v)
    elif t == 'None':
        emit(LOAD_NONE, 0)
    elif t == 'name':
        names.load(v)
    else:
        print('LOAD_LOCAL ' + str(v.val))

jmp_list = [
    JUMP_ON_FALSE, 
    JUMP_ON_TRUE, 
    POP_JUMP_ON_FALSE, 
    SETJUMP, 
    JUMP,
    TM_NEXT
]


#jmp_optimize_map = {
#    OP_LT: LT_JUMP_ON_FALSE,
#    GT: GT_JUMP_ON_FALSE,
#    LTEQ: LTEQ_JUMP_ON_FALSE,
#    GTEQ: GTEQ_JUMP_ON_FALSE,
#    EQEQ: EQEQ_JUMP_ON_FALSE,
#    NOTEQ: NOTEQ_JUMP_ON_FALSE
#}
        
def asm_save_bin(lst):
    bin = ''
    # print lst
    for _ins in lst:
        ins = _ins[0]
        val = _ins[1]
        if ins == NEW_NUMBER or ins == NEW_STRING:
            bin += code8(ins) + code16(len(val)) + val
        else:
            bin += code8(ins) + code16(val)
    return bin
    
def optimize0(nx, x,tags, jmp_list):
    for ins in x:
        if ins[0] in jmp_list:
            if ins[1] in tags:
                ins[1] = tags[ins[1]]
            nx.append(ins)
        elif ins[1] == None:
            pass
        else:
            nx.append(ins)

def findTag(code, val):
    cur = 0
    for ins in code:
        if ins[0] == TAG and ins[1] == val:
            # this `return will disturb the normal `for loop
            return cur
        if ins[0] != TAG:
            cur+=1

def assembleJmps(code):
    codeSize = len(code)
    cur = 0
    newCode = []
    for ins in code:
        if ins[0] in jmp_list:
            pos = findTag(code, ins[1])
            gap = pos - cur
            if gap < 0:ins = [UP_JUMP, -gap]
            else:ins[1] = gap
        if ins[0] != TAG:
            cur+=1
            newCode.append(ins)
    return newCode
    
def optimize(x, optimize_jmp = False):
    last = 0
    tag = 0
    nx = assembleJmps(x)
    return nx
    
    
def gen_code(lst = False):
    global out
    global out_ext

    emit(TM_EOP)
    #x = gen_constants(tagsize) + out
    x = out_ext + out
    # release memory
    out = []
    out_ext = []
    x = optimize(x)
    if lst:return x
    for i in x:
        if i[1] == None:
            print(i)
    return asm_save_bin(x)


def def_local(v):
    names.add_local(v)

def get_loc_num():
    return len(names.scope.locals)

def push_scope():
    names.push()

def pop_scope():
    names.pop()

def emit_store(v):
    names.store(v)


def encode_error(token, msg):
    global ctx
    compile_error("encode", ctx.src, token, msg)
    
def loadAttr(name):
    if name.type == 'name':
        name.type = 'string'
    emit_load(name)
    
def store(t):
    if t.type == 'name':
        emit_store(t)
    elif t.type == 'get':
        encodeItem(t.first)
        encodeItem(t.second)
        emit(SET)
    elif t.type == 'attr':
        encodeItem(t.first)
        loadAttr(t.second)
        emit(SET)
    elif t.type == ',':
        # remains unsure.
        store(t.first)
        store(t.second)

_tag_cnt = 0
_glo_idx = 0

def newglo():
    global _glo_idx
    _glo_idx += 1
    return Token("name", "#" + str(_glo_idx-1))

def newtag():
    global _tag_cnt
    _tag_cnt+=1
    return _tag_cnt-1

def jump(p, ins = JUMP):
    emit(ins, p)

def emit_tag(p):
    emit(TAG, p)
    

op_map = {
    '+':ADD,
    '-':SUB,
    '*':MUL,
    '/':DIV,
    '%':MOD,
    '>' : OP_GT,
    '<' : OP_LT,
    '>=': GTEQ,
    '<=': LTEQ,
    '==': EQEQ,
    '!=': NOTEQ,
    'get':GET
    #'notin' : NOTIN,
}
op_ext_map = {
    '+=' : ADD,
    '-=' : SUB,
    '*=' : MUL,
    '/=' : DIV,
    '%=' : MOD
}

begin_tag_list = [-1]
end_tag_list = [-1]

def build_set(self, key, val):
    node = AstNode('=')
    node.first = val
    setNode = AstNode('get')
    setNode.first = key
    setNode.second = val
    node.first = setNode
    return node

def encode_op(tk):
    encodeItem(tk.first)
    encodeItem(tk.second)
    emit(op_map[tk.type])

def encode_notin(tk):
    encode_in(tk)
    emit(NOT)

def encode_op_ext(tk):
    encodeItem(tk.first)
    encodeItem(tk.second)
    emit(op_ext_map[tk.type] )
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
        encodeItem(item)
        emit(LIST_APPEND)

        
def encode_list(v):
    emit(LIST, 0)
    # encode_list0(v.first)
    if v.first == None:
        return 0
    if gettype(v.first) == "list":
        for item in v.first:
            encodeItem(item)
            emit(LIST_APPEND)
    else:
        encodeItem(v.first)
        emit(LIST_APPEND)
    return 1

    #emit(LIST, tk_list_len(v.first))

def isConstList(list):
    for item in list:
        if not hasattr(item, "type"):
            return False
        if item.type not in ("string", "number", "None"):
            return False
    return True

def getConstList(list):
    asm_switch_out()
    g = newglo()
    emit(LIST)
    for item in list:
        encodeItem(item)
        emit(LIST_APPEND)
    store_glo(g)
    asm_switch_out()
    return g
    
    
def encode_if(tk):
    # refuse assignment in if condition
    if tk.first.type == '=':
        encode_error(tk.first, 'do not allow assignment in if condition')
    encodeItem(tk.first)
    else_tag,end_tag = newtag(), newtag()
    jump(else_tag, POP_JUMP_ON_FALSE)
    # optimize for `if x in const_list`
    encodeItem(tk.second)
    jump(end_tag)
    emit_tag(else_tag)
    encodeItem(tk.third)
    emit_tag(end_tag)
    
    
def encode_assign(tk):
    if gettype(tk.second) == "list":
        for item in tk.second:
            encodeItem(item)
        n = len(tk.second)
    else:
        encodeItem(tk.second)
        n = 1
    if gettype(tk.first) == "list":
        if n == 1:
            emit(TM_UNARRAY, len(tk.first))
        else:
            emit(TM_ROT, n)
        for item in tk.first:
            store(item)
    else:
        store(tk.first)
    
def encode_tuple(tk):
    return encodeItem(tk.first) + encodeItem(tk.second)
    
def encode_dict(tk):
    items = tk.first
    emit(DICT, 0)
    if items != None:
        for item in items:
            encodeItem(item[0])
            encodeItem(item[1])
            emit(DICT_SET)
    
def encode_neg(tk):
    if tk.first.type == 'number':
        tk = tk.first
        tk.val = -tk.val
        encodeItem(tk)
    else:
        encodeItem(tk.first)
        emit(NEG)
        
def encode_not(tk):
    encodeItem(tk.first)
    emit(NOT)
    
    
def encode_call(tk):
    encodeItem(tk.first)
    if gettype(tk.second) == "list":
        for item in tk.second:
            encodeItem(item)
        n = len(tk.second)
    else:
        n = encodeItem(tk.second)
    emit(CALL, n)

    
def encode_def(tk, in_class = 0):
    emit_def(tk.first)
    # emit(TM_DEF, 0)
    # regs = []
    # emit(0, 0) # filename
    #regs = emit(0, 0) # regs
    #func_end  = newtag()
    #jump(func_end)
    # emit(0, regs) # regs
    # loc_num_ins = emit(LOC_NUM, 0)
    push_scope()
    narg = 0
    parg = 0
    varg = 0
    for item in tk.second:
        def_local(item.first)
        # regs.append(item.first)
        if item.type == 'narg':
            narg = 1
            emit(TM_NARG)
            break
        if item.second:
            varg += 1
            encodeItem(item.second)
            store(item.first)
        else:
            parg += 1
    if not narg:
        emit(LOAD_PARAMS, parg*256 + varg)
    encodeItem(tk.third)
    emit(TM_EOF)
    #regs[1] = asm_get_regs()
    #emit_tag(func_end)
    #loc_num_ins[1] = get_loc_num()
    pop_scope()
    if not in_class:
        emit_store(tk.first)
 

def encode_class(tk):
    buildclass = []
    emit(DICT, 0)
    store(tk.first)
    for func in tk.second:
        if func.type == "pass": continue
        if func.type != "def":
            encode_error(func, "non-func expression in class is invalid")
        encode_def(func, 1)
        emit_load(tk.first)
        loadAttr(func.first)
        emit(SET)

def encode_return(tk):
    if tk.first:
        if gettype(tk.first) == "list":
            emit(LIST)
            for item in tk.first:
                encodeItem(item)
                emit(LIST_APPEND)
        else:
            encodeItem(tk.first)
    else:
        # is this necessary ?
        emit_load(None);
    emit(RETURN)

def encode_while(tk):
    start_tag, end_tag = newtag(), newtag()
    # set while stack
    begin_tag_list.append(start_tag)
    end_tag_list.append(end_tag)
    ###
    emit_tag(start_tag)
    encodeItem(tk.first)
    jump(end_tag, POP_JUMP_ON_FALSE)
    encodeItem(tk.second)
    #emit(UP_JUMP, start_tag)
    jump(start_tag)
    emit_tag(end_tag)
    # clear while stack
    begin_tag_list.pop()
    end_tag_list.pop()
    
def encode_continue(tk):
    #emit( UP_JUMP, begin_tag_list[-1] )
    jump(begin_tag_list[-1])

    
def encode_break(tk):
    jump(end_tag_list[-1])

def encode_import_one(mod, item):
    # encodeItem(Token("name", "_import"))
    encodeItem(mod)
    item.type = 'string'
    encodeItem(item)
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
    
def encode_import_many(mod, items):
    mod = _import_name2str(mod)
    if items.type == ',':
        encode_import_many(mod, items.first)
        encode_import_many(mod, items.second)
    else:
        encode_import_one(mod, items)

def encode_from(tk):
    encode_import_many(tk.first, tk.second)

def _encode_import(item):
    item.type = 'string'
    encodeItem(item)
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
    encodeItem(tk.first)
    emit(JUMP_ON_FALSE, end)
    encodeItem(tk.second)
    emit(AND)
    emit_tag(end)
    

def encode_or(tk):
    end = newtag()
    encodeItem(tk.first)
    emit(JUMP_ON_TRUE, end)
    encodeItem(tk.second)
    emit(OR)
    emit_tag(end)
    
def encode_raw_list(list):
    if gettype(list) == "list":
        emit(LIST)
        for item in list:
            encodeItem(item)
            emit(LIST_APPEND)
        return len(list)
    else:
        encodeItem(list)
        return 1

def encode_for(tk):
    start_tag = newtag()
    end_tag = newtag()
    # set for stack
    begin_tag_list.append(start_tag)
    end_tag_list.append(end_tag)
    ### load index and iterator
    encode_raw_list(tk.first.second)
    emit(ITER_NEW) # create a iterator
    emit_tag(start_tag)
    jump(end_tag, TM_NEXT)
    store(tk.first.first)
    encodeItem(tk.second)
    jump(start_tag)
    emit_tag(end_tag)
    # clear for stack
    begin_tag_list.pop()
    end_tag_list.pop()
    emit(POP) # pop iterator.
    
def encode_global(tk):
    def_global(tk.first)
    
def encode_try(tk):
    exception = newtag()
    end = newtag()
    if not asm_try(exception):
        encode_error(tk, "do not support cursive try")
    encodeItem(tk.first)
    emit(CLRJUMP)
    jump(end)
    emit_tag(exception)
    if tk.second != None:
        emit(LOAD_EX)
        store(tk.second)
    else:
        emit(POP)
    encodeItem(tk.third)
    emit_tag(end)
    asm_try_exit()
    
def do_nothing(tk):
    pass

def encode_del(tk):
    item = tk.first
    if item.type != 'get' and item.type != 'attr':
        encode_error(item, 'require get or attr expression')
    encodeItem(item.first)
    if item.type == 'attr':
        loadAttr(item.second)
    else:
        encodeItem(item.second)
    emit(TM_DEL)
    
def encode_annotation(tk):
    token = tk.first
    if token.val == "debugger":
        emit(TM_DEBUG)
    
def encode_attr(tk):
    tk.second.type = 'string'
    encodeItem(tk.first)
    encodeItem(tk.second)
    emit(GET)
    
def encode_in(tk):
    encodeItem(tk.first)
    if gettype(tk.second) == 'list' and isConstList(tk.second):
        g = getConstList(tk.second)
        emit_load(g)
    else:
        encodeItem(tk.second)
    emit(OP_IN)    
    
encode_map = {
    'if': encode_if,
    '=': encode_assign,
    ',': encode_tuple,
    'dict': encode_dict,
    'call': encode_call,
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
    'attr':encode_attr,
    'in': encode_in,
    '@':encode_annotation,
}

for k in op_map:
    encode_map[k] = encode_op
for k in op_ext_map:
    encode_map[k] = encode_op_ext

def getlineno(tk):
    if hasattr(tk, 'pos'):
        return tk.pos[0]
    elif hasattr(tk, 'first'):
        return getlineno(tk.first)
    return None
    
def encodeItem(tk):
    if tk == None: return 0
    # encode for statement list.
    if gettype(tk) == 'list':
        for i in tk:
            # comment 
            if i.type == 'string':
                continue
            lineno = getlineno(i)
            if lineno != None: emit(TM_LINE, lineno)
            encodeItem(i)
            if i.type == 'call': emit(POP)
        return
    r = encode_map[tk.type](tk)
    if r != None:return r
    return 1

load_type_list = ['number', 'name', "string", 'None']

def encode(content):
    global _tag_cnt
    global _bin 
    global _bin_ext
    _tag_cnt = 0
    _glo_idx = 0
    r = parse(content)
    encodeItem(r)

class EncodeCtx:
    def __init__(self, src):
        self.src = src

def compile(src, des = None):
    global ctx
    asm_init()
    ctx = EncodeCtx(src)
    encode(src)
    ctx = None
    code = gen_code()
    if des: save(des, code)
    return code

def compilefile(file, des = None):
    return compile(load(file), des)

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
    
def main():
    if len(ARGV) < 2:pass
    elif len(ARGV) == 2:
        import repl
        import tmcode
        tmcodes = tmcode.tmcodes
        # from tools import *
        replPrint = repl.replPrint
        code = compilefile(ARGV[1])
        list = split_instr(code)
        for item in list:
            print(tmcodes[item[0]], item[1])
        # replPrint(code, 0, 3)
    elif len(ARGV) == 3:
        compile(ARGV[1], ARGV[2])

if __name__ == "__main__":
    main()
