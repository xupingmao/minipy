from encode import *
from boot import *
from tmcode import *

def code_str(s):
    return code32(len(s))+s

class Lib:
    def __init__(self, name, path):
        self.name = name
        self.path = path

def build(cc="tcc", libs=None):
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
    for obj in libs:
        code = compilefile(obj.path)
        destCode += code_str(obj.name)+code_str(code)
    code = modLen + code_str("constants") + code_str(build_const_code()) + destCode
    save("../bin.c", "unsigned char bin[] = {" + ','.join(str_to_chars(code))+'};\n')
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
    chararray = []
    for c in code:
        chararray.append(str(ord(c)))
    return chararray

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
    
def main():
    argc = len(ARGV)
    if argc == 2:
        cc = ARGV[1]
        build_one(cc)
    else:
        printf("usage: %s [ccompiler]\n", ARGV[0])
    
if __name__ == '__main__':
    main()