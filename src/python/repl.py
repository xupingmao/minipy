# -*- coding:utf-8 -*-
# @author xupingmao
# @since 2016
# @modified 2020/09/23 01:42:34

import mp_encode

PRINT_STR_LEN = 40
compilefile = mp_encode.compilefile

def repl_print(p, n = 0, depth = 2):
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
            repl_print(p[key], n+1, depth-1)
        print(' '*n+"}")
    elif gettype(p) == 'list':
        if depth == 1:
            printf("%s[...]\n", n * ' ')
            return
        printf("%s[\n", n * ' ')
        for item in p:
            #printf((n+1) * ' ')
            repl_print(item, n+1, depth-1)
            #printf(",")
        printf('%s]\n', n * ' ')
    elif gettype(p) == "string":
        printf(' ' * n)
        if len(p) > PRINT_STR_LEN:
            p = p.substring(0, PRINT_STR_LEN)
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
    return str(get_vm_info().alloc_mem / 1024) + ' kb'

def run(path, args = None):
    bak_argv = ARGV
    if args == None:
        args = [path]
    else:
        args.insert(0, path)
    load_module(path, compilefile(path))
    ARGV = bak_argv
    
def print_help():
    print ()
    print ("to run a python file")
    print (" - minipy filename")
    print ()
    print ("built-in variables")
    print (" - __builtins__  built-in functions")
    print (" - __modules__   avaliable modules")
    print ()
    print ("input exit to quit")
    print ()
    
    
    
def repl():
    import pyeval
    fpyeval = pyeval.pyeval
    print("Welcome To Minipy!!!")
    print("Try 'help' for more information, 'exit' to quit")
    
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
                if x == 'help':
                    print_help()
                    continue
                if 'debug' not in g:
                    g['debug'] = 0
                v = fpyeval(x, g, g['debug'])
                if v != None:
                    repl_print(v)
            except Exception as e:
                traceback()
                # print(e)
if __name__ == '__main__':
    repl()

