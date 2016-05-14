
def visitArg(item, context):
    ref = item.first.val
    context.push(ref, sformat("Object %s=argTakeObj(\"test\");", ref));