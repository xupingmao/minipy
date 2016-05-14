
def toCodeStr(val):
    return '"' + escape(val) + '"'

def getPrefix(fname):
    v = fname.split(".")[0]
    v = v.replace("-", "_")
    return v

def _genCode(codeList):
    code = ""
    for kv in codeList:
        name, value = kv
        if name == '#temp':
            if len(value) > 0:
                for temp in value:
                    code += sformat('Object %s;\n', temp)
        else:
            code += value +"\n"
    return code


class CodeWriter:

    def __init__(self):
        self.strings = []

    def write(self, value):
        self.strings.append(value)

class Context:

    def __init__(self):
        self.stack = []
        self.regs = []
        self.scope = "global"
        self.globals = []
        self.strings = []
        self.numbers = []
        self.globalTempList = []
        self.localTempList = []
        self.functionDict = {}
        self.prefix = "_"
        self.fname = "None"
        
    def switchToScope(self, scope):
        self.scope = scope
        if scope == "local":
            self.localTempList = []
            self.push('#temp', self.localTempList)

    def setFname(self, fname):
        self.fname = fname
        self.prefix = getPrefix(fname)

    def push(self, ref, value):
        self.stack.append([ref, value])

    def pop(self):
        return self.stack.pop()

    def store(self, name):
        if self.scope == "global":
            ref, val = self.pop()
            varName = self.getString(name)
            # varRef = self.getVar(name)
            if ref == None:
                ref = self.getTemp(val)
            # self.push(varRef, sformat("%s=%s;", varRef, val))
            self.push(None, sformat('objSet(%s,%s,%s);', self.getGlobals(), varName, ref))
        else:
            ref, val = self.pop()
            varName = self.getVar(name)
            self.push(varName, sformat("%s=%s;", varName, val))

    def getTempSize(self):
        return len(self.regs)

    def restoreTempSize(self, size):
        while len(self.regs) > size:
            self.regs.pop()
            
    def addToTempList(self, temp):
        if self.scope == "global":
            list = self.globalTempList
        else:
            list = self.localTempList
        if temp not in list:
            list.append(temp)

    def getTemp(self, value=None):
        n = len(self.regs)
        self.regs.append(n)
        temp = "_t" + str(n)
        if value != None:
            self.push(temp, temp + "=" + str(value)+";")
        self.addToTempList(temp)
        return temp

    def freeTemp(self):
        pass

    def getVar(self, name):
        if self.scope == "global":
            name = self.prefix + "G" + name
            # self.push(name, )
            return name
        # if name not in self.globals:
        #     self.globals.append(name)
        return self.prefix + "G" + name
        
    def getGlobals(self):
        return self.prefix + "0";

    def getString(self, name):
        if name not in self.strings:
            self.strings.append(name)
        return self.prefix + "S" + str(self.strings.index(name))

    def getNumber(self, name):
        if name not in self.numbers:
            self.numbers.append(name)
        return self.prefix + "N" + str(self.numbers.index(name))

    def defineFunction(self, name, codeList):
        if name in self.functionDict:
            raise sformat("function %s already exists!", name)
        self.functionDict[name] = codeList

    def genVarDef(self):
        code  = '#include"include/tm.h"\n'
        code += '#define TM_NO_BIN 1\n'
        code += '#include "vm.c"\n'
        
        code += "/* definition begin */\n"
        for string in self.strings:
            code += "Object " + self.getString(string) + ";\n"

        for name in self.globals:
            code += "Object " + self.getVar(name) + ";\n"

        for name in self.numbers:
            code += "Object " + self.getNumber(name) + ";\n"

        # globals definition
        code += "Object " + self.getGlobals() + ";\n"

        code += "/* definition end */\n"
        return code

    def genFuncDef(self):
        code = ""
        for name in self.functionDict:
            codeList = self.functionDict[name]
            code += _genCode(codeList)

        return code


    def getCode(self):
        code = ''

        code += self.genVarDef()

        code += self.genFuncDef()

        code += sformat("void %s_main() {\n", self.prefix)
        # code += "int main(int argc, char* argv[]) {\n"
        for string in self.strings:
            code += self.getString(string) + sformat("=stringNew(\"%s\");\n", escape(string))

        # definition of numbers
        for num in self.numbers:
            code += self.getNumber(num) + sformat("=tmNumber(%s);\n", num)

        # temp variables
        for temp in self.globalTempList:
            code += sformat('Object %s;\n', temp)
        
        # globals init
        code += sformat('%s = dictNew();\n', self.getGlobals());
        
        # code
        code += _genCode(self.stack)

        code += "}\n"

        code += "int main(int argc, char* argv[]) {\n"
        code += sformat("tmRunFunc(argc, argv, %s, %s_main);\n", toCodeStr(self.fname), self.prefix);
        code += "}\n";
        return code
