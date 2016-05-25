
def to_code_str(val):
    return '"' + escape(val) + '"'

def get_prefix(fname):
    v = fname.split(".")[0]
    v = v.replace("-", "_")
    return v

def _gen_code(code_list):
    code = ""
    for kv in code_list:
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
        self.global_temp_list = []
        self.local_temp_list = []
        self.function_dict = {}
        self.prefix = "_"
        self.fname = "None"
        
    def switch_to_scope(self, scope):
        self.scope = scope
        if scope == "local":
            self.local_temp_list = []
            self.push('#temp', self.local_temp_list)

    def set_fname(self, fname):
        self.fname = fname
        self.prefix = get_prefix(fname)

    def push(self, ref, value):
        self.stack.append([ref, value])

    def pop(self):
        return self.stack.pop()

    def store(self, name):
        if self.scope == "global":
            ref, val = self.pop()
            var_name = self.get_string(name)
            # var_ref = self.get_var(name)
            if ref == None:
                ref = self.get_temp(val)
            # self.push(var_ref, sformat("%s=%s;", var_ref, val))
            self.push(None, sformat('obj_set(%s,%s,%s);', self.get_globals(), var_name, ref))
        else:
            ref, val = self.pop()
            var_name = self.get_var(name)
            self.push(var_name, sformat("%s=%s;", var_name, val))

    def get_temp_size(self):
        return len(self.regs)

    def restore_temp_size(self, size):
        while len(self.regs) > size:
            self.regs.pop()
            
    def add_to_temp_list(self, temp):
        if self.scope == "global":
            list = self.global_temp_list
        else:
            list = self.local_temp_list
        if temp not in list:
            list.append(temp)
            
    def add_comment(self, comment):
        self.push(None, "/* " + comment + "*/")

    def get_temp(self, value=None):
        n = len(self.regs)
        self.regs.append(n)
        temp = "_t" + str(n)
        if value != None:
            self.push(temp, temp + "=" + str(value)+";")
        self.add_to_temp_list(temp)
        return temp

    def free_temp(self):
        pass

    def get_var(self, name):
        if self.scope == "global":
            name = self.prefix + "_" + name
            # self.push(name, )
            return name
        # if name not in self.globals:
        #     self.globals.append(name)
        return self.prefix + "_" + name
        
    def get_globals(self):
        return self.prefix + "0";

    def get_string(self, name):
        if name not in self.strings:
            self.strings.append(name)
        return self.prefix + "S" + str(self.strings.index(name))

    def get_number(self, name):
        if name not in self.numbers:
            self.numbers.append(name)
        return self.prefix + "N" + str(self.numbers.index(name))

    def define_function(self, name, code_list):
        if name in self.function_dict:
            raise sformat("function %s already exists!", name)
        self.function_dict[name] = code_list

    def gen_var_def(self):
        code  = '#include"include/tm.h"\n'
        code += '#define TM_NO_BIN 1\n'
        code += '#include "vm.c"\n'
        
        code += "/* definition begin */\n"
        for string in self.strings:
            code += "Object " + self.get_string(string) + ";\n"

        for name in self.globals:
            code += "Object " + self.get_var(name) + ";\n"

        for name in self.numbers:
            code += "Object " + self.get_number(name) + ";\n"

        # globals definition
        code += "Object " + self.get_globals() + ";\n"

        code += "/* definition end */\n"
        return code

    def gen_func_def(self):
        code = ""
        for name in self.function_dict:
            code_list = self.function_dict[name]
            code += _gen_code(code_list)

        return code


    def get_code(self):
        code = ''

        code += self.gen_var_def()

        code += self.gen_func_def()

        code += sformat("void %s_main() {\n", self.prefix)
        # code += "int main(int argc, char* argv[]) {\n"
        for string in self.strings:
            code += self.get_string(string) + sformat("=string_new(\"%s\");\n", escape(string))

        # definition of numbers
        for num in self.numbers:
            code += self.get_number(num) + sformat("=tm_number(%s);\n", num)

        # temp variables
        for temp in self.global_temp_list:
            code += sformat('Object %s;\n', temp)
        
        # globals init
        code += sformat('%s = dict_new();\n', self.get_globals());
        
        # code
        code += _gen_code(self.stack)

        code += "}\n"

        code += "int main(int argc, char* argv[]) {\n"
        code += sformat("tm_run_func(argc, argv, %s, %s_main);\n", to_code_str(self.fname), self.prefix);
        code += "}\n";
        return code


