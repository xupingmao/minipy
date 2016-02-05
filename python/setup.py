from encode import *
from boot import *
from tmcode import *

def code_str(s):
    return code32(len(s))+s

class Lib:
    def __init__(self, name, path):
        self.name = name
        self.path = path

def build(cc="tcc", libs=None, dstPath = "../bin.c"):
    destCode = ""
    if libs == None:
        libs = [
            Lib("init", "init.py"),
            Lib("tokenize", "tokenize.py"), 
            Lib("parse", "parse.py"), 
            Lib("tmcode", "tmcode.py"),
            Lib("asm", "asm.py"), 
            Lib("encode", "encode.py")
        ]
        #Lib("builtins", "builtins.py"),
        #Lib("dis", "dis.py"),
        #Lib("printast", "printast.py"),
        #Lib("repl", "repl.py"),
        #Lib("pyeval", "pyeval.py")]
    modLen = code32(len(libs)+1) # constants
    dstMtime = mtime(dstPath)
    modified = False
    for obj in libs:
        path = obj.path
        if mtime(path) > dstMtime:
            modified = True
            break
    if modified:
        for obj in libs:
            try:
                code = compilefile(obj.path)
            except Exception as e:
                print("parse file", obj.path, "failed")
                raise
            destCode += code_str(obj.name)+code_str(code)
        code = modLen + code_str("constants") + code_str(build_const_code()) + destCode
        save(dstPath, "unsigned char bin[] = {\n" + str_to_chars(code)+'\n};\n')
    export_clang_define("../include/instruction.h", "tmcode.py")
    if cc != None:
        if str(1.0) != '1.0':
            cmd = cc + " -o tm0.exe ../vm.c"
            if exists("tm0.exe"):
                remove("tm0.exe")
        else:
            cmd = cc + " -o tm.exe ../vm.c"
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
    for i in range(getConstLen()):
        v = getConst(i)
        t = chr(NEW_STRING)
        if gettype(v) == 'number':t = chr(NEW_NUMBER); v = str(v)
        b+=t+code16(len(v))+v
    return b+code8(TM_EOP)+code16(0)

def build_one(cc):
    ccompiler = cc
    libs = [
            Lib("init", "init.py"),
            Lib("tokenize", "tokenize.py"), 
            Lib("parse", "parse.py"), 
            Lib("tmcode", "tmcode.py"),
            Lib("asm", "asm.py"), 
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
    build(ccompiler, libs, "../initbin.c")
    
def main():
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