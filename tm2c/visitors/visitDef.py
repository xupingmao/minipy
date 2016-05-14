
def visitDef(item, context):

    context.push ("##", "#func")
    originName = item.first.val
    # visitItem(item.first, context)
    name = context.getVar(originName)
    # ref, name = context.pop()
    context.push(None, "Object " + name + "() {")
    context.switchToScope("local")
    visitItem(item.second, context)
    visitItem(item.third, context)
    context.push(None, "}\n")
    context.switchToScope("global")
    
    code = context.pop()

    functionCode = []

    while not (code[0] == "##" and code[1] == "#func"):
        functionCode.append(code)
        code = context.pop()
    functionCode.reverse()
    context.defineFunction(originName, functionCode)
    context.push(None, sformat("defFunc(%s, %s, %s);",
        context.getGlobals(), context.getString(originName), name))