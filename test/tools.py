PATH = 'libs'

def print_usage():
    printf("options:\n")
    printf("%-10s%-10s:print help\n", "-help", "")
    printf("%-10s%-10s:execute file\n", "", "[file]")
    printf('%-10s%-10s:tokenize file\n', "-tk", "[file]")
    printf("%-10s%-10s:pause after file executed\n", "-p", "[file]")
    printf("%-10s%-10s:disassemble file\n","-dis", "[file]")
    printf("%-10s%-10s:compile source code and save to file\n", "-dump", "[file]")
    printf("%-10s%-10s:print abstract syntax tree\n", "-printast", "[file]")
    printf("%-10s%-10s:debug source code\n", "-debug", "[file]")
    printf("%-10s%-10s:disassemble builtin-func\n", "-dis-bf", "[file]")
    
argc = len(ARGV)
if argc == 1:
    if ARGV[0] == '-help':
        print_usage()
    else:
        print_usage()
elif argc > 2:
    opt = ARGV[1]
    name = ARGV[2]
    if opt == '-tk':
        from tokenize import *
        r = tokenize(load(name))
        for i in r:
            printf("%s := %s\n", i.type, i.val)
    elif opt == '-src':
        printSource(name)
    elif opt == '-p':
        _execute_file(name)
        input("press any key to quit")
    elif opt == '-dis':
        from dis import dissimple
        argv = ARGV.clone()
        del argv[0]
        dissimple(argv)
    elif opt == '-dump':
        compilefile(name, name + '.bin')
    elif opt == '-ast':
        ast = loadlib('printast.py').printast
        ast(load(name))
    elif opt == '-transform':
        trans = loadlib('transform.py').main
        tran(ARGV)
    elif opt == '-dis-bf':
        dis_func = loadlib('dis.py').dis_func
        dis_func(__builtins__[name])
    else:
        pass    # normal execute file
        


    
printf("")

   
    