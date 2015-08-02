_PRINT_STR_LEN = 40

def replPrint(p, n = 0, depth = 2):
    if depth <= 0:
        print("...")
        return
    if gettype(p) == 'dict':
        if depth == 1:
            printf("%s{...}\n", n * ' ')
            return
        print("{")
        for key in p:
            printf(' '*(n+1)+str(key))
            printf(":")
            replPrint(p[key], n+1, depth-1)
        print(' '*n+"}")
    elif gettype(p) == 'list':
        if depth == 1:
            printf("%s[...]\n", n * ' ')
            return
        printf("%s[\n", n * ' ')
        for item in p:
            #printf((n+1) * ' ')
            replPrint(item, n+1, depth-1)
            #printf(",")
        printf('%s]\n', n * ' ')
    elif gettype(p) == "string":
        printf(' ' * n)
        if len(p) > _PRINT_STR_LEN:
            p = p.substring(0, _PRINT_STR_LEN)
            p += '...'
        p = escape(p)
        printf("\"%s\"\n", p)
    else:
        printf(' '*n)
        print(p)

def repl():
    from parse import parse
    printast = loadlib("libs/tools/printast.py").printast
    pyeval = loadlib("libs/tools/pyeval.py").pyeval
    dis = loadlib("libs/tools/dis.py")
    from encode import compile
    from tokenize import tokenize
    print("Welcome To TinyMatrix!!!")
    
    global debug
    debug = 0

    g = {}
    g.update(globals())
    g['g'] = g
    g.update(dis)
    g.pyeval = pyeval
    g.printast = printast

    while 1:
        x = input(">>> ")
        if x != '':
            try:
                v = pyeval(x, g, g['debug'])
                if v != None:
                    replPrint(v)
            except Exception as e:
                print(e)
if __name__ == '__main__':
    repl()