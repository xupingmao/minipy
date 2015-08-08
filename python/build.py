from libs import encode, boot
from libs.boot import *
compilefile = encode.compilefile
from libs.tmcode import *

    
def clean_temp():
    remove("../bin.c")
    
def code_str(s):
    return code32(len(s))+s

class Lib:
    def __init__(self, name, path):
        self.name = name
        self.path = path

def build(ccompiler="tcc", libs=None):
    destCode = ""
    if libs == None:
        libs = [
            Lib("init0", "libs/init0.py"),
            Lib("builtins", "libs/builtins.py"),
            Lib("tokenize", "libs/tokenize.py"), 
            Lib("parse", "libs/parse.py"), 
            Lib("tmcode", "libs/tmcode.py"),
            Lib("asm", "libs/asm.py"), 
            Lib("encode", "libs/encode.py")
        ]
        #Lib("builtins", "libs/builtins.py"),
        #Lib("dis", "libs/tools/dis.py"),
        #Lib("printast", "libs/tools/printast.py"),
        #Lib("repl", "libs/tools/repl.py"),
        #Lib("pyeval", "libs/tools/pyeval.py")]
    modLen = code32(len(libs)+1) # constants
    for obj in libs:
        code = compilefile(obj.path)
        destCode += code_str(obj.name)+code_str(code)
    code = modLen + code_str("constants") + code_str(build_const_code()) + destCode
    save("../bin.c", "unsigned char bin[] = {" + ','.join(str_to_chars(code))+'};\n')
    export_clang_define("../include/instruction.h", "libs/tmcode.py")
    if ccompiler != None:
        system(ccompiler+ " -o tm.exe ../vm.c")
    
def str_to_chars(code):
    chararray = []
    for c in code:
        chararray.append(str(ord(c)))
    return chararray

def build_const_code():
    list = getConstList()
    b = ''
    for v in list:
        t = chr(NEW_STRING)
        if istype(v,'number'):t = chr(NEW_NUMBER); v = str(v)
        b+=t+code16(len(v))+v
    return b+code8(TM_EOP)+code16(0)

def build_one():
    if len(ARGV) < 3:
        ccompiler = "tcc"
    else:
        ccompiler = ARGV[2]
    libs = [
            Lib("init0", "libs/init0.py"),
            Lib("tokenize", "libs/tokenize.py"), 
            Lib("parse", "libs/parse.py"), 
            Lib("tmcode", "libs/tmcode.py"),
            Lib("asm", "libs/asm.py"), 
            Lib("encode", "libs/encode.py"),
            Lib("init", "libs/init.py"),
            
            Lib("builtins", "libs/builtins.py"),
            Lib("repl", "libs/tools/repl.py"),
            Lib("libs/tools/pyeval.py", "libs/tools/pyeval.py"),
            Lib("libs/tools/dis.py", "libs/tools/dis.py"),
            Lib("libs/tools/printast.py", "libs/tools/printast.py")
        ]
    build(ccompiler, libs)
    
def main():
    argc = len(ARGV)
    if argc > 1:
        opt = ARGV[1]
        if opt == 'gcc':build("gcc")
        elif opt == 'one':build_one()
        elif opt == "compile":build(None)
        else:build("tcc")
    else:
        build("tcc")
    clean_temp()
    
if __name__ == '__main__':main()