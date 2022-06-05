# -*- coding:utf-8 -*-
# 这个版本通过字节码来生成C语言的代码，相比于通过语法树解析，逻辑要简单很多
# @author xupingmao <578749341@qq.com>
# @since 2020/10/18 00:32:28
# @modified 2022/06/05 17:04:51
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

    OP_OR:  "obj_or",
    OP_AND: "obj_and",

    OP_GET: "obj_get",
    OP_EQEQ: "obj_EQEQ",
    OP_NOTEQ: "obj_not_eq",
    
    OP_LT: "obj_LT",
    OP_LTEQ: "obj_LTEQ",

    OP_GT: "obj_GT",
    OP_GTEQ: "obj_GTEQ",
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
        self.py_entry_func = "init_%s" % self.name
        # 当前的Python函数名
        self.py_func_name = None
        # 当前的Python类名
        self.py_class_name = None

    def indent(self):
        pass

    def writeline(self, line):
        if self.is_in_func:
            self.func_buf.append(["str", line])
        else:
            self.line_buf.append(["str", line])

    def write_debug_opcode(self, op, val):
        op_name = opcodes[op]
        self.writeline("  // %s %r" % (op_name, val))
        return
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

    def emit_op_def(self, val):
        if self.py_class_name is None:
            self.emit_op_def_func(val)
        else:
            self.emit_op_def_method(val)

    def emit_op_def_method(self, val):
        func_name = "%s:%s" % (self.py_class_name, val)
        func_name_full = "method_%s_%s" % (self.py_class_name, val)
        self.writeline("  func = mp2c_def_func(module, \"%s\", %s);" % 
            (func_name,func_name_full))
        self.writeline("  MP2C_PUSH(func);")

        # 进入函数的作用域
        self.enter_func()
        self.writeline("")
        line = "static MpObj {}() {{".format(func_name_full)
        self.writeline(line)
        self.writeline("  MpObj locals[256];")
        self.writeline("  MpObj op_stack[256];")
        self.writeline("  MpObj *top = op_stack;")
        self.py_func_name = func_name

    def emit_op_def_func(self, val):
        func_name = val
        func_name_full = "func_{}".format(func_name)
        self.writeline("  func = mp2c_def_func(module, \"%s\", %s);" % 
            (func_name,func_name_full))
        self.writeline("  MP2C_PUSH(func);")

        # 进入函数的作用域
        self.enter_func()
        self.writeline("")
        line = "static MpObj {}() {{".format(func_name_full)
        self.writeline(line)
        self.writeline("  MpObj locals[256];")
        self.writeline("  MpObj op_stack[256];")
        self.writeline("  MpObj *top = op_stack;")
        self.py_func_name = func_name

    def emit_op_func_end(self):
        self.writeline("  return NONE_OBJECT;")
        self.writeline("  /* EOF: end of function(%s) */" % self.py_func_name)
        self.writeline("}")
        self.exit_func()

    def emit_op_rot(self, val):
        """
        case OP_ROT: {
            MpObj* left = top - cache->v.ival + 1;
            MpObj* right = top;
            for (; left < right; left++, right--) {
                MpObj temp = *left;
                *left = *right;
                *right = temp;
            }
            break;
        }
        """
        self.writeline("  { /** OP_ROT **/")
        self.writeline("    MpObj* left = top - %s + 1;" % val)
        self.writeline("    MpObj* right = top;")
        self.writeline("    for (; left < right; left++, right--) {")
        self.writeline("      MpObj temp = *left; *left = *right; *right = temp;")
        self.writeline("    }")
        self.writeline("  }")

    def emit_op_unpack(self, val):
        """
        case OP_UNPACK: {
            x = MP_POP();
            mp_assert_type(x, TYPE_LIST, "mp_eval: UNPACK");
            int j;
            for(j = LIST_LEN(x)-1; j >= 0; j--) {
                MP_PUSH(LIST_GET(x, j));
            }
            break;
        }
        """
        self.writeline("  {/** OP_UNPACK **/")
        self.writeline("    MpObj x = MP2C_POP();")
        self.writeline("    mp_assert_type(x, TYPE_LIST, \"mp_eval: UNPACK\");")
        self.writeline("    int j;")
        self.writeline("    for(j = LIST_LEN(x)-1; j>=0; j--) {")
        self.writeline("      MP2C_PUSH(LIST_GET(x,j));")
        self.writeline("    }")
        self.writeline("  }")

    def emit_op_set(self):
        """
        case OP_SET:{
            MpObj* key = top;
            MpObj* obj = top-1;
            MpObj* value = top-2;
            top-=3;
            obj_set(*obj, *key, *value);
            break;
        }
        """

        self.writeline("  {")
        self.writeline("  MpObj* key = top;")
        self.writeline("  MpObj* obj = top-1;")
        self.writeline("  MpObj* value = top-2;")
        self.writeline("  top-=3;")
        self.writeline("  obj_set(*obj, *key, *value);")
        self.writeline("  }")

    def emit_op_append(self):
        """
        case OP_APPEND:
            v = MP_POP();
            x = MP_TOP();
            mp_assert(IS_LIST(x), "mp_eval: OP_APPEND require list");
            list_append(GET_LIST(x), v);
            break;
        """
        self.writeline("  {")
        self.writeline("  MpObj v = MP2C_POP();")
        self.writeline("  MpObj x = *top;")
        self.writeline("  mp_assert(IS_LIST(x), \"mp_eval: OP_APPEND require list\");")
        self.writeline("  list_append(GET_LIST(x), v);")
        self.writeline("  }")

    def emit_op_slice(self, val):
        """
        case OP_SLICE: {
            MpObj second = MP_POP();
            MpObj first  = MP_POP();
            *top = obj_slice(*top, first, second);
            break;
        }
        """
        self.writeline("  {")
        self.writeline("  MpObj second = MP2C_POP();")
        self.writeline("  MpObj first  = MP2C_POP();")
        self.writeline("  *top = obj_slice(*top, first, second);")
        self.writeline("  }")


    def emit_op_class(self, val):
        """
        case OP_CLASS: {
            MpObj class_name = cache->v.obj;
            MpObj clazz = class_new(class_name);
            dict_set0(GET_DICT(globals), class_name, clazz);
            break;
        }
        """
        self.writeline("  { /* OP_CLASS */")
        self.writeline("    MpObj class_name = %s;" % self.get_const_var(val))
        self.writeline("    MpObj clazz = class_new(class_name);")
        self.writeline("    obj_set(globals, class_name, clazz);")
        self.writeline("  }")
        self.py_class_name = val

    def emit_op_class_end(self):
        self.py_class_name = None

    def emit_op_import(self, val):
        if val == 1:
            self.writeline("  obj_import(globals, MP2C_POP());")
        else:
            self.writeline("  obj_import_attr(globals, *(top-1), *top);")
            self.writeline("  top -= 2;")

    def emit_op_next(self, op_index, val):
        self.writeline("  {")
        self.writeline("    MpObj *next = obj_next(*top);")
        self.writeline("    if (next != NULL) { MP2C_PUSH(*next); }")
        self.writeline("    else {")
        self.goto_label(op_index + val, 6)
        self.writeline("    }")
        self.writeline("  }")

    def emit_op_setjump(self, val):
        """
        case OP_SETJUMP: { 
            f->last_top = top; 
            f->cache_jmp = cache + cache->v.ival;
            break; 
        }
        """
        self.writeline("  /* OP_SETJUMP */")
        self.writeline("""  printf("OP_SETJUMP\n");""")

    def emit_op_clr_jump(self, val):
        self.writeline("  /* OP_CLR_JUMP */")
        self.writeline("""  printf("OP_CLR_JUMP\n");""")

    def emit_op_load_ex(self):
        self.writeline("  MP2C_PUSH(tm->ex);")

    def gen_py_entry_start(self):
        buf = []
        buf.append("MpObj %s() {" % self.py_entry_func)
        buf.append("  MpObj fname = string_const(\"%s\");" % self.name)
        # 入口的__name__默认为__main__
        buf.append("  MpObj module_name = string_const(\"__main__\");")
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
            buf.append("MpObj %s; /* %r */" % (name, key))
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
        # buf.append("#include \"include/mp.h\"")
        # buf.append("#include \"mp2c.h\"")
        buf.append("#include <include/mp.h>")
        buf.append("#include <mp2c.h>")

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

    # 由于历史原因，大部分代码的转换逻辑在`convert`函数里面
    # 后面新增的比较复杂的转换逻辑抽象出了`CodeWriter`的方法
    # 方法名的约定为`emit_op_?`

    for op, val in inst_list:
        op_index += 1
        writer.define_label(op_index)
        writer.write_debug_opcode(op, val)

        # print(op, val)
        if op == OP_FILE:
            writer.writeline("  MpObj module_file = string_new(\"%s\");" % val)
            module_name = val.replace(".", "_")
            writer.module_name = module_name
        elif op == OP_DEF:
            writer.emit_op_def(val)
            func_name = writer.py_func_name
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
        elif op == OP_NONE:
            writer.writeline("  MP2C_PUSH(NONE_OBJECT);")
        elif op == OP_LIST:
            writer.writeline("  MP2C_PUSH(list_new(2));")
        elif op == OP_DICT:
            writer.writeline("  MP2C_PUSH(dict_new());")
        elif op == OP_APPEND:
            writer.emit_op_append()
        elif op == OP_SLICE:
            writer.emit_op_slice(val)
        elif op == OP_CALL or op == OP_TAILCALL:
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
        elif op == OP_JUMP_ON_FALSE:
            writer.writeline("  if (!is_true_obj(MP2C_TOP())) {");
            writer.goto_label(op_index + val, 4)
            writer.writeline("  }");
        elif op == OP_RETURN:
            writer.writeline("  return MP2C_POP();")
        elif op in OP_FUNC_MAP:
            func_name = OP_FUNC_MAP.get(op)
            do_convert_op(writer, op, val, func_name)
        elif op == OP_SET:
            writer.emit_op_set()
        elif op == OP_NOT:
            writer.writeline("*top = number_obj(!is_true_obj(*top));")
        elif op == OP_DEF_END:
            writer.emit_op_func_end()
        elif op == OP_STORE_GLOBAL:
            var_name = writer.get_const_var(val)
            writer.writeline("  obj_set(globals, %s/*%s*/, MP2C_POP());" % (var_name, val))
        elif op == OP_LOAD_GLOBAL:
            var_name = writer.get_const_var(val)
            writer.writeline("  MP2C_PUSH(mp2c_load_global(globals, {}/*{}*/));"
                .format(var_name, val))
        elif op == OP_EOP:
            writer.writeline("  /* EOP */")
        elif op == OP_IMPORT:
            writer.emit_op_import(val)
        elif op == OP_ITER:
            writer.writeline("  *top = iter_new(*top);")
        elif op == OP_NEXT:
            writer.emit_op_next(op_index, val)
        elif op == OP_UP_JUMP:
            writer.goto_label(op_index - val, 4)
        elif op == OP_ROT:
            writer.emit_op_rot(val)
        elif op == OP_CLASS:
            writer.emit_op_class(val)
        elif op == OP_CLASS_END:
            writer.emit_op_class_end()
        elif op == OP_UNPACK:
            writer.emit_op_unpack(val)
        elif op == OP_SETJUMP:
            writer.emit_op_setjump(val)
        elif op == OP_CLR_JUMP:
            writer.emit_op_clr_jump(val)
        elif op == OP_LOAD_EX:
            writer.emit_op_load_ex()
        else:
            op_str = opcodes[op]
            message = "Unknown opcode, key={}, val={}".format(op_str, val)
            raise message

    writer.end()

def do_benchmark(fpath, target):
    print("")
    print("Test File: %s" % fpath)
    print("-" * 50)
    print(">>> Run with minipy")
    os.system("./minipy %r" % fpath)
    
    print("")
    print("-" * 50)
    print(">>> Run with %s" % target)
    os.system("./%s" % target)

    print("")
    print("-" * 50)
    print(">>> Run with python3")
    os.system("python3 %r" % fpath)


class AotCompiler:

    def __init__(self, debug = False):
        self._debug = debug

    def do_build(self, c_file_path, target):
        print("building %s ..." % c_file_path)
        print("target: %s" % target)
        
        build_cmd = "gcc -O2 %r -o %r -lm -Isrc -Imp2c" % (c_file_path, target)
        print("build command:", build_cmd)
        result = os.system(build_cmd)
        if result == 0:
            print("build success!")
        else:
            print("build failed")
            sys.exit(1)


    def get_base_name(self, fpath):
        fname = fpath.split("/")[-1]
        name = fname.split(".")[0]
        return name

    def get_default_target(self, fpath):
        name = self.get_base_name(fpath)
        return "build/%s.out" % name

    def compile(self, fpath, target = None):
        code = load(fpath)    
        name = self.get_base_name(fpath)
        writer = CodeWriter(code, name)
        convert(code, writer)
        result_code = writer.get_code()
        # print(result_code)

        if target == None:
            target = self.get_default_target(fpath)

        c_file_path = "build/%s.c" % name

        save(c_file_path, result_code)

        if self._debug:
            print("saved to %s" % c_file_path)

        self.do_build(c_file_path, target)


def main():
    argv = sys.argv
    # print(argv)

    if len(argv) == 2:
        fpath = argv[1]
    elif len(argv) == 1:
        fpath = "test/mp2c/fib.py"
    else:
        raise Exception("参数异常")

    compiler = AotCompiler(True)
    compiler.compile(fpath)

    do_benchmark(fpath, target)


if __name__ == '__main__':
    main()
