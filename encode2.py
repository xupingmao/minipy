from parse import *
from boot import *
from tmcode import *

_isClassScope = False
_isFunctionScope = False
_isGlobalScope = True

_globalRegIndex = 0
_globalTempRegs = []

_localRegIndex = 0
_localTempRegs = []

_functionVars = []
_functionGlobalVars = []
_tagIndex = 0
_instructions = []
_tagDict = {}

_startTags = []
_endTags = []


def doNothing(*args):
    pass

def compilerInit():
    global _isGlobalScope
    global _isFunctionScope
    global _tagIndex
    global _tagDict
    _tagIndex = 0
    _isGlobalScope = True
    _isFunctionScope = False
    # _globalTempRegs.clear()
    _globalTempRegs = []
    # _localTempRegs.clear()
    _localTempRegs = []
    # _functionVars.clear()
    _functionVars = []
    # _functionGlobalVars.clear()
    _functionGlobalVars = []
    # _instructions.clear()
    _instructions = []
    _tagDict = {}

def getTempReg():
    global _globalRegIndex
    global _localRegIndex
    if _isGlobalScope:
        if len(_globalTempRegs) > 0:
            temp = _globalTempRegs.pop()
            return temp
        temp = _globalRegIndex
        _globalRegIndex += 1
        # _globalTempRegs.append(temp)
    elif _isFunctionScope:
        temp = len(_functionVars)
        # _localRegIndex += 1
        addVar("#" + str(temp))
        # _localTempRegs.append(temp)
    return temp

def emit(args):
    # opcode = args[0]
    # if opcode == "TAG":
    #     value = args[1]
    #     _tagDict[value] = len(_instructions)
    #     return
    # else:
    #     print(args)
    _instructions.append(args)

def getNewTag():
    global _tagIndex
    temp = _tagIndex
    _tagIndex += 1
    return temp

def addBlockTags(start, end):
    global _startTags
    global _endTags
    _startTags.append(start)
    _endTags.append(end)

def removeBlockTags():
    _startTags.pop()
    _endTags.pop()


def freeTempVar(v):
    if _isGlobalScope:
        if v not in _globalTempRegs:
            _globalTempRegs.append(v)
    elif _isFunctionScope:
        if v not in _localTempRegs:
            _localTempRegs.append(v)
    # pass

def setScope(scope):
    global _isFunctionScope
    global _isGlobalScope
    if scope == "function":
        # _functionVars.clear()
        _functionVars = []
        # _localTempRegs.clear()
        _localTempRegs = []
        # _functionGlobalVars.clear()
        _functionGlobalVars = []
        _isFunctionScope = True
        _isGlobalScope = False
        _localRegIndex = 0
    elif scope == "global":
        _isGlobalScope = True
        _isFunctionScope = False
    
def isGlobalVar(name):
    if _isGlobalScope:
        return True
    if name in _functionGlobalVars:
        return True
    return False

def getGlobalVar(name):
    v = getTempReg()
    emit([GETG, v, name])
    return v

def getLocalVar(name):
    if name in _functionVars:
        return _functionVars.index(name)
    return getGlobalVar(name)

def getVar(name):
    if _isGlobalScope:
        return getGlobalVar(name)
    return getLocalVar(name)

def addVar(name):
    if _isGlobalScope:
        return
    if name in _functionGlobalVars:
        return
    _functionVars.append(name)

def encode_error(token, msg):
    global ctx
    compile_error("encode", ctx.src, token, msg)

def encodeAssign(token, dest):
    left = token.first
    right = token.second
    if istype(left, "list"):
        # multi assignment
        pass
    else:
        if left.type == "name":
            addVar(left.val)
            if isGlobalVar(left.val):
                r = encodeItem(right)
                emit([SETG, left.val, r])
                freeTempVar(r)
            else:
                v = getVar(left.val)
                r = encodeItem(right, v)
        else:
            self = encodeItem(left.first)
            key  = encodeItem(left.second)
            value = encodeItem(right)
            emit([SET, self, key, value])

    return None

def encodeName(token, dest=None):
    v = getVar(token.val)
    if dest == None:
        return v
    else:
        emit([MOV, dest, v])
    return v

def encodeOp(token, dest, op):
    if dest == None:
        dest = getTempReg()
    a = encodeItem(token.first)
    b = encodeItem(token.second)
    emit([op, dest, a, b])
    return dest

def encodeAdd(token, dest = None):
    return encodeOp(token, dest, ADD)

def encodeSub(token, dest = None):
    return encodeOp(token, dest, SUB)

def encodeMul(token, dest = None):
    return encodeOp(token, dest, MUL)

