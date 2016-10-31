from encode import *
from boot import *
from tmcode import *
import sys

def code_str(s):
    return code32(len(s))+s

class Lib:
    def __init__(self, name, path):
        self.name = name
        self.path = path

def build(cc="tcc", libs=None, dst_path = "bin.c"):
    dest_code = ""
    if libs == None:
        libs = [
            Lib("init", "init.py"),
            Lib("tokenize", "tokenize.py"), 
            Lib("parse", "parse.py"), 
            Lib("tmcode", "tmcode.py"),
            Lib("encode", "encode.py")
        ]
        #Lib("builtins", "builtins.py"),
        #Lib("dis", "dis.py"),
        #Lib("printast", "printast.py"),
        #Lib("repl", "repl.py"),
        #Lib("pyeval", "pyeval.py")]
    mod_len = code32(len(libs)+1) # constants
    if not exists(dst_path):
        dst_mtime = -1
    else:
        dst_mtime = mtime(dst_path)
    modified = False
    for obj in libs:
        path = obj.path
        if mtime(path) > dst_mtime:
            modified = True
            break
    if modified:
        for obj in libs:
            try:
                code = compilefile(obj.path)
            except Exception as e:
                print("parse file", obj.path, "failed")
                raise
            dest_code += code_str(obj.name)+code_str(code)
        code = mod_len + code_str("constants") + code_str(build_const_code()) + dest_code
        save(dst_path, "unsigned char bin[] = {\n" + str_to_chars(code)+'\n};\n')
    export_clang_define("include/instruction.h", "tmcode.py")
    if cc != None:
        # tm itself
        if "tm" in globals():
            if cc == "tcc":
                cmd = cc + " -o tm0.exe main.c"
            else:
                cmd = cc + " -o tm0.exe main.c -lm"
            if exists("tm0.exe"):
                remove("tm0.exe")
        else:
            if cc == "tcc":
                cmd = cc + " -o tm.exe main.c"
            else:
                cmd = cc + " -o tm.exe main.c -lm"
        system(cmd)
        #remove("../bin.c")
    
def str_to_chars(code):
    dest = ''
    for i in range(len(code)):
        if i != 0:
            dest += ','
        if (i+1) % 9 == 0:
            dest += '\n'
        dest += str(ord(code[i]))
    return dest

def build_const_code():
    b = ''
    for i in range(get_const_len()):
        v = get_const(i)
        t = chr(OP_STRING)
        if gettype(v) == 'number':t = chr(OP_NUMBER); v = str(v)
        b+=t+code16(len(v))+v
    return b+code8(OP_EOP)+code16(0)

def build_one(cc):
    ccompiler = cc
    libs = [
            Lib("init", "init.py"),
            Lib("tokenize", "tokenize.py"), 
            Lib("parse", "parse.py"), 
            Lib("tmcode", "tmcode.py"),
            Lib("encode", "encode.py"),
            Lib("pyeval", "pyeval.py"),
            Lib("repl", "repl.py"),
            #Lib("dis.py", "dis.py"),
            #Lib("printast.py", "printast.py")
        ]
    build(ccompiler, libs)

def build_tm2c(cc):
    ccompiler = cc
    libs = [ Lib("init", "init.py") ]
    build(ccompiler, libs, "initbin.c")
    
def main():
    ARGV = sys.argv
    argc = len(ARGV)
    if argc == 2:
        cc = ARGV[1]
        build_one(cc)
    elif argc == 3 and ARGV[1] == "-init":
        cc = ARGV[2]
        build_tm2c(cc)
    else:
        printf("usage: %s [ccompiler]\n" + 
               "       %s -init [ccompiler]\n" , ARGV[0], ARGV[0])
    
if __name__ == '__main__':
    main()
