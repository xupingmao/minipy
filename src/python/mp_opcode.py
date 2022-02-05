# -*- coding:utf-8 -*-
# stack based opcodes
# @author xupingmao
# @since 2016
# @modified 2022/02/04 23:00:30

"""使用说明
在`minipy`的根目录执行

minipy> python3 src/python/mp_opcode.py
"""

from boot import *
cheader = '''/*
* @author xupingmao<578749341@qq.com>
* @generated by Python
* @date %s
*/
#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_
'''
ctail = "\n#endif\n\n"

_opcode_names = [
    "OP_IMPORT",
    # basic types
    'OP_STRING', 
    'OP_NUMBER',
    
    'OP_ADD', 'OP_SUB', 'OP_MUL', 'OP_DIV', 'OP_MOD', 'OP_NEG',
    'OP_NOT', 'OP_GT', 'OP_LT', 'OP_GTEQ', 'OP_LTEQ', 'OP_EQEQ', 'OP_NOTEQ', 'OP_IN', 'OP_NOTIN',
    'OP_AND', 'OP_OR', 'OP_SET', 
    'OP_GET',
    'OP_SLICE',

    'OP_NONE', 'OP_STORE_LOCAL', 'OP_STORE_GLOBAL', 'OP_LOAD_LOCAL', 'OP_LOAD_GLOBAL', 'OP_CONSTANT',
    #'LOAD_GLOBALS', 
    'OP_POP', 

    # list
    'OP_LIST', 
    'OP_APPEND',

    # dict
    'OP_DICT', 
    'OP_DICT_SET',
    
    'OP_JUMP', 'OP_UP_JUMP', 'OP_POP_JUMP_ON_FALSE', 'OP_JUMP_ON_FALSE', 'OP_JUMP_ON_TRUE',
    
    # TAGSIZE
    'OP_UNPACK', 'OP_ROT', 'OP_DEL', 'OP_FOR', 'OP_NEXT', 'OP_ITER', 'OP_LOAD_EX',
    'OP_SETJUMP', 
    
    # 函数调用相关
    'OP_CALL', 
    'OP_TAILCALL',
    # apply(a, b)
    'OP_APPLY', 
    'OP_DEF', 
    'OP_RETURN',
    # end of function
    'OP_DEF_END',
    
    # class opcodes
    'OP_CLASS', 
    'OP_CLASS_SET',
    'OP_CLASS_END',

    'OP_LOAD_PARAMS', 
    'OP_LOAD_NARG', 
    "OP_LOAD_PARG",

    "OP_CLR_JUMP",

    # end of program
    'OP_EOP',
    
    # mulity assignment, eg. x,y = 1,2
    'OP_DEBUG',
    'OP_LINE',
    

    'OP_TAG', 
    # instructions for vm to optimize.
    'OP_FAST_ST_GLO',
    'OP_FAST_LD_GLO',

    'OP_FILE',

    # 专门用于性能分析的字节码
    'OP_PROFILE',
    # unused instructions
    #'LT_JUMP_ON_FALSE', 
    #'GT_JUMP_ON_FALSE',
    #'LTEQ_JUMP_ON_FALSE',
    #'GTEQ_JUMP_ON_FALSE',
    #'EQEQ_JUMP_ON_FALSE',
    #'NOTEQ_JUMP_ON_FALSE',
]

# update global values.
i = 0
opcodes = {}
while i < len(_opcode_names):
    name = _opcode_names[i]
    globals()[name] = i + 1
    opcodes[i+1] = name
    i += 1

def build_get_name_by_code():
    i = 0
    codes = []
    codes.append("const char* inst_get_name_by_code(int code) {")
    codes.append("  switch(code) {")
    while i < len(_opcode_names):
        op_name  = _opcode_names[i]
        op_value = i+1
        codes.append("    case %s:return \"%s\";" % (op_value, op_name))
        i += 1

    codes.append("  }");
    codes.append("  return \"UNKNOWN\";")
    codes.append("}");
    return "\n".join(codes)

def export_clang_define(des, self = None):
    # check modified time
    # if src is modified before des and cdes, do not convert.
    if self == None:
        self = ARGV[0]
    if not exists(des):
        pass
    elif mtime(self) < mtime(des):
        return
    defines = []
    i = 0
    while i < len(_opcode_names):
        x = _opcode_names[i]
        defines.append('#define '+ x + ' ' + str(i+1))
        i += 1
    ctext = cheader % str(asctime()) + '\n'.join(defines) + ctail
    ctext += build_get_name_by_code()
    save(des, ctext)
    # save('../include/instruction.h', ctext)

if __name__ == '__main__':
    export_clang_define('./src/include/instruction.h')
