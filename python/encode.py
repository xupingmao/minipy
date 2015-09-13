from parse import *
from asm import *
from boot import *

def encode_error(token, msg):
    global ctx
    compile_error("encode", ctx.src, token, msg)
    
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
        emit(SET)
    elif t.type == 'attr':
        encode_item(t.first)
        load_attr(t.second)
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
    encode_item(tk.first)
    encode_item(tk.second)
    emit(op_map[tk.type])

def encode_notin(tk):
    encode_in(tk)
    emit(NOT)

def encode_op_ext(tk):
    encode_item(tk.first)
    encode_item(tk.second)
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
        encode_item(item)
        emit(LIST_APPEND)

        
def encode_list(v):
    emit(LIST, 0)
    encode_list0(v.first)
    #emit(LIST, tk_list_len(v.first))

def isconst(token):
    if token.type == ',':
        return isconst(token.first) and isconst(token.second)
    elif token.type in ('string', 'number', 'None'):
        return True

def getconst(const):
    asm_switch_out()
    g = newglo()
    emit(LIST)
    encode_list0(const)
    store_glo(g)
    asm_switch_out()
    return g
    
    
def encode_if(tk):
    # refuse assignment in if condition
    if tk.first.type == '=':
        encode_error(tk.first, 'do not allow assignment in if condition')
    encode_item(tk.first)
    else_tag,end_tag = newtag(), newtag()
    jump(else_tag, POP_JUMP_ON_FALSE)
    # optimize for `if x in const_list`
    encode_item(tk.second)
    jump(end_tag)
    emit_tag(else_tag)
    encode_item(tk.third)
    emit_tag(end_tag)
    
    
def encode_assign(tk):
    n = encode_item(tk.second)
    if tk.first.type == ',':
        if n == 1:emit(TM_UNARRAY,tk_list_len(tk.first))
        else:emit(TM_ROT, n)
    store(tk.first)
    
def encode_tuple(tk):
    return encode_item(tk.first) + encode_item(tk.second)
    
def encode_dict(tk):
    items = tk.first
    emit(DICT, 0)
    if items != None:
        for item in items:
            encode_item(item[0])
            encode_item(item[1])
            emit(DICT_SET)
    
def encode_neg(tk):
    if tk.first.type == 'number':
        tk = tk.first
        tk.val = -tk.val
        encode_item(tk)
    else:
        encode_item(tk.first)
        emit(NEG)
        
def encode_not(tk):
    encode_item(tk.first)
    emit(NOT)
    
    
def encode_call(tk):
    encode_item(tk.first)
    n = encode_item(tk.second)
    emit(CALL, n)

    
def encode_def(tk, in_class = 0):
    emit_def(tk.first)
    # emit(TM_DEF, 0)
    # regs = []
    # emit(0, 0) # filename
    regs = emit(0, 0) # regs
    func_end  = newtag()
    jump(func_end)
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
            encode_item(item.second)
            store(item.first)
        else:
            parg += 1
    if not narg:
        emit(LOAD_PARAMS, parg*256 + varg)
    encode_item(tk.third)
    emit(TM_EOF)
    regs[1] = asm_get_regs()
    emit_tag(func_end)
    #loc_num_ins[1] = get_loc_num()
    pop_scope()
    if not in_class:
        emit_store(tk.first)
 

def encode_class(tk):
    buildclass = []
    emit(DICT, 0)
    store(tk.first)
    for func in tk.second:
        if func.type != 'def':
            encode_error(func, "non-func expression in class is invalid")
        encode_def(func, 1)
        emit_load(tk.first)
        load_attr(func.first)
        emit(SET)

def encode_return(tk):
    if tk.first:
        if tk.first.type == ',':
            emit(LIST)
            encode_list0(tk.first)
            #emit(LIST, tk_list_len(tk.first))
        else:
            encode_item(tk.first)
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
    encode_item(tk.first)
    jump(end_tag, POP_JUMP_ON_FALSE)
    encode_item(tk.second)
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
    encode_item(Token("name", "_import"))
    encode_item(mod)
    item.type = 'string'
    emit(LOAD_GLOBALS)
    encode_item(item)
    emit(CALL,3)
    emit(POP)

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
    encode_item(Token('name','_import'))
    item.type = 'string'
    encode_item(item)
    emit(LOAD_GLOBALS)
    emit(CALL,2)
    emit(POP)

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
    emit(JUMP_ON_FALSE, end)
    encode_item(tk.second)
    emit(AND)
    emit_tag(end)
    

def encode_or(tk):
    end = newtag()
    encode_item(tk.first)
    emit(JUMP_ON_TRUE, end)
    encode_item(tk.second)
    emit(OR)
    emit_tag(end)
    
def encode_for(tk):
    start_tag = newtag()
    end_tag = newtag()
    # set for stack
    begin_tag_list.append(start_tag)
    end_tag_list.append(end_tag)
    ### load index and iterator
    encode_item(tk.first.second)
    emit(ITER_NEW) # create a iterator
    emit_tag(start_tag)
    jump(end_tag, TM_NEXT)
    store(tk.first.first)
    encode_item(tk.second)
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
    encode_item(tk.first)
    jump(end)
    emit_tag(exception)
    if tk.second != None:
        emit(LOAD_EX)
        store(tk.second)
    else:
        emit(POP)
    encode_item(tk.third)
    emit_tag(end)
    asm_try_exit()
    
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
    emit(TM_DEL)
    
def encode_debug():
    emit(TM_DEBUG)
    
def encode_attr(tk):
    tk.second.type = 'string'
    encode_item(tk.first)
    encode_item(tk.second)
    emit(GET)
    
def encode_in(tk):
    encode_item(tk.first)
    if tk.second.type == ',' and isconst(tk.second):
        g = getconst(tk.second)
        emit_load(g)
    else:
        encode_item(tk.second)
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
    '@':encode_debug,
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
    
def encode_item(tk):
    if tk == None: return 0
    # encode for statement list.
    if gettype(tk) == 'list':
        for i in tk:
            # comment 
            if i.type == 'string':
                continue
            lineno = getlineno(i)
            if lineno != None: emit(TM_LINE, lineno)
            encode_item(i)
            if i.type == 'call': emit(POP)
        return
    r = encode_map[tk.type](tk)
    if r:return r
    return 1

load_type_list = ['number', 'name', "string", 'None']

def encode(content):
    global _tag_cnt
    global _bin 
    global _bin_ext
    _tag_cnt = 0
    _glo_idx = 0
    r = parse(content)
    encode_item(r)

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
        from tools import repl
        replPrint = repl.replPrint
        code = compilefile(ARGV[1])
        replPrint(code, 0, 3)
    elif len(ARGV) == 3:
        compile(ARGV[1], ARGV[2])
if __name__ == "__main__":
    main()
