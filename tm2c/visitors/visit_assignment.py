def visit_assignment(item, context):
    # temp_size = context.get_temp_size()
    
    first = item.first
    second = item.second
    if istype(first, 'list'):
        # multiple assignment
        assert len(first) == len(second)
        for i in range(len(first)):
            node = AstNode('=')
            node.first = first[i]
            node.second = second[i]
            visit_assignment(node, context)
    elif first.type == "name":
        visit_item(item.second, context)
        name = first.val
        # ref, val = context.pop()
        # context.push(name, sformat("%s=%s", name, val))
        context.store(name)
        # context.restore_temp_size(temp_size)
    else:
        # handle set, multi-assignment, etc.
        pass

