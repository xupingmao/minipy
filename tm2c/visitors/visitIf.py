
def visitIf (item, context):
    # { first : condition, second : body, third : rest }
    condition = item.first
    body = item.second
    rest = item.third

    visitItem(condition, context)

    ref, val = context.pop()

    if ref == None:
        ref = context.getTemp(val)

    context.push (None, sformat("if (isTrue(%s)) { ", ref))

    visitItem(body, context)

    context.push (None, "}")

    if rest != None:
        context.push(None, "else { ")
        visitItem(rest, context)
        context.push(None, "}")

