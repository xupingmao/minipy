
def visit_call(item, context):
    first = item.first
    second = item.second
    visit_item(first, context)
    func_ref, val = context.pop()
    if func_ref == None:
        func_ref = context.get_temp(val)
    if istype(second, "list"):
        arg_list = []
        for item in second:
            visit_item(item, context)
            ref, val = context.pop()
            if ref == None:
                ref = context.get_temp(val)
            arg_list.append(ref)
        context.push(None, sformat("tm_call(%s,%s,%s,%s);", \
            get_line_no(item), func_ref, len(arg_list), ",".join(arg_list)))
    elif second == None:
        context.push(None, sformat("tm_call(%s,%s,0);", get_line_no(item), func_ref))
    else:
        visit_item(second, context)
        ref, val = context.pop()
        if ref == None:
            ref = context.get_temp(val)
        context.push(None, sformat("tm_call(%s,%s,1,%s);", get_line_no(item), func_ref, ref))


