# -*- coding:utf-8 -*-
# 这个版本通过字节码来生成C语言的代码，相比于通过语法树解析，逻辑要简单很多
# @author xupingmao <578749341@qq.com>
# @since 2020/10/18 00:32:28
# @modified 2022/01/26 15:35:02
# @version v0.2

import sys
import os
from mp_encode import *
from mp_opcode import *

OP_FUNC_MAP = {
    OP_ADD: "obj_add",
    OP_SUB: "obj_sub",
    OP_MUL: "obj_mul",
    OP_DIV: "obj_div",
    OP_MOD: "obj_mod",
    OP_GET: "obj_get",
    OP_OR:  "obj_or",
    OP_LTEQ: "obj_LTEQ",
    OP_EQEQ: "obj_EQEQ",
}

class CodeWriter:

    def __init__(self, code, name):
        self.code = code
        self.name = name
        self.const_dict = dict()
        self.line_buf = []
        self.func_buf = []
        self.label_dict = {}
        self.is_in_func = False
        self.py_entry_func = "%s_main" % self.name

    def indent(self):
        pass

    def writeline(self, line):
        if self.is_in_func:
            self.func_buf.append(["str", line])
        else:
            self.line_buf.append(["str", line])

    def write_debug_line(self, op, val):
        return
        op_name = opcodes[op]
        self.writeline("  puts(\"%s %r\");" % (op_name, val));

    def write_debug_lineno(self, lineno):
        return
        self.writeline("  tm->mp2c_lineno = %d;" % lineno)

    def goto_label(self, val, indent = 2):
        self.label_dict[val] = 1
        self.writeline(" " * indent + "goto L%s;" % val)

    def define_label(self, label):
        if self.is_in_func:
            buf = self.func_buf
        else:
            buf = self.line_buf

        buf.append(["label", label])

    def enter_func(self):
        self.is_in_func = True

    def exit_func(self):
        self.is_in_func = False

    def end(self):
        pass

    def gen_py_entry_start(self):
        buf = []
        buf.append("MpObj %s() {" % self.py_entry_func)
        buf.append("  MpObj fname = string_const(\"%s\");" % self.name)
        buf.append("  MpObj module_name = fname;")
        buf.append("  module = module_new(fname, module_name, NONE_OBJECT);")
        buf.append("  globals = GET_MODULE(module)->globals;")
        buf.append("  MpObj op_stack[256];")
        buf.append("  MpObj *top = op_stack;")
        buf.append("  MpObj func = NONE_OBJECT;")
        return "\n".join(buf)

    def gen_code_by_buf(self, buf):
        result = []
        for type, value in buf:
            if type == "label":
                if value in self.label_dict:
                    result.append("L%s:" % value)
            else:
                result.append(value)
        return "\n".join(result)

    def gen_const_def_section_code(self):
        buf = []
        for key in self.const_dict:
            name = self.const_dict.get(key)
            buf.append("MpObj %s;" % name)
        return "\n".join(buf)

    def gen_const_init_code(self):
        buf = []
        for key in self.const_dict:
            name = self.const_dict.get(key)
            buf.append("  %s = string_const(\"%s\");" % (name, key))
        return "\n".join(buf)

    def gen_global_section_code(self):
        self.writeline("  return NONE_OBJECT;")
        self.writeline("}")

        buf = []
        buf.append(self.gen_py_entry_start())
        buf.append(self.gen_const_init_code())
        buf.append(self.gen_code_by_buf(self.line_buf))
        return "\n".join(buf)

    def gen_main_entry(self):
        buf = []
        buf.append("")
        buf.append("int main(int argc, char* argv[]) {")
        buf.append("  mp2c_run_func(argc, argv, \"%s\", %s);" % 
            (self.name, self.py_entry_func))
        buf.append("  return 0;")
        buf.append("}")
        return "\n".join(buf)

    def get_code(self):
        const_section = self.gen_const_def_section_code()
        global_section = self.gen_global_section_code()
        func_section = self.gen_code_by_buf(self.func_buf)
        main_entry = self.gen_main_entry()
        buf = []
        buf.append("#include \"../src/include/mp.h\"")
        buf.append("#include \"../mp2c/mp2c.c\"")
        buf.append("static MpObj module;")
        buf.append("static MpObj globals;")
        buf.append(const_section)
        buf.append(func_section)
        buf.append(global_section)
        buf.append(main_entry)
        return "\n".join(buf)

    def get_const_var(self, key):
        name = self.const_dict.get(key)
        if name is None:
            name = "const_%s" % len(self.const_dict)
            self.const_dict[key] = name
        return name


def do_convert_op(writer, op, val, func_name):
    # writer.writeline("  t2 = MP2C_POP();")
    # writer.writeline("  t1 = MP2C_POP();")
    # writer.writeline("  MP2C_PUSH(%s(t1, t2));" % func_name)
    writer.writeline("  top--;")
    writer.writeline("  *top = %s(*top, *(top+1));" % func_name)

def convert(code, writer):
    inst_list = compile_to_list(code, "test.py")
    module_name = None
    func_name   = None
    op_index = 0
    code_list = code.split("\n")

    for op, val in inst_list:
        op_index += 1
        writer.define_label(op_index)
        writer.write_debug_line(op, val)

        # print(op, val)
        if op == OP_FILE:
            writer.writeline("  MpObj module_file = string_new(\"%s\");" % val)
            module_name = val.replace(".", "_")
        elif op == OP_DEF:
            func_name = val
            func_name_full = "{}_{}".format(module_name, func_name)
            t = func_name_full
            writer.writeline("  func = mp2c_def_func(module, \"%s\", %s);" % (func_name,t))
            writer.writeline("  MP2C_PUSH(func);")

            # 进入函数的作用域
            writer.enter_func()
            writer.writeline("")
            line = "MpObj {}() {{".format(func_name_full)
            writer.writeline(line)
            writer.writeline("  MpObj locals[256];")
            writer.writeline("  MpObj op_stack[256];")
            writer.writeline("  MpObj *top = op_stack;")
        elif op == OP_LOAD_PARAMS:
            parg = int(val / 256)
            varg = val % 256
            line = "  /* OP_LOAD_PARAMS(%s) */" % parg
            writer.writeline(line)
            for i in range(parg):
                writer.writeline("  locals[%d] = arg_take_obj(\"%s\");" % (i, func_name))
        elif op == OP_LOAD_PARG:
            for i in range(val):
                writer.writeline("  locals[%d] = arg_take_obj(\"\");" % (i, func_name))
        elif op == OP_LOAD_NARG:
            arg_index = val
            writer.writeline("  MpObj args = list_new(arg_remains());")
            writer.writeline("  while(arg_remains() > 0) {")
            writer.writeline("    obj_append(args, arg_take_obj(\"%s\"));" % (func_name))
            writer.writeline("  }")
            writer.writeline("  locals[%s] = args;" % arg_index)
        elif op == OP_LOAD_LOCAL:
            line = "  MP2C_PUSH(locals[{}]);".format(val)
            writer.writeline(line)
        elif op == OP_STORE_LOCAL:
            line = "  locals[{}] = MP2C_POP();".format(val)
            writer.writeline(line)
        elif op == OP_NUMBER:
            line = "  MP2C_PUSH(number_obj({}));".format(val)
            writer.writeline(line)
        elif op == OP_STRING:
            str_var = writer.get_const_var(val)
            line = "  MP2C_PUSH({});".format(str_var)
            writer.writeline(line)
        elif op == OP_CALL:
            argc = val
            writer.writeline("  top -= %s; /* args */" % argc)
            writer.writeline("  *top = obj_call_nargs(*top, %s, top + 1);" % argc)
        elif op == OP_APPLY:
            writer.writeline("  top--;")
            writer.writeline("  *top = obj_apply(*top, *(top+1));")
        elif op == OP_POP:
            writer.writeline("  MP2C_POP();")
        elif op == OP_JUMP:
            writer.goto_label(op_index + val)
        elif op == OP_LINE:
            index = val - 1
            if index < len(code_list):
                line = code_list[index]
                line = line.rstrip()
            else:
                line = "Unknown"
            writer.write_debug_lineno(val)
            writer.writeline("  /* (line:%03d) %r */" % (val, line))
            # writer.writeline("  printf(\"%%d: %%s\n\", %d, %s);" % (val, line))
            # writer.writeline("L%s:" % val)
        elif op == OP_POP_JUMP_ON_FALSE:
            writer.writeline("  if(!is_true_obj(MP2C_POP())) {")
            writer.goto_label(op_index + val, 4)
            writer.writeline("  }")
        elif op == OP_JUMP_ON_TRUE:
            writer.writeline("  if (is_true_obj(MP2C_TOP())) {");
            writer.goto_label(op_index + val, 4)
            writer.writeline("  }");
        elif op == OP_RETURN:
            writer.writeline("  return MP2C_POP();")
        elif op in OP_FUNC_MAP:
            func_name = OP_FUNC_MAP.get(op)
            do_convert_op(writer, op, val, func_name)
        elif op == OP_EOF:
            writer.writeline("  return NONE_OBJECT;")
            writer.writeline("  /* EOF: end of function */")
            writer.writeline("}")
            writer.exit_func()
        elif op == OP_STORE_GLOBAL:
            var_name = writer.get_const_var(val)
            writer.writeline("  obj_set(globals, %s, MP2C_POP());" % var_name)
        elif op == OP_LOAD_GLOBAL:
            var_name = writer.get_const_var(val)
            writer.writeline("  MP2C_PUSH(mp2c_load_global(globals, {}));".format(var_name))
        elif op == OP_EOP:
            writer.writeline("  /* EOP */")
        elif op == OP_IMPORT:
            if val == 1:
                writer.writeline("  obj_import(globals, MP2C_POP());")
            else:
                writer.writeline("  obj_import_attr(globals, *(top-1), *top);")
                writer.writeline("  top -= 2;")
        else:
            op_str = opcodes[op]
            message = "Unknown opcode, key={}, val={}".format(op_str, val)
            raise message

    writer.end()

def file_basename(fpath):
    fname = fpath.split("/")[-1]
    name = fname.split(".")[0]
    return name

def do_build(c_file_path, target):
    print("building %s ..." % c_file_path)
    os.system("rm build/mp2c")
    result = os.system("gcc -O2 %r -o %r -lm" % (c_file_path, target))
    if result == 0:
        print("build success!")
    else:
        print("build failed")
        sys.exit(1)

def main():
    argv = sys.argv
    # print(argv)

    if len(argv) == 2:
        fpath = argv[1]
    elif len(argv) == 1:
        fpath = "test/mp2c/fib.py"
    else:
        raise Exception("参数异常")

    code = load(fpath)    
    name = file_basename(fpath)
    writer = CodeWriter(code, name)
    convert(code, writer)
    result_code = writer.get_code()
    # print(result_code)

    c_file_path = "build/%s.c" % name
    target = "build/%s.out" % name

    save(c_file_path, result_code)
    print("saved to %s" % c_file_path)

    do_build(c_file_path, target)

    print("run build/mp2c ...")
    print("")
    print("-" * 50)
    print(">>> Run with minipy")
    os.system("./minipy %r" % fpath)
    
    print("")
    print("-" * 50)
    print(">>> Run with mp2c")
    os.system("./%s" % target)


if __name__ == '__main__':
    main()
