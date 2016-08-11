
def visit_op(item, context, op):
    visit_item(item.first, context)
    visit_item(item.second, context)
    name2, v2 = context.pop()
    name1, v1 = context.pop()
    if name1 == None:
        name1 = context.get_temp(v1)
    if name2 == None:
        name2 = context.get_temp(v2)
    context.push(None, sformat("%s(%s, %s)", op, name1,name2))

def visit_add(item, context):
    visit_op(item, context, "obj_add")

def visit_sub(item, context):
    visit_op(item, context, "obj_sub")

def visit_mul(item, context):
    visit_op(item, context, "obj_mul")

def visit_div(item, context):
    visit_op(item, context, "obj_div")

def visit_mod(item, context):
    visit_op(item, context, "obj_mod")


