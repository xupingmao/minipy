

'''
tools for tinyvm to bootstrap
'''

def _import(fname, des_glo, tar = None):
    'this _import function can not prevent import circle'
    if fname in __modules__:
        pass
    else:
        from encode import compilefile
        try:
            #__modules__[fname] = {}
            _code = compilefile(fname + '.py' )
        except Exception as e:
            #del __modules__[fname]
            raise(sformat('fail to compile file "%s.py":\n\t%s', fname, e))
        load_module(fname, _code)
    g = __modules__[fname]
    if tar == '*':
        for k in g:
            # _ stands for private variable
            # and #N is a special variable created by tinyvm compiler
            if k[0] != '_' and k[0] != '#':
                des_glo[k] = g[k]
    else:
        des_glo[tar] = g[tar]


def newobj():
    return {}

def hasattr(obj, name):
    return name in obj

def Exception(e):
    return e

def add_builtin(name, func):
    __builtins__[name] = func

add_builtin("add_builtin", add_builtin)
add_builtin("Exception", Exception)
add_builtin("hasattr", hasattr)
add_builtin("_import", _import)
add_builtin("newobj", newobj)

class Lib:
    def __init__(self, name, path, load = 1):
        self.name = name
        self.path = path
        self.load = load

# built-in functions.
#_libs = [
    #Lib("builtins", "libs/builtins.py"),
    #Lib("dis", "libs/tools/dis.py", 1),
    #Lib("printast", "libs/tools/printast.py", 1), 
    #Lib("repl", "libs/tools/repl.py"),
    #Lib("pyeval", "libs/tools/pyeval.py"),
    #Lib("test", "libs/tools/test.py")
#]

def require(path, name = None):
    'for modules which can not import by import statement'
    if path in __modules__:
        return __modules__[path]
    code = compilefile(path)
    if name == None:
        name = path
    m = load_module(name, code)
    __modules__[path] = m
    return m

add_builtin("require", require)

def reload():
    for item in _libs:
        if item.load:
            code = compilefile(item.path)
            load_module(item.name, code)

add_builtin('reload', reload)

if getosname() == "nt":
    FILE_SEP = '\\'
else:
    FILE_SEP = '/'

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
        
    
    

def _execute_file(path):
    from encode import compilefile
    fname = resolvepath(path)
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

def boot(loadlibs=True):
    from tokenize import *
    from encode import compilefile
    from repl import repl
    argc = len(ARGV)
    
    if argc == 0:
        repl()
    else:
        _execute_file(ARGV[0])
    return
    if argc == 1:
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
            ast = loadlib('printast.py').printast
            ast(load(name))
        elif opt == '-transform':
            trans = loadlib('transform.py').main
            tran(ARGV)
        elif opt == '-dis-bf':
            dis_func = loadlib('dis.py').dis_func
            dis_func(__builtins__[name])
        else:
            # normal execute file
            _execute_file(ARGV[0])