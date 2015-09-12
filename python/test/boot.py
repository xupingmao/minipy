import sys
import struct
import os
import shutil
import time

ARGV = sys.argv
argv = sys.argv
system = os.system

# import loadlib
'''
bootstrap for standard python to go.
'''
def require(path, name):
    raise "require not implemented"
    
def do_nothing(*args):
    pass
    
def clock():
    return time.time() * 1000
    
def copy(src, des):
    shutil.copyfile(src, des)
    
class Obj:
    pass

def newobj():
    return Obj()

def add_builtin(name, func):
    if isinstance(__builtins__, dict):
        #print 'dict'
        __builtins__[name] = func
    else:
        #print 'module'
        setattr(__builtins__, name, func)

def asctime():
    return time.asctime()
add_builtin("asctime", asctime)

def printf(s, *nargs):
    if len(nargs) == 0:
        sys.stdout.write(str(s))
    else:
        sys.stdout.write(s % nargs)
    
const_list = []
def getConstIdx(v):
    #print v,
    if v not in const_list:
        const_list.append(v)
    return const_list.index(v) # const_list[0] should be None
def getConstList():
    return const_list
    
def getConstLen():
    return len(const_list)
    
def getConst(i):
    return const_list[i]
    
def mtime(name):
    return os.path.getmtime(name)
# def makesure(v, expect = None, cur = None):
    # if not v and cur:
        # print('error at ' + str(cur.pos) + ' expect ' + expect + ' but see ' + cur.val)


''' file system utils start'''
def save(name, content):
    fp = open(name, 'w')
    fp.write(content)
    fp.close()

def remove(fname):
    os.remove(fname)
rm = remove

def exists(fname):
    return os.path.exists(fname)

def load(name):
    fp = open(name, "r")
    t = fp.read()
    fp.close()
    return t

def mtime(fname):
    return os.path.getmtime(fname)
''' file system util end '''

def _and(a,b):
    return a and b
    
def _slice(self, start, end):
    return self[start:end]
    
def istype(val,  type):
    if type == 'string':
        return isinstance(val, str)
    elif type == 'number':
        return isinstance(val, int) or isinstance(val, float)
    elif type == 'list':
        return isinstance(val, list) or isinstance(val, tuple)
    elif type == 'dict':
        return isinstance(val, dict)

def gettype(val):
    '''to be different with python builtin function type'''
    if isinstance(val, str):return 'string'
    elif isinstance(val, int) or isinstance(val, float):return 'number'
    elif isinstance(val, list) or isinstance(val, tuple):return 'list'
    elif isinstance(val, dict) :return 'dict'

def short( a, b):
    return (a << 8) + b

def getshort(a, b):
    return (ord(a) << 8) + ord(b)

nextshort = getshort
_code = ''

def uncode16(a, b):
    return (ord(a) << 8) + ord(b)

def substring(str, start, end):
    return str[start:end]
    
def code8(ins):
    return chr(ins)

def code16(ins):
    return chr((ins>>8) & 0xff) + chr(ins & 0xff)

def code32(ins):
    return chr((ins>>20) & 0xff) + chr((ins>>16) & 0xff) + \
        chr((ins>>8) & 0xff) + chr(ins & 0xff)

def codeF(value):
    return struct.pack('d', value)

def code(type, val):
    if istype(val, "string"):
        return chr(type) + code16(len(val))+ val
    elif istype(val, "number"):
        return chr(type) + codeF(val)
        
        
add_builtin("add_builtin", add_builtin)
add_builtin('require', require)
add_builtin('add_obj_method', do_nothing)
add_builtin('istype', istype)

if __name__ == '__main__':
    file = sys.argv[1]
    loadlib(file)