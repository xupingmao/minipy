#!python
# -*- coding:utf-8 -*-
# @author xupingmao
# @since 2016
# @modified 2020/09/30 16:49:29
from mp_encode import *
from boot import *
from mp_opcode import *
import sys

if "exists" not in globals():
    import os
    exists = os.exists

def code_str(s):
    return code32(len(s))+s

class Lib:
    def __init__(self, name, path):
        self.name = name
        self.path = path
        
__libs = [
    Lib("mp_init", "mp_init.py"),
    Lib("mp_tokenize", "mp_tokenize.py"), 
    Lib("mp_parse",  "mp_parse.py"), 
    Lib("mp_opcode", "mp_opcode.py"),
    Lib("mp_encode", "mp_encode.py"),
    Lib("pyeval", "pyeval.py"),
    Lib("repl", "repl.py"),
]

__compiler_files = [
    Lib("boot", "boot.py"),
    Lib("mp_lex",    "mp_lex.py"),
    Lib("mp_parse",  "mp_parse.py"),
    Lib("mp_encode", "mp_encode.py"),
]
        
def build(cc="tcc", libs=None, dst_path = "../bin.c"):
    dest_code = ""
    if libs == None:
        libs = __libs
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
                raise(e)
            dest_code += code_str(obj.name)+code_str(code)
        code = mod_len + code_str("constants") + code_str(build_const_code()) + dest_code
        print("generate bin.c ...")
        save(dst_path, "unsigned char bin[] = {\n" + str_to_chars(code)+'\n};\n')
    else:
        print("file not modified, bye")
        return
    print("generate instruction.h ...")
    export_clang_define("../include/instruction.h", "mp_opcode.py")
    print("build successfull!")
    """
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
    """
    
def str_to_chars(code):
    width = 20 - 1
    dest = ''
    for i in range(len(code)):
        if i != 0:
            dest += ','
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

def build_cfiles(cc="gcc"):
    ccompiler = cc
    libs = __libs
    build(ccompiler, libs)

def build_tm2c(cc):
    ccompiler = cc
    libs = [ Lib("init", "init.py") ]
    build(ccompiler, libs, "initbin.c")
    
def build_single_py():
    lines = []
    
    for lib in __compiler_files:
        path = lib.path
        fp = open(path)
        state = 1
        for line in fp.readlines():
            if "# MP_TEST_END" in line:
                state = 1
                continue
            if "# MP_TEST" in line:
                # 测试内容
                state = 0
                continue
            if state == 1:
                # 正常内容
                lines.append(line)
    result = "".join(lines)
    
    save("compiler.py", result)
    
def build_single_c(filename, build_map=None):
    import re
    if build_map == None:
        build_map = {}
    path = os.path.abspath(filename)
    if path in build_map:
        return ""
    
    build_map[path] = 1
    
    text = load(filename)
    lines = []
    p = re.compile(' *#include *"(.*)"')
    dirname = os.path.dirname(filename)
    
    for line in text.split("\n"):
        if p.match(line):
            libname = p.findall(line)[0]
            libpath = os.path.join(dirname, libname)
            line = build_single_c(libpath, build_map)
        lines.append(line)
    return "\n".join(lines)
    
def print_usage():
    print("build.py          -- build `bin.c` and `instruction.h`")
    print("build.py compiler -- build a standalone compiler")
    
def build_mini_c(pyfilename):
    out = []
    if exists("vm.c"):
        text = load("vm.c")
    else:
        text = build_single_c("../vm.c")
    code = compilefile(pyfilename)
    dest_code = code_str("main") + code_str(code)
    mod_len = code32(2)
    code = mod_len + code_str("constants") + code_str(build_const_code()) + dest_code
    c_array_def = "unsigned char bin[] = {\n" + str_to_chars(code) + "\n};\n";
    text += "\n" + c_array_def
    
    text += '''
int main(int argc, char *argv[])
{
    /* start vm with bin */
    return mp_run(argc, argv, bin);
}
    '''
    return text
    
    
    
def main():
    argc = len(sys.argv)
    argv = sys.argv
    if argc == 1:
        build_cfiles()
        return
    elif argc == 2:
        arg1 = argv[1]
        # build a standalone compiler
        if arg1 == "compiler":
            build_single_py()
            return
    elif argc == 3:
        arg1 = argv[1]
        arg2 = argv[2]
        if arg1 == "-c":
            text = build_single_c(arg2)
            print(text)
            return
        elif arg1 == "-mc":
            text = build_mini_c(arg2)
            print(text)
            return
    print_usage()
            
if __name__ == '__main__':
    main()
