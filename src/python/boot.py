# -*- coding:utf-8 -*-
# @author xupingmao
# @since 2016
# @modified 2020/10/02 08:57:56


# CPython运行环境使用
# minipy 不依赖这个工具

import sys
import struct
import os
import shutil
import time

ARGV = sys.argv
argv = sys.argv
system = os.system

def _repl_print(p, maxlen = 40, n = 0, depth = 2):
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
        if len(p) > maxlen:
            p = p.substring(0, maxlen)
            p += '...'
        p = escape(p)
        printf("\"%s\"\n", p)
    else:
        printf(' '*n)
        print(p)

def repl_print(p, n, depth):
    return _repl_print(p, 40, n, depth)

def mmatch(s, i, v):
    return s[i:i+len(v)] == v

def require(path, name):
    raise "require not implemented"
    
def do_nothing(*args):
    pass
    
def apply(fn, args):
    return fn(*args)

def clock():
    return time.time() * 1000
    
def copy(src, des):
    shutil.copyfile(src, des)

def sformat(fmt, *v):
    return fmt % tuple(v)
    
class Obj(dict):
    
    def __setattr__(self, k, v):
        self[k] = v

    def __getattr__(self, k):
        return self.get(k)

class Tk:
    def __repr__(self):
        return str(self.__dict__)
    def __str__(self):
        return str(self.__dict__)

# newobj
def newobj():
    return Tk()

def add_builtin(name, func):
    if isinstance(__builtins__, dict):
        #print 'dict'
        __builtins__[name] = func
    else:
        #print 'module'
        setattr(__builtins__, name, func)

def asctime():
    return time.asctime()

def printf(s, *nargs):
    if len(nargs) == 0:
        sys.stdout.write(str(s))
    else:
        sys.stdout.write(s % nargs)
    
const_list = []
def get_const_idx(v):
    #print v,
    if v not in const_list:
        const_list.append(v)
    return const_list.index(v) # const_list[0] should be None
def get_const_list():
    return const_list
    
def get_const_len():
    return len(const_list)
    
def get_const(i):
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
    fp = open(name, "r", encoding="utf-8")
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
    elif type == "object":
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

def code_f(value):
    return struct.pack('d', value)

def code(type, val):
    if istype(val, "string"):
        return chr(type) + code16(len(val))+ val
    elif istype(val, "number"):
        return chr(type) + code_f(val)
        
        
add_builtin("add_builtin", add_builtin)
add_builtin('require', require)
add_builtin('add_obj_method', do_nothing)
add_builtin('istype', istype)
add_builtin("newobj", newobj)
add_builtin("asctime", asctime)
add_builtin("mmatch", mmatch)

if __name__ == '__main__':
    file = sys.argv[1]
    loadlib(file)











