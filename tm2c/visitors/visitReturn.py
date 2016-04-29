def visitReturn(item, context):
    visitItem(item.first, context)
    ref, val = context.pop()
    context.push(None, "return " + val + ";")