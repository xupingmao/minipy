import encode, boot
from boot import *
from tmcode import *
compilefile = encode.compilefile

    
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
            Lib("init0", "init0.py"),
            Lib("builtins", "builtins.py"),
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
            Lib("init", "init.py"),
            Lib("tokenize", "tokenize.py"), 
            Lib("parse", "parse.py"), 
            Lib("tmcode", "tmcode.py"),
            Lib("asm", "asm.py"), 
            Lib("encode", "encode.py"),
            Lib("builtins", "builtins.py"),
            Lib("repl", "repl.py"),
            Lib("pyeval.py", "pyeval.py"),
            #Lib("dis.py", "dis.py"),
            #Lib("printast.py", "printast.py")
        ]
    build(ccompiler, libs)
    clean_temp()

def print_usage():
    print("usage: " + ARGV[0] + " [ccompiler]")
    
def main():
    argc = len(ARGV)
    if argc == 1:
        cc = "tcc"
    elif argc == 2:
        cc = ARGV[1]
    else:
        print_usage()
    build_one()
    
if __name__ == '__main__':main()