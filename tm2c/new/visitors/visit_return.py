def visit_return(item, context):
    visit_item(item.first, context)
    ref, val = context.pop()
    context.push(None, "return " + val + ";")

