def visit_name(item, context):
    if context.scope == "global":
        # ref = context.get_var(item.val)
        context.push(None, sformat("tm_get_global(%s, %s)", context.get_globals(), context.get_string(item.val)))
    else:
        context.push(item.val, item.val)

