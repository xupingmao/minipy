class Context:

    def __init__(self):
        self.stack = []
        self.regs = []
        self.scope = "global"
        self.globals = []
        self.strings = []
        self.prefix = "_"

    def push(self, ref, value):
        self.stack.append([ref, value])

    def pop(self):
        return self.stack.pop()

    def store(self, name):
        if self.scope == "global":
            ref, val = self.pop()
            varName = self.getString(name)
            varRef = self.getVar(name)
            self.push(varRef, sformat("%s=%s;", varRef, val))
            self.push(varRef, sformat('tmSetGlobal(%s, %s);', varName, varRef))

    def getTempSize(self):
        return len(self.regs)

    def restoreTempSize(self, size):
        while len(self.regs) > size:
            self.regs.pop()

    def getTemp(self, value=None):
        n = len(self.regs)
        self.regs.append(n)
        temp = "_t" + str(n)
        if value != None:
            self.push(None, temp + "=" + str(value)+";")
        return temp

    def freeTemp(self):
        pass

    def getVar(self, name):
        if name not in self.globals:
            self.globals.append(name)
        return self.prefix + "G" + name

    def getString(self, name):
        if name not in self.strings:
            self.strings.append(name)
        return self.prefix + "S" + str(self.strings.index(name))

    def getDefinition(self):
        code = '#include"tm.c";\n'
        for string in self.strings:
            code += "Object " + self.getString(string) + ";\n"

        for name in self.globals:
            code += "Object " + self.getVar(name) + ";\n"

        return code


    def getCode(self):
        code = ''

        code += self.getDefinition()

        code += "int main(int argc, char* argv[]) {\n"
        for string in self.strings:
            code += self.getString(string) + sformat("=stringNew(\"%s\");", escape(string)) + "\n"

        for kv in self.stack:
            name, value = kv
            code += value +"\n"

        code += "}\n"
        return code
