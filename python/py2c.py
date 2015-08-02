from encode import *

def py2bin(srclist, des):
    if istype(srclist, 'string'):
        srclist = [srclist]
    txt = ""
    for src in srclist:
        code = compile(src)
        txt += "unsigned char " + src.replace(".py", "_pyc")+ "[] = {"
        codes = []
        for c in code:
            codes.append(str(ord(c)))
        txt += ','.join(codes) + "};\n"
    save(des, txt)

opmap = {
ADD:"tm_jit_add",
SUB:"tm_jit_sub",
MUL:"tm_jit_mul",
DIV:"tm_jit_div",
MOD:"tm_jit_mod"
}
oplist = opmap.keys()
stackmap={
LOAD_CONSTANT:"tm_jit_load_const",
LOAD_GLOBAL:"tm_jit_load_global",
LOAD_LOCAL:"tm_jit_load_local",
STORE_GLOBAL:"tm_jit_store_global",
STORE_LOCAL:"tm_jit_store_local",
CALL:"tm_jit_call",
NEW_STRING: "tm_new_string",
NEW_NUMBER: "tm_new_number",
TAGSIZE: "tm_tagsize"
}
stacklist = stackmap.keys()
def gen(ins):
    code,val = ins
    if code in oplist:
        return opmap[code]+"()"
    elif code in stacklist:
        return stackmap[code]+"("+str(val)+")"
    else:
        return ""
def py2c(srclist, des):
    if istype(srclist, 'string'):srclist=[srclist]
    txt = ''
    for src in srclist:
        lst = compile2list(src)
        codes = []
        for ins in lst:
            print(ins)
            codes.append(gen(ins))
        txt += ';\n'.join(codes)
    save(des, txt)
        
        
if __name__ == "__main__":
    py2bin(["_boot.py", "tokenize.py", "expression.py", "parse.py","tmcode.py", "codegen.py", "encode.py", "repl.py"], "compile.c")
    #py2c("_boot.py", "_boot.c")

