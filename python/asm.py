from boot import *
from tokenize import *
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

jmp_optimize_map = {
    LT: LT_JUMP_ON_FALSE,
    GT: GT_JUMP_ON_FALSE,
    LTEQ: LTEQ_JUMP_ON_FALSE,
    GTEQ: GTEQ_JUMP_ON_FALSE,
    EQEQ: EQEQ_JUMP_ON_FALSE,
    NOTEQ: NOTEQ_JUMP_ON_FALSE
}
        
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
            
def optimize1(nx, x, tags, jmp_list):
    for ins in x:
        if ins[0] in jmp_list:
            if ins[1] in tags:
                ins[1] = tags[ins[1]]
            pre = nx.pop()
            if ins[0] == POP_JUMP_ON_FALSE and pre[0] in jmp_optimize_map:
                ins[0] = jmp_optimize_map[pre[0]]
            else:
                nx.append(pre)
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
    #tags = {}
    #for ins in x:
        #if ins[0] == TAG:
            #if last:tags[ins[1]] = tag;ins[1]=None
            #else:last=1;tag = ins[1]
        #else: last = 0
    #nx = []
    #print(tags)
    #if optimize_jmp:optimize1(nx,x,tags,jmp_list)
    #else:optimize0(nx, x, tags,jmp_list)
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

