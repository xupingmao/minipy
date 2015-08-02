class Lib:
    def __init__(self, name, path, load = 1):
        self.name = name
        self.path = path
        self.load = load

# built-in functions.
_libs = [
    Lib("builtins", "libs/object/builtins.py"),
    Lib("dis", "libs/tools/dis.py", 1),
    Lib("printast", "libs/tools/printast.py", 1), 
    Lib("repl", "libs/tools/repl.py"),
    Lib("pyeval", "libs/tools/pyeval.py"),
    Lib("test", "libs/tools/test.py")
]

def loadlib(path, name = None):
    'for modules which can not import by import statement'
    if path in __modules__:
        return __modules__[path]
    code = compilefile(path)
    if name == None:
        name = path
    m = load_module(name, code)
    __modules__[path] = m
    return m

add_builtin("loadlib", loadlib)

def reload():
    for item in _libs:
        if item.load:
            code = compilefile(item.path)
            load_module(item.name, code)

add_builtin('reload', reload)

FILE_SEP = '\\'

def split_path(path):
    cached_pathes = []
    name = ''
    for c in path:
        if c == '/' or c == '\\':
            cached_pathes.append(name)
            name = ''
        else:
            name += c
    if name != '':cached_pathes.append(name)
    return cached_pathes

# join a list like ['home', 'usr'] to a dir "home/usr"
# also handle pathes like 'home/usr/../proc' to "home/proc"
def join_path(path_list):
    if len(path_list) == 0: return ""
    else: lastdir = path_list.pop()
    path = ''
    cleanpath = []
    for item in path_list:
        if item == '..':
            cleanpath.pop()
        elif item == '.':
            pass
        else:
            cleanpath.append(item)
    for item in cleanpath:
        path += item + FILE_SEP
    return path + lastdir


def resolvepath(path):
    if not ('/' in path or '\\' in path):
        return path
    fs1 = split_path(getcwd())
    fs2 = split_path(path)
    fname = fs2.pop()
    parent = join_path(fs1+fs2)
    chdir(parent)
    return fname
        
    
    

def _execute_file(fname):
    from encode import compilefile
    fname = resolvepath(fname)
    _code = compilefile(fname)
    load_module(fname, _code, '__main__')
    
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

def boot():
    from tokenize import *
    from encode import compilefile
    for item in _libs:
        if item.load:
            loadlib(item.path, item.name)
    argc = len(ARGV)
    if argc == 0:
        repl = loadlib("libs/tools/repl.py").repl
        repl()
    elif argc == 1:
        if ARGV[0] == '-help':
            print_usage()
        else:
            _execute_file(ARGV[0])
    elif argc > 1:
        opt = ARGV[0]
        name = ARGV[1]
        if opt == '-tk':
            tokenize0(name)
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
            ast = loadlib('libs/tools/printast.py').printast
            ast(load(name))
        elif opt == '-transform':
            trans = loadlib('libs/tools/transform.py').main
            tran(ARGV)
        elif opt == '-dis-bf':
            dis_func = loadlib('libs/tools/dis.py').dis_func
            dis_func(__builtins__[name])
        else:
            # normal execute file
            _execute_file(ARGV[0])