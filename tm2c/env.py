


def getValidCodeName(fname):
    fname = fname.replace("-", "_")
    fname = fname.replace(".", "_")
    return fname
        

class AstNode:
    def __init__(self, type, first = None, second = None):
        self.type = type
        self.first = first
        self.second = second
        
class Scope:
    def __init__(self, prev):
        self.vars = []
        self.gVars = []
        self.prev = prev
        self.temp = 0
    def addGlobalVar(self, name):
        if name not in self.gVars:
            self.gVars.append(name)
        
class Env:
    def __init__(self, fname, prefix = None):
        self.consts = []
        self.scope = Scope(None)
        self.globalScope = self.scope
        self.indent = 0
        self.fname = fname
        self.func_defines = []
        self.func_cnt = 0
        if prefix == None:
            self.prefix = getValidCodeName(fname)
        else:
            self.prefix = getValidCodeName(prefix) 
    
    def newScope(self):
        scope = Scope(self.scope)
        self.scope = scope
        
    def exitScope(self, func_define):
        if self.scope != None:
            self.scope = self.scope.prev
        else:
            raise "fatal error"
        self.func_defines.append(func_define)
        self.func_cnt += 1
        
    def getCFuncName(self):
        return self.prefix + "F" + str(self.func_cnt) 

    def getModName(self):
        return self.fname
            
    def getConst(self, val):
        if val not in self.consts:
            self.consts.append(val)
        return self.prefix+ "C" + str(self.consts.index(val))
        
    def getVarName(self, name):
        return self.prefix + "V" + name

    def getTempVarName(self):
        temp = new_temp(self)
        return self.prefix + "V" + temp

    # you must declare temp var in your scope
    def getTempPtrName(self):
        temp = new_temp(self)
        return self.prefix + "P" + temp
        
    def locals(self):
        if self.scope != None:
            return self.scope.vars
            
    def getGlobals(self):
        return self.prefix + "0g";
            
    def hasVar(self, name):
        scope = self.scope
        while scope != None:
            if name in scope.vars:
                return True
            scope = scope.prev
        return False
        
    def defVar(self, name):
        if self.hasVar(name):
            pass
        else:
            self.scope.vars.append(name)

    def defGlobalVar(self, name):
        self.scope.addGlobalVar(name)

    def isGlobalVar(self, name):
        return self.scope == self.globalScope or \
            name in self.scope.gVars