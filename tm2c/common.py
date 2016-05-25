
def get_line_no(item):
    if hasattr(item, "pos"):
        return item.pos[0]
    elif hasattr(item, "first"):
        return get_line_no(item.first)


_handlers = {
    "number": visit_number,
    "name":   visit_name,
    "string": visit_string,
    "None": visit_none,
    "=" : visit_assignment,
    "+" : visit_add,
    "if": visit_if,
    "call": visit_call,
    "def": visit_def,
    "arg": visit_arg,
    "return": visit_return
}

def visit_item0(item, context):
    _handlers[item.type](item, context)

def visit_item(item, context):
    context.item = item
    if istype(item, "list"):
        for item0 in item:
            visit_item(item0, context)
    else:
        visit_item0(item, context)

def visit_item_list(item_list, context):
    for item in item_list:
        temp_size = context.get_temp_size()
        visit_item(item, context)
        context.restore_temp_size(temp_size)


def tm2c(fname, src, prefix=None):
    tree = parse(src)
    # print_ast(tree)
    context = Context()
    context.set_fname(prefix)
    try:
        visit_item_list(tree, context)
    except Exception as e:
        traceback()
        print_ast(context.item)
    return context.get_code()
    
if __name__ == "__main__":
    name = sys.argv[1]
    # path = "../test/tm2c/" + name
    path = name
    text = load(path)
    pathes = path.split('/')
    if len(pathes) > 1:
        name = pathes[-1]
    mod = name.split(".")[0]
    code = tm2c(name, text, mod)
    print(code)


