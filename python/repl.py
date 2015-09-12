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

def remove_consts(g):
    idx = []
    for k in g:
        if gettype(g[k]) in ('string', 'number'):
            idx.append(k)
    #print(idx)
    for k in idx:
        #print('del ' + str(k))
        del g[k]

def getmem():
    return str(getVmInfo().alloc_mem / 1024) + ' kb'
    
def repl():
    from parse import parse
    from encode import compile
    from tokenize import tokenize
    from pyeval import pyeval
    print("Welcome To TinyMatrix!!!")
    
    global debug
    debug = 0

    g = {}
    g.update(globals())
    g['g'] = g
    remove_consts(g)
    while 1:
        x = input(">>> ")
        if x != '':
            try:
                if x == 'exit':
                    break
                if 'debug' not in g:
                    g['debug'] = 0
                v = pyeval(x, g, g['debug'])
                if v != None:
                    replPrint(v)
            except Exception as e:
                print(e)
if __name__ == '__main__':
    repl()