
def visitNumber(item, context):
    ref = context.getNumber(item.val)
    context.push(ref, ref)

def visitString(item, context):
    ref = context.getString(item.val)
    context.push(ref, ref)

def visitNone(item, context):
    context.push("NONE_OBJ", "NONE_OBJ")

    