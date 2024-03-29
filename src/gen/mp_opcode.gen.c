const char* mp_opcode_bin="69#\n"
"63#13\n2#boot\n2#*\n1#2\n"
"63#14\n2#/*\\n* @author xupingmao<578749341@qq.com>\\n* @generated by Python\\n* @date %s\\n*/\\n#ifndef INSTRUCTION_H_\\n#define INSTRUCTION_H_\\n\n26#cheader\n"
"63#22\n2#\\n#endif\\n\\n\n26#ctail\n"
"63#24\n31\n2#OP_IMPORT\n32\n2#OP_STRING\n32\n2#OP_NUMBER\n32\n2#OP_ADD\n32\n2#OP_SUB\n32\n2#OP_MUL\n32\n2#OP_DIV\n32\n2#OP_MOD\n32\n2#OP_NEG\n32\n2#OP_NOT\n32\n2#OP_GT\n32\n2#OP_LT\n32\n2#OP_GTEQ\n32\n2#OP_LTEQ\n32\n2#OP_EQEQ\n32\n2#OP_NOTEQ\n32\n2#OP_IN\n32\n2#OP_NOTIN\n32\n2#OP_AND\n32\n2#OP_OR\n32\n2#OP_SET\n32\n2#OP_GET\n32\n2#OP_SLICE\n32\n2#OP_NONE\n32\n2#OP_STORE_LOCAL\n32\n2#OP_STORE_GLOBAL\n32\n2#OP_LOAD_LOCAL\n32\n2#OP_LOAD_GLOBAL\n32\n2#OP_CONSTANT\n32\n2#OP_POP\n32\n2#OP_LIST\n32\n2#OP_APPEND\n32\n2#OP_DICT\n32\n2#OP_DICT_SET\n32\n2#OP_JUMP\n32\n2#OP_UP_JUMP\n32\n2#OP_POP_JUMP_ON_FALSE\n32\n2#OP_JUMP_ON_FALSE\n32\n2#OP_JUMP_ON_TRUE\n32\n2#OP_UNPACK\n32\n2#OP_ROT\n32\n2#OP_DEL\n32\n2#OP_FOR\n32\n2#OP_NEXT\n32\n2#OP_ITER\n32\n2#OP_LOAD_EX\n32\n2#OP_SETJUMP\n32\n2#OP_CALL\n32\n2#OP_TAILCALL\n32\n2#OP_APPLY\n32\n2#OP_DEF\n32\n2#OP_RETURN\n32\n2#OP_DEF_END\n32\n2#OP_CLASS\n32\n2#OP_CLASS_SET\n32\n2#OP_CLASS_END\n32\n2#OP_LOAD_PARAMS\n32\n2#OP_LOAD_NARG\n32\n2#OP_LOAD_PARG\n32\n2#OP_CLR_JUMP\n32\n2#OP_EOP\n32\n2#OP_DEBUG\n32\n2#OP_LINE\n32\n2#OP_TAG\n32\n2#OP_STORE_GLOBAL_FAST\n32\n2#OP_LOAD_GLOBAL_FAST\n32\n2#OP_SET_FAST\n32\n2#OP_GET_FAST\n32\n2#OP_FILE\n32\n2#OP_PROFILE\n32\n26#_opcode_names\n"
"63#104\n3\n26#i\n"
"63#105\n33\n26#opcodes\n"
"63#106\n28#i\n28#len\n28#_opcode_names\n48#1\n12\n37#27\n"
"63#107\n28#_opcode_names\n28#i\n22\n26#name\n"
"63#108\n28#i\n3#1.0\n4\n28#globals\n48\n28#name\n21\n"
"63#109\n28#name\n28#opcodes\n28#i\n3#1.0\n4\n21\n"
"63#110\n28#i\n3#1.0\n4\n26#i\n36#31\n"
"63#112\n51#build_get_name_by_code\n"
"63#112\n57\n"
"63#113\n3\n25\n"
"63#114\n31\n25#1\n"
"63#115\n27#1\n2#append\n22\n2#const char* inst_get_name_by_code(int code) {\n48#1\n30\n"
"63#116\n27#1\n2#append\n22\n2#  switch(code) {\n48#1\n30\n"
"63#117\n27\n28#len\n28#_opcode_names\n48#1\n12\n37#30\n"
"63#118\n28#_opcode_names\n27\n22\n25#2\n"
"63#119\n27\n3#1.0\n4\n25#3\n"
"63#120\n27#1\n2#append\n22\n2#    case %s:return \"%s\";\n31\n27#3\n32\n27#2\n32\n8\n48#1\n30\n"
"63#121\n27\n3#1.0\n4\n25\n36#34\n"
"63#123\n27#1\n2#append\n22\n2#  }\n48#1\n30\n"
"63#124\n27#1\n2#append\n22\n2#  return \"UNKNOWN\";\n48#1\n30\n"
"63#125\n27#1\n2#append\n22\n2#}\n48#1\n30\n"
"63#126\n2#\\n\n2#join\n22\n27#1\n49#1\n52\n53\n26#build_get_name_by_code\n"
"63#128\n51#export_clang_define\n24\n25#1\n"
"63#128\n57#257\n"
"63#131\n27#1\n24\n15\n37#7\n"
"63#132\n28#ARGV\n3\n22\n25#1\n35#1\n"
"63#133\n28#exists\n27\n48#1\n10\n37#3\n"
"63#134\n35#12\n28#mtime\n27#1\n48#1\n28#mtime\n27\n48#1\n12\n37#4\n24\n52\n35#1\n"
"63#137\n31\n25#2\n"
"63#138\n3\n25#3\n"
"63#139\n27#3\n28#len\n28#_opcode_names\n48#1\n12\n37#29\n"
"63#140\n28#_opcode_names\n27#3\n22\n25#4\n"
"63#141\n27#2\n2#append\n22\n2##define \n27#4\n4\n2# \n4\n28#str\n27#3\n3#1.0\n4\n48#1\n4\n48#1\n30\n"
"63#142\n27#3\n3#1.0\n4\n25#3\n36#33\n"
"63#143\n28#cheader\n28#str\n28#asctime\n48\n48#1\n8\n2#\\n\n2#join\n22\n27#2\n48#1\n4\n28#ctail\n4\n25#5\n"
"63#144\n27#5\n28#build_get_name_by_code\n48\n4\n25#5\n"
"63#145\n28#save\n27\n27#5\n48#2\n30\n53\n26#export_clang_define\n"
"63#148\n28#__name__\n2#__main__\n15\n37#7\n"
"63#149\n28#export_clang_define\n2#./src/include/instruction.h\n48#1\n30\n35#1\n61\n";