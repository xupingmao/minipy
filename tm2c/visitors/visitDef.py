
def visitDef(item, context):
    # visitItem(item.first, context)
    name = context.getVar(item.first.val)
    # ref, name = context.pop()
    context.push(None, "Object " + name + "() {\n")
    context.scope = "local"
    visitItem(item.second, context)
    visitItem(item.third, context)
    context.push(None, "}\n")
    context.scope = "global"