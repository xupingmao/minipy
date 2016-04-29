
def getLineNo(item):
    if hasattr(item, "pos"):
        return item.pos[0]
    elif hasattr(item, "first"):
        return getLineNo(item.first)


_handlers = {
    "number": visitNumber,
    "name":   visitName,
    "string": visitString,
    "None": visitNone,
    "=" : visitAssignment,
    "+" : visitAdd,
    "call": visitCall,
    "def": visitDef,
    "arg": visitArg,
    "return": visitReturn
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
    context.setFname(prefix)
    try:
        visitItemList(tree, context)
    except Exception as e:
        traceback()
        printAst(context.item)
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
