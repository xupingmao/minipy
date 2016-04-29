def visitName(item, context):
    if context.scope == "global":
        # ref = context.getVar(item.val)
        context.push(None, sformat("tmGetGlobal(%s, %s)", context.getGlobals(), context.getString(item.val)))
    else:
        context.push(item.val, item.val)