from libs import encode, boot
from libs.boot import *
compilefile = encode.compilefile
from libs.tmcode import *

def move(src, des):
    v = load(src)
    save(des, v)
def copy(src, des):
    v = load(src)
    save(des, v)

def build_bin():
    dest = build_dest()
    dest_to_bin_c(dest)
    
def build0():
    build_bin()
    system("python libs/tmcode.py")
    if not exists("../include/instruction.h") or \
        mtime("instruction.h") > mtime("../include/instruction.h"):
        copy("instruction.h", "../include/instruction.h")
    
    
def clean_temp():
    # copy("tm.exe", "../test/tm.exe")
    pass

def build_gcc():
    build0()
    system("gcc ../vm.c -o tm.exe")
    clean_temp()

def build_tcc():
    build0()
    system("tcc ../vm.c -o tm.exe")
    clean_temp()

def build_gcc_debug():
    build0()
    system("gcc -g ../vm.c -o tm.exe")
    clean_temp()
    
def code_str(s):
    return code32(len(s))+s

class Lib:
    def __init__(self, name, path):
        self.name = name
        self.path = path

def build_dest():
    destCode = ""
    coreList = [
        Lib("init0", "libs/init0.py"),
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
    modLen = code32(len(coreList)+1) # constants
    for obj in coreList:
        code = compilefile(obj.path)
        destCode += code_str(obj.name)+code_str(code)
    return modLen + code_str("constants") + code_str(build_const_code()) + destCode
    
def str_to_chars(code):
    chararray = []
    for c in code:
        chararray.append(str(ord(c)))
    return chararray
def dest_to_bin_c(code):
    save("../bin.c", "unsigned char bin[] = {" + ','.join(str_to_chars(code))+'};\n')

def load_core():
    lst = ["repl", "_boot", "tokenize", "expression",
                 "parse","tmcode", "codegen", "encode", 'printast', "dis"]
    dest = ''
    for name in lst:
        dest += compilefile(name+".py")
    save('core', dest)
    
def build_const_code():
    list = getConstList()
    b = ''
    for v in list:
        t = chr(NEW_STRING)
        if istype(v,'number'):t = chr(NEW_NUMBER); v = str(v)
        b+=t+code16(len(v))+v
    return b+code8(TM_EOP)+code16(0)
    
def main():
    argc = len(ARGV)
    if argc > 1:
        opt = ARGV[1]
        if opt == 'gcc':build_gcc()
        elif opt == 'gccdebug':build_gcc_debug()
        elif opt == 'core':load_core()
        elif opt == 'one':buildtcc_one()
        else:build_tcc()
    else:
        build_tcc()
if __name__ == '__main__':main()