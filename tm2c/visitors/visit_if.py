
def visit_if (item, context):
    # { first : condition, second : body, third : rest }
    condition = item.first
    body = item.second
    rest = item.third

    visit_item(condition, context)

    ref, val = context.pop()

    if ref == None:
        ref = context.get_temp(val)

    context.push (None, sformat("if (is_true(%s)) { ", ref))

    visit_item(body, context)

    context.push (None, "}")

    if rest != None:
        context.push(None, "else { ")
        visit_item(rest, context)
        context.push(None, "}")



