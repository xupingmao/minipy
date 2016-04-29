
def visitOp(item, context, op):
    visitItem(item.first, context)
    visitItem(item.second, context)
    name2, v2 = context.pop()
    name1, v1 = context.pop()
    if name1 == None:
        name1 = context.getTemp(v1)
    if name2 == None:
        name2 = context.getTemp(v2)
    context.push(None, sformat("%s(%s, %s)", op, name1,name2))

def visitAdd(item, context):
    visitOp(item, context, "objAdd")

def visitSub(item, context):
    visitOp(item, context, "objSub")

def visitMul(item, context):
    visitOp(item, context, "objMul")

def visitDiv(item, context):
    visitOp(item, context, "objDiv")

def visitMod(item, context):
    visitOp(item, context, "objMod")
