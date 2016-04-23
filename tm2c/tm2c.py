

from context import *
from parse import *
import sys

def getLineNo(item):
    if hasattr(item, "pos"):
        return item.pos[0]
    elif hasattr(item, "first"):
        return getLineNo(item.first)

def visitNumber(item, context):
    ref = context.getNumber(item.val)
    context.push(ref, ref)

def visitName(item, context):
    if context.scope == "global":
        # ref = context.getVar(item.val)
        context.push(None, sformat("tmGetGlobal(%s)", context.getString(item.val)))
    else:
        context.push(item.val, item.val)

def visitAdd(item, context):
    visitItem(item.first, context)
    visitItem(item.second, context)
    name2, v2 = context.pop()
    name1, v1 = context.pop()
    if name1 == None:
        name1 = context.getTemp(v1)
    if name2 == None:
        name2 = context.getTemp(v2)
    context.push(None, sformat("tmAdd(%s, %s)", name1,name2))

def visitAssignment(item, context):
    # tempSize = context.getTempSize()
    visitItem(item.second, context)
    first = item.first
    if first.type == "name":
        name = first.val
        # ref, val = context.pop()
        # context.push(name, sformat("%s=%s", name, val))
        context.store(name)
        # context.restoreTempSize(tempSize)
    else:
        # handle set, multi-assignment, etc.
        pass

def visitCall(item, context):
    first = item.first
    second = item.second
    visitItem(first, context)
    funcRef, val = context.pop()
    if funcRef == None:
        funcRef = context.getTemp(val)
    if istype(second, "list"):
        argList = []
        for item in second:
            visitItem(item, context)
            ref, val = context.pop()
            if ref == None:
                ref = context.getTemp(val)
            argList.append(ref)
        context.push(None, sformat("tmCall(%s,%s,%s,%s);", \
            getLineNo(item), funcRef, len(argList), ",".join(argList)))
    elif second == None:
        context.push(None, sformat("tmCall(%s,%s,0);", getLineNo(item), funcRef))
    else:
        visitItem(second)
        ref, val = context.pop()
        if ref == None:
            ref = context.getTemp(val)
        context.push(None, sformat("tmCall(%s,%s,1,%s);", getLineNo(item), funcRef, ref))

_handlers = {
    "number": visitNumber,
    "name":   visitName,
    "=" : visitAssignment,
    "+" : visitAdd,
    "call": visitCall,
}

def visitItem0(item, context):
    _handlers[item.type](item, context)

def visitItem(item, context):
    context.item = item
    if istype(item, "list"):
        for item0 in item:
            visitItem(item0, context)
    else:
        visitItem0(item, context)

def visitItemList(itemList, context):
    for item in itemList:
        tempSize = context.getTempSize()
        visitItem(item, context)
        context.restoreTempSize(tempSize)


def tm2c(fname, src, prefix=None):
    tree = parse(src)
    # printAst(tree)
    context = Context()
    try:
        visitItemList(tree, context)
    except Exception as e:
        print(context.item)
        print(e)
    return context.getCode()
    
if __name__ == "__main__":
    name = sys.argv[1]
    # path = "../test/tm2c/" + name
    path = name
    text = load(path)
    pathes = path.split('/')
    if len(pathes) > 1:
        name = pathes[-1]
    mod = name.split(".")[0]
    code = tm2c(name, text, mod)
    print(code)
