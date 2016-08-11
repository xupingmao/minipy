
def visit_def(item, context):
    origin_name = item.first.val
    
    context.push ("##", "#func")
    
    # visit_item(item.first, context)
    name = context.get_var(origin_name)
    # ref, name = context.pop()
    context.push(None, "Object " + name + "() {")
    context.switch_to_scope("local")
    visit_item(item.second, context)
    visit_item(item.third, context)
    context.push(None, "}\n")
    context.switch_to_scope("global")
    
    code = context.pop()
    function_code = []
    
    while not (code[0] == "##" and code[1] == "#func"):
        function_code.append(code)
        code = context.pop()
    function_code.append([None, "/* function " + origin_name + " */"])
    function_code.reverse()
    context.define_function(origin_name, function_code)
    context.push(None, sformat("def_func(%s, %s, %s);",
        context.get_globals(), context.get_string(origin_name), name))