def encodeDiv(token, dest = None):
    return encodeOp(token, dest, DIV)

def encodeMod(token, dest = None):
    return encodeOp(token, dest, MOD)

def encodeGT(token, dest=None):
    return encodeOp(token, dest, GT)

def encodeLT(token, dest=None):
    return encodeOp(token, dest, LT)

def encodeGTEQ(token, dest=None):
    return encodeOp(token, dest, GTEQ)

def encodeLTEQ(token, dest=None):
    return encodeOp(token, dest, LTEQ)

def encodeEQEQ(token, dest = None):
    return encodeOp(token, dest, EQEQ)

def encodeNEQ(token, dest=None):
    return encodeOp(token, dest, NEQ)

def encodeIn(token, dest=None):
    return encodeOp(token, dest, OP_IN)

def encodeNotin(token, dest=None):
    return encodeOp(token, dest, NOTIN)

def encodeOp1(token, dest, op):
    if dest == None:
        dest = getTempReg()
    a = encodeItem(token.first)
    emit([op, dest, a])
    return dest

def encodeNot(token, dest=None):
    return encodeOp1(token, dest, NOT)

def encodeNeg(token, dest=None):
    return encodeOp1(token, dest, NEG)

def encodeAttr(token, dest=None):
    self = encodeItem(token.first)
    key =  encodeItem(token.second)
    v = getTempReg()
    emit([GET, v, self, key])
    return v

def encodeGet(token, dest=None):
    return encodeAttr(token, dest)


def encodeInplaceOp(token, dest, op):
    a = encodeItem(token.first)
    b = encodeItem(token.second)
    emit([op, a, a, b])
    return a

def encodeInplaceAdd(token, dest):
    return encodeInplaceOp(token, dest, "ADD")

def encodeInplaceSub(token, dest=None):
    return encodeInplaceOp(token, dest, "SUB")

def encodeInplaceMul(token, dest=None):
    return encodeInplaceOp(token, dest, "MUL")

def encodeInplaceDiv(token, dest=None):
    return encodeInplaceOp(token, dest, "DIV")

def encodeInplaceMod(token, dest=None):
    return encodeInplaceOp(token, dest, "MOD")


def encodeNumber(token, dest=None):
    r = getTempReg()
    emit([LD_CONST, r, token.val])
    return r

def encodeString(token, dest=None):
    r = getTempReg()
    emit([LD_CONST, r, token.val])
    return r

def encodeNone(token, dest = None):
    if dest == None:
        dest = getTempReg()
    emit([LD_NONE, dest])
    return dest

def encodeList(token, dest=None):
    if dest == None:
        listReg = getTempReg()
    else:
        listReg = dest
    emit([LIST, listReg])
    if token.first == None:
        return listReg
    if istype(token.first, "list"):
        for item in token.first:
            # temp regs can be freed here.
            v = encodeItem(item)
            emit([LISTAPPEND, listReg, v])
    else:
        v = encodeItem(token.first)
        emit([LISTAPPEND, listReg, v])
    return listReg

def encodeDict(token, dest=None):
    dictReg = getTempReg()
    if token.first == None:
        return dictReg
    for item in token.first:
        k = encodeItem(item[0])
        v = encodeItem(item[1])
        emit([SET, dictReg, k, v])
    return dictReg

def encodeReturn(token, dest=None):
    r = encodeItem(token.first)
    emit([RET, r])
    return r

def encodeGlobal(token, dest=None):
    name = token.first.val
    if name not in _functionGlobalVars:
        _functionGlobalVars.append(name)

def encodeFrom(token, dest=None):
    name = token.first.val
    value = token.second.val
    emit(["IMPORT", name, value])

def encodeContinue(token, dest=None):
    emit([JMP, _startTags[-1]])
    return None

def encodeBreak(token, dest=None):
    emit([JMP, _endTags[-1]])
    return None

def encodeArg(token, dest=None):
    name = token.first.val
    value = token.second
    # addVar(name), add name in encodeDef
    if value != None:
        r = encodeItem(value, getVar(name))
        # emit(["MOV", getVar(name), r])


def encodeNArg(token, dest=None):
    name = token.first.val
    # addVar(name)
    emit([NARG, getVar(name)])

def encodeFunction(token, dest = None):
    setScope("function")
    name = token.first.val
    emit([TM_DEF, name])
    args = token.second
    body = token.third
    if istype(args, "list"):
        for item in args:
            name = item.first.val
            addVar(name)
    else:
        name = item.first.val
        addVar(name)
    encodeItem(args)
    encodeItem(body)
    emit([TM_EOF])
    setScope("global")
    return None

