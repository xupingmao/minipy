def visitAssignment(item, context):
    # tempSize = context.getTempSize()
    visitItem(item.second, context)
    first = item.first
    if first.type == "name":
        name = first.val
        # ref, val = context.pop()
        # context.push(name, sformat("%s=%s", name, val))
        context.store(name)
        # context.restoreTempSize(tempSize)
    else:
        # handle set, multi-assignment, etc.
        pass