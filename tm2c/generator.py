
# tm2c.py

from tokenize import *

class AstNode:
    def __init__(self, type, first = None, second = None):
        self.type = type
        self.first = first
        self.second = second

def getStringDef(value):
    return '"' + escape(str(value)) + '"'

class Generator:

    def __init__(self, env):
        self.env = env


    def process(self, lines):
        
        env = self.env
        defineLines = []
        # assign constants
        # defineLines.append("  " + env.getGlobals() + "=dict_new();");
        defineLines.append(env.getGlobals() + "=dictNew();")
        

        for const in env.consts:
            h = env.getConst(const) + "="
            if gettype(const) == "number":
                body = "tmNumber(" + str(const) + ");"
            elif gettype(const) == "string":
                body = "stringNew(" + getStringDef(str(const)) + ');'
            defineLines.append(h+body)
        defineLines.append("tmDefMod(\"%s\", %s);".format(env.getModName(), env.getGlobals()))
            
        lines = defineLines + lines
            
        head = '#include "tm.c"\n'
        head += "/* DEFINE START */\n"
        # define constants
        for const in env.consts:
            head += "Object " + env.getConst(const) + ";\n";
        head += "Object " + env.getGlobals() + ";\n"
        # do vars
        for var in env.locals():
            head += "Object " + env.getVarName(var) + ";\n"
        head += "/* DEFINE END */\n"
        
        # function define.
        for func_define in env.func_defines:
            head += func_define + "\n"
        
        # global 
        head += "void " + env.prefix + "_main(){\n"
        code =  head + "\n".join(lines)+"\n}\n"
        code += sformat("\nint main(int argc, char* argv[]) {\ntmRunFunc(argc, argv, \"%s\", %s_main);\n}\n", env.getModName(), env.prefix)
        self.code = code

    def getCode(self):
        return self.code

