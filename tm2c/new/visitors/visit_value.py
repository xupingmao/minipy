
def visit_number(item, context):
    ref = context.get_number(item.val)
    context.push(ref, ref)

def visit_string(item, context):
    ref = context.get_string(item.val)
    context.push(ref, ref)

def visit_none(item, context):
    context.push("NONE_OBJ", "NONE_OBJ")

    

