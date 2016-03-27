from boot import *
from parse import *
from tmcode import *

_names = None
_codeList = None
_extCodeList = None # extra _codeList for optimize.

_jmpList = [
    JUMP_ON_FALSE, 
    JUMP_ON_TRUE, 
    POP_JUMP_ON_FALSE, 
    SETJUMP, 
    JUMP,
    TM_NEXT
]


_opDict = {
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
_opExtDict = {
    '+=' : ADD,
    '-=' : SUB,
    '*=' : MUL,
    '/=' : DIV,
    '%=' : MOD
}

_beginTagList = [-1]
_endTagList = [-1]

_tagCnt = 0
_globalIndex = 0


def loadConst(const):
    emit(LOAD_CONSTANT, getConstIdx(const.val))

class Scope:
    def __init__(self):
        self.locals = []
        self.globals = []
        self.jmps = 0 # try block count.
        
    def addGlobal(self, v):
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
        
    def addLocal(self, v):
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
            
    def indexLocal(self, v):
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
            idx = self.indexLocal(v)
            emit(STORE_LOCAL, idx)
        else:
            idx = getConstIdx(v.val)
            emit(STORE_GLOBAL, idx)

def asmInit():
    global _names
    global _codeList
    global _extCodeList

    _names = Names()
    _codeList = []
    _extCodeList = []

def chkTryBlock(tag):
    if _names.scope.jmps > 0:
        return 0
    _names.scope.jmps += 1
    emit(SETJUMP, tag)
    return 1

def exitTryBlock():
    _names.scope.jmps -= 1
    
def asm_switch__codeList():
    global _codeList
    global _extCodeList
    _codeList, _extCodeList = _extCodeList, _codeList

def asm_get_regs():
    return len(_names.scope.locals)

def storeGlobal(glo):
    idx = getConstIdx(glo.val)
    emit(STORE_GLOBAL, idx)

def addGlobal(v):
    _names.scope.globals.append(v.val)
# opcode : op

def emit(op, val = 0):
    ins = [op, val]
    _codeList.append(ins)
    return ins
    
def emitDef(v):
    idx = getConstIdx(v.val)
    emit(TM_DEF, idx)

# inside function, assignment will be made to locals by default,
# except that a global is declared. so we must save the declared
# globals to a local scope.
# 
def emitLoad(v):
    if v == None:
        emit(LOAD_NONE)
        return;
    t = v.type
    if t == 'string' or t == 'number':
        loadConst(v)
    elif t == 'None':
        emit(LOAD_NONE, 0)
    elif t == 'name':
        _names.load(v)
    else:
        print('LOAD_LOCAL ' + str(v.val))
        
def saveAsBin(lst):
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

def findTag(code, val):
    cur = 0
    for ins in code:
        if ins[0] == TAG and ins[1] == val:
            # this `return will disturb the normal `for loop
            return cur
        if ins[0] != TAG:
            cur+=1

def handleJmps(code):
    codeSize = len(code)
    cur = 0
    newCode = []
    for ins in code:
        if ins[0] in _jmpList:
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
    nx = handleJmps(x)
    return nx
    
    
def genCode(lst = False):
    global _codeList
    global _extCodeList

    emit(TM_EOP)
    #x = gen_constants(tagsize) + _codeList
    x = _extCodeList + _codeList
    # release memory
    _codeList = []
    _extCodeList = []
    x = optimize(x)
    if lst:return x
    for i in x:
        if i[1] == None:
            print(i)
    return saveAsBin(x)


def def_local(v):
    _names.addLocal(v)

def get_loc_num():
    return len(_names.scope.locals)

def push_scope():
    _names.push()

def pop_scope():
    _names.pop()

def emit_store(v):
    _names.store(v)


def encode_error(token, msg):
    global ctx
    compile_error("encode", ctx.src, token, msg)
    
def loadAttr(name):
    if name.type == 'name':
        name.type = 'string'
    emitLoad(name)
    
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

def newglo():
    global _globalIndex
    _globalIndex += 1
    return Token("name", "#" + str(_globalIndex-1))

def newtag():
    global _tagCnt
    _tagCnt+=1
    return _tagCnt-1

def jump(p, ins = JUMP):
    emit(ins, p)

def emitTag(p):
    emit(TAG, p)
    

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
    emit(_opDict[tk.type])

def encodeNotin(tk):
    encodeIn(tk)
    emit(NOT)

def encodeInplaceOp(tk):
    encodeItem(tk.first)
    encodeItem(tk.second)
    emit(_opExtDict[tk.type])
    store(tk.first)
    
# [1, 2, 3] 
# will be parsed like.
#     ^
#    ^  3
#  1  2
def encodeList0(v):
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

        
def encodeList(v):
    emit(LIST, 0)
    # encodeList0(v.first)
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
    asm_switch__codeList()
    g = newglo()
    emit(LIST)
    for item in list:
        encodeItem(item)
        emit(LIST_APPEND)
    storeGlobal(g)
    asm_switch__codeList()
    return g
    
    
def encodeIf(tk):
    # refuse assignment in if condition
    if tk.first.type == '=':
        encode_error(tk.first, 'do not allow assignment in if condition')
    encodeItem(tk.first)
    else_tag,end_tag = newtag(), newtag()
    jump(else_tag, POP_JUMP_ON_FALSE)
    # optimize for `if x in const_list`
    encodeItem(tk.second)
    jump(end_tag)
    emitTag(else_tag)
    encodeItem(tk.third)
    emitTag(end_tag)
    
    
def encodeAssign(tk):
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
    
def encodeTuple(tk):
    return encodeItem(tk.first) + encodeItem(tk.second)
    
def encodeDict(tk):
    items = tk.first
    emit(DICT, 0)
    if items != None:
        for item in items:
            encodeItem(item[0])
            encodeItem(item[1])
            emit(DICT_SET)
    
def encodeNeg(tk):
    if tk.first.type == 'number':
        tk = tk.first
        tk.val = -tk.val
        encodeItem(tk)
    else:
        encodeItem(tk.first)
        emit(NEG)
        
def encodeNot(tk):
    encodeItem(tk.first)
    emit(NOT)
    
    
def encodeCall(tk):
    encodeItem(tk.first)
    if gettype(tk.second) == "list":
        for item in tk.second:
            encodeItem(item)
        n = len(tk.second)
    else:
        n = encodeItem(tk.second)
    emit(CALL, n)

    
def encodeDef(tk, in_class = 0):
    emitDef(tk.first)
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
    #emitTag(func_end)
    #loc_num_ins[1] = get_loc_num()
    pop_scope()
    if not in_class:
        emit_store(tk.first)
 

def encodeClass(tk):
    buildclass = []
    emit(DICT, 0)
    store(tk.first)
    for func in tk.second:
        if func.type == "pass": continue
        if func.type != "def":
            encode_error(func, "non-func expression in class is invalid")
        encodeDef(func, 1)
        emitLoad(tk.first)
        loadAttr(func.first)
        emit(SET)

def encodeReturn(tk):
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
        emitLoad(None);
    emit(RETURN)

def encodeWhile(tk):
    start_tag, end_tag = newtag(), newtag()
    # set while stack
    _beginTagList.append(start_tag)
    _endTagList.append(end_tag)
    ###
    emitTag(start_tag)
    encodeItem(tk.first)
    jump(end_tag, POP_JUMP_ON_FALSE)
    encodeItem(tk.second)
    #emit(UP_JUMP, start_tag)
    jump(start_tag)
    emitTag(end_tag)
    # clear while stack
    _beginTagList.pop()
    _endTagList.pop()
    
def encodeContinue(tk):
    #emit( UP_JUMP, _beginTagList[-1] )
    jump(_beginTagList[-1])

    
def encodeBreak(tk):
    jump(_endTagList[-1])

def encode_impIt_one(mod, item):
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
    
def encode_impoI_many(mod, items):
    mod = _import_name2str(mod)
    if items.type == ',':
        encode_impoI_many(mod, items.first)
        encode_impoI_many(mod, items.second)
    else:
        encode_impIt_one(mod, items)

def encodeFrom(tk):
    encode_impoI_many(tk.first, tk.second)

def _encodeImport(item):
    item.type = 'string'
    encodeItem(item)
    emit(OP_IMPORT, 1)

def encodeImport(tk):
    if tk.first.type == ',':
        tk = tk.first
        _encodeImport(tk.first)
        _encodeImport(tk.second)
    else:
        _encodeImport(tk.first)


def encodeAnd(tk):
    end = newtag()
    encodeItem(tk.first)
    emit(JUMP_ON_FALSE, end)
    encodeItem(tk.second)
    emit(AND)
    emitTag(end)
    

def encodeOr(tk):
    end = newtag()
    encodeItem(tk.first)
    emit(JUMP_ON_TRUE, end)
    encodeItem(tk.second)
    emit(OR)
    emitTag(end)
    
def encodeRawList(list):
    if gettype(list) == "list":
        emit(LIST)
        for item in list:
            encodeItem(item)
            emit(LIST_APPEND)
        return len(list)
    else:
        encodeItem(list)
        return 1

def encodeFor(tk):
    start_tag = newtag()
    end_tag = newtag()
    # set for stack
    _beginTagList.append(start_tag)
    _endTagList.append(end_tag)
    ### load index and iterator
    encodeRawList(tk.first.second)
    emit(ITER_NEW) # create a iterator
    emitTag(start_tag)
    jump(end_tag, TM_NEXT)
    store(tk.first.first)
    encodeItem(tk.second)
    jump(start_tag)
    emitTag(end_tag)
    # clear for stack
    _beginTagList.pop()
    _endTagList.pop()
    emit(POP) # pop iterator.
    
def encodeGlobal(tk):
    addGlobal(tk.first)
    
def encodeTry(tk):
    exception = newtag()
    end = newtag()
    if not chkTryBlock(exception):
        encode_error(tk, "do not support cursive try")
    encodeItem(tk.first)
    emit(CLRJUMP)
    jump(end)
    emitTag(exception)
    if tk.second != None:
        emit(LOAD_EX)
        store(tk.second)
    else:
        emit(POP)
    encodeItem(tk.third)
    emitTag(end)
    exitTryBlock()
    
def doNothing(tk):
    pass

def encodeDel(tk):
    item = tk.first
    if item.type != 'get' and item.type != 'attr':
        encode_error(item, 'require get or attr expression')
    encodeItem(item.first)
    if item.type == 'attr':
        loadAttr(item.second)
    else:
        encodeItem(item.second)
    emit(TM_DEL)
    
def encodeAnnotation(tk):
    token = tk.first
    if token.val == "debugger":
        emit(TM_DEBUG)
    
def encodeAttr(tk):
    tk.second.type = 'string'
    encodeItem(tk.first)
    encodeItem(tk.second)
    emit(GET)
    
def encodeIn(tk):
    encodeItem(tk.first)
    if gettype(tk.second) == 'list' and isConstList(tk.second):
        g = getConstList(tk.second)
        emitLoad(g)
    else:
        encodeItem(tk.second)
    emit(OP_IN)    
    
_encodeDict = {
    'if': encodeIf,
    '=': encodeAssign,
    ',': encodeTuple,
    'dict': encodeDict,
    'call': encodeCall,
    'neg': encodeNeg,
    'not': encodeNot,
    'list':encodeList,
    'def':encodeDef,
    'del':encodeDel,
    'class':encodeClass,
    'return':encodeReturn,
    'while':encodeWhile,
    'continue': encodeContinue,
    'break':encodeBreak,
    'from':encodeFrom,
    'import':encodeImport,
    'and':encodeAnd,
    'or':encodeOr,
    'for':encodeFor,
    'global':encodeGlobal,
    'name':emitLoad,
    'number':emitLoad,
    'string':emitLoad,
    'None':emitLoad,
    'True':emitLoad,
    'False':emitLoad,
    'try':encodeTry,
    'pass':doNothing,
    'notin':encodeNotin,
    'attr':encodeAttr,
    'in': encodeIn,
    '@':encodeAnnotation,
}

for k in _opDict:
    _encodeDict[k] = encode_op
for k in _opExtDict:
    _encodeDict[k] = encodeInplaceOp

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
    r = _encodeDict[tk.type](tk)
    if r != None:return r
    return 1

load_type_list = ['number', 'name', "string", 'None']

def encode(content):
    global _tagCnt
    global _bin 
    global _bin_ext
    _tagCnt = 0
    _globalIndex = 0
    r = parse(content)
    encodeItem(r)

class EncodeCtx:
    def __init__(self, src):
        self.src = src

def compile(src, des = None):
    global ctx
    asmInit()
    ctx = EncodeCtx(src)
    encode(src)
    ctx = None
    code = genCode()
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
