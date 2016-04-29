def visitAssignment(item, context):
    # tempSize = context.getTempSize()
    
    first = item.first
    second = item.second
    if istype(first, 'list'):
        # multiple assignment
        assert len(first) == len(second)
        for i in range(len(first)):
            node = AstNode('=')
            node.first = first[i]
            node.second = second[i]
            visitAssignment(node, context)
    elif first.type == "name":
        visitItem(item.second, context)
        name = first.val
        # ref, val = context.pop()
        # context.push(name, sformat("%s=%s", name, val))
        context.store(name)
        # context.restoreTempSize(tempSize)
    else:
        # handle set, multi-assignment, etc.
        pass