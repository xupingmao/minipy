
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
        visitItem(second, context)
        ref, val = context.pop()
        if ref == None:
            ref = context.getTemp(val)
        context.push(None, sformat("tmCall(%s,%s,1,%s);", getLineNo(item), funcRef, ref))
