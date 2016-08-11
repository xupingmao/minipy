
def visit_arg(item, context):
    ref = item.first.val
    context.push(ref, sformat("Object %s=arg_take_obj(\"test\");", ref));