def encodeCall(token, dest=None):
    name = token.first
    args = token.second
    destReg = dest
    if dest == None:
        destReg = getTempReg()
    func = encodeItem(name)
    if args == None:
        emit([CALL0, destReg, func, 0])
    elif istype(args, "list"):
        if len(args) == 1:
            arg0 = encodeItem(args[0])
            callee = [CALL1, destReg, func, arg0]
            emit(callee)
        elif len(args) == 2:
            arg0 = encodeItem(args[0])
            arg1 = encodeItem(args[1])
            callee = [CALL2, destReg, func, arg0, arg1, 0, 0, 0]
            emit(callee)
        elif len(args) == 3:
            arg0 = encodeItem(args[0])
            arg1 = encodeItem(args[1])
            arg2 = encodeItem(args[2])
            callee = [CALL3, destReg, func, arg0, arg1, arg2, 0, 0]
            emit(callee)
        elif len(args) == 4:
            arg0 = encodeItem(args[0])
            arg1 = encodeItem(args[1])
            arg2 = encodeItem(args[2])
            arg3 = encodeItem(args[3])
            callee = [CALL4, destReg, func, arg0, arg1, arg2, arg3, 0]
            emit(callee)
        elif len(args) == 5:
            arg0 = encodeItem(args[0])
            arg1 = encodeItem(args[1])
            arg2 = encodeItem(args[2])
            arg3 = encodeItem(args[3])
            arg4 = encodeItem(args[4])
            callee = [CALL5, destReg, func, arg0, arg1, arg2, arg3, arg4]
            emit(callee)
        else:
            raise("too many arguments!, arguments=" + str(len(args)))
    else:
        arg0 = encodeItem(args)
        emit([CALL1, destReg, func, arg0])
    return destReg

def encodeIf(token, dest=None):
    cond = token.first
    body = token.second
    rest = token.third
    r = encodeItem(cond, None)
    elseOfIf = getNewTag()
    endOfIf = getNewTag()
    emit([JF, r, elseOfIf])
    freeTempVar(r)
    encodeItem(body)
    emit([JMP, endOfIf])
    emit([TAG, elseOfIf])
    encodeItem(rest)
    emit([TAG, endOfIf])
    return None

def encodeWhile(token, dest=None):
    cond = token.first
    body = token.second
    r = getTempReg()
    startOfWhile = getNewTag()
    endOfWhile = getNewTag()
    addBlockTags(startOfWhile, endOfWhile)
    emit([TAG, startOfWhile])
    r = encodeItem(cond, r)
    emit([JF, r, endOfWhile])
    encodeItem(body)
    emit([JMP, startOfWhile])
    emit([TAG, endOfWhile])
    removeBlockTags()
    return None

def encodeFor(token, dest=None):
    startOfFor = getNewTag()
    endOfFor = getNewTag()
    addBlockTags(startOfFor, endOfFor)
    printAst(token)
    # handle for
    removeBlockTags()
    return None

_encode_map = {
    "=": encodeAssign,
    "+": encodeAdd,
    "-": encodeSub,
    "*": encodeMul,
    "/": encodeDiv,
    "%": encodeMod,
    ">": encodeGT,
    "<": encodeLT,
    "==": encodeEQEQ,
    "!=": encodeNEQ,
    ">=": encodeGTEQ,
    "<=": encodeLTEQ,
    "+=": encodeInplaceAdd,
    "-=": encodeInplaceSub,
    "*=": encodeInplaceMul,
    "/=": encodeInplaceDiv,
    "%=": encodeInplaceMod,
    "in": encodeIn,
    "notin": encodeNotin,
    "not": encodeNot,
    "neg": encodeNeg,
    "attr": encodeAttr,
    "get": encodeGet,
    "name": encodeName,
    "number": encodeNumber,
    "string": encodeString,
    "None": encodeNone,
    "list": encodeList,
    "dict": encodeDict,
    "if": encodeIf,
    "while": encodeWhile,
    "for": encodeFor,
    "call": encodeCall,
    "def": encodeFunction,
    "arg": encodeArg,
    "narg": encodeNArg,
    "return": encodeReturn,
    "global": encodeGlobal,
    "from": encodeFrom,
    "pass": doNothing,
    "continue": encodeContinue,
    "break": encodeBreak
}


def findTag(code, val):
    cur = 0
    for ins in code:
        opcode = ins[0]
        if opcode == TAG and ins[1] == val:
            # this `return will disturb the normal `for loop
            return cur
        if opcode in _multi_ins:
            cur+=2
        elif opcode != TAG:
            cur+=1

_jmp_list = [JMP, JF]
_multi_ins = [CALL2, CALL3, CALL4, CALL5]

def patchJmps(code):
    codeSize = len(code)
    cur = 0
    newCode = []
    for ins in code:
        opcode = ins[0]
        isJmp = 0
        if opcode == JMP:
            value = ins[1]
            pos = findTag(code, value)
            gap = pos - cur
            if gap < 0:
                ins = [UP_JMP, -gap]
            else:
                ins[1] = gap
        elif opcode == JF:
            value = ins[2]
            pos = findTag(code, value)
            gap = pos - cur
            if gap < 0:
                ins = [UP_JMP, -gap]
            else:
                ins[2] = gap
        if opcode in _multi_ins:
            cur += 2
            newCode.append(ins)
        elif opcode != TAG:
            cur+=1
            newCode.append(ins)
    return newCode

def getlineno(tk):
    if hasattr(tk, 'pos'):
        return tk.pos[0]
    elif hasattr(tk, 'first'):
        return getlineno(tk.first)
    return None

def encodeItem(token, dest = None):
    if token == None:
        return None
    if istype(token, "list"):
        last = None
        for item in token:
            lineno = getlineno(i)
            if lineno != None: emit(TM_LINE, lineno)
            last = encodeItem(item, dest)
        return last
    else:
        return _encode_map[token.type](token, dest)

def resolveConstants(codelist):
    for code in codelist:
        op = code[0]
        if op == TM_DEF:
            code[1] = getConstIdx(code[1])
        elif op == LD_CONST:
            code[2] = getConstIdx(code[2])
        elif op == GETG:
            code[2] = getConstIdx(code[2])
        elif op == SETG:
            code[1] = getConstIdx(code[1])

def tmLock():
    pass

def tmUnlock():
    pass

def compile(text):
    tmLock()
    tree = parse(text)
    compilerInit()
    encodeItem(tree)
    code = patchJmps(_instructions)
    resolveConstants(code)
    tmUnlock()
    return code

def _dumpConstants():
    b = ""
    for i in range(getConstLen()):
        v = getConst(i)
        t = chr(NEW_STRING)
        if istype(v, 'number'):
            t = chr(NEW_NUMBER)
            v = str(v)
        b+=t+code16(len(v))+v
    return b


def codeStr(s):
    return code32(len(s))+s

def _code8List(list):
    bin = ''
    for item in list:
        bin += code8(item)
    return bin

def _dump(fname, codeList):
    eop = _code8List([TM_EOP,0,0,0])
    constants = _dumpConstants() + eop
    bin = ""
    for code in codeList:
        op = code[0]
        try:
            if op == TM_DEF:
                bin += code8(op) + code16(code[1])
            elif op >= TM_LINE and op <= UP_JMP:
                bin += code8(op) + code16(code[1]) + code8(0)
            elif op >= LD_CONST and op <= JF:
                bin += code8(op) + code8(code[1]) + code16(code[2])
            elif op == SETG:
                bin += code8(op) + code16(code[1]) + code8(code[1])
            elif op >= ADD and op <= GET:
                bin += code8(op) + code8(code[1]) + code8(code[2]) + code8(code[3])
            elif op == RET:
                bin += code8(op) + code16(code[1]) + code8(0)
            elif op == TM_EOF:
                bin += code8(op) + code16(0) + code8(0)
            elif op >= CALL0 and op <= CALL5:
                bin += _code8List(code)
            elif op >= LD_NONE and op <= DICT:
                bin += code8(op) + code8(code[1]) + code16(0)
            elif op >= NOT and op <= LISTAPPEND:
                bin += code8(op) + code8(code[1]) + code8(code[2]) + code8(0)
            else:
                raise Exception("unknown opcode, value=" + str(op))
        except Exception as e:
            print(code)
            print(e)
            raise
    # save(fname, bin)
    bin = code32(2) + codeStr("constants") + codeStr(constants) \
        + codeStr("test") + codeStr(bin+eop)
    list = []
    for c in bin:
        list.append(str(ord(c)))
    print("unsigned char bin[] = {")
    print(",".join(list))
    print("};")


def main():
    if len(ARGV) < 2:pass
    elif len(ARGV) == 2:
        fname = ARGV[1]
        code = compile(load(fname))
        for i in code:
            print(i)
    elif len(ARGV) == 3:
        arg = ARGV[1]
        if arg == "-dump":
            fname = ARGV[2]
            code = compile(load(fname))
            _dump(fname + ".bin", code)

if __name__ == "__main__":
    main()
