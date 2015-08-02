
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
            raise('fail to compile file "%s.py":\n\t%s'.format(fname, e))
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

def init():
    glo = {}
    _import("libs/init", glo, '*')
    glo.boot()
