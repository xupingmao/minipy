/*
* @author xupingmao<578749341@qq.com>
* @generated by Python
* @date Fri Feb  4 22:59:51 2022
*/
#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_
#define OP_IMPORT 1
#define OP_STRING 2
#define OP_NUMBER 3
#define OP_ADD 4
#define OP_SUB 5
#define OP_MUL 6
#define OP_DIV 7
#define OP_MOD 8
#define OP_NEG 9
#define OP_NOT 10
#define OP_GT 11
#define OP_LT 12
#define OP_GTEQ 13
#define OP_LTEQ 14
#define OP_EQEQ 15
#define OP_NOTEQ 16
#define OP_IN 17
#define OP_NOTIN 18
#define OP_AND 19
#define OP_OR 20
#define OP_SET 21
#define OP_GET 22
#define OP_SLICE 23
#define OP_NONE 24
#define OP_STORE_LOCAL 25
#define OP_STORE_GLOBAL 26
#define OP_LOAD_LOCAL 27
#define OP_LOAD_GLOBAL 28
#define OP_CONSTANT 29
#define OP_POP 30
#define OP_LIST 31
#define OP_APPEND 32
#define OP_DICT 33
#define OP_DICT_SET 34
#define OP_JUMP 35
#define OP_UP_JUMP 36
#define OP_POP_JUMP_ON_FALSE 37
#define OP_JUMP_ON_FALSE 38
#define OP_JUMP_ON_TRUE 39
#define OP_UNPACK 40
#define OP_ROT 41
#define OP_DEL 42
#define OP_FOR 43
#define OP_NEXT 44
#define OP_ITER 45
#define OP_LOAD_EX 46
#define OP_SETJUMP 47
#define OP_CALL 48
#define OP_TAILCALL 49
#define OP_APPLY 50
#define OP_DEF 51
#define OP_RETURN 52
#define OP_DEF_END 53
#define OP_CLASS 54
#define OP_CLASS_SET 55
#define OP_CLASS_END 56
#define OP_LOAD_PARAMS 57
#define OP_LOAD_NARG 58
#define OP_LOAD_PARG 59
#define OP_CLR_JUMP 60
#define OP_EOP 61
#define OP_DEBUG 62
#define OP_LINE 63
#define OP_TAG 64
#define OP_FAST_ST_GLO 65
#define OP_FAST_LD_GLO 66
#define OP_FILE 67
#define OP_PROFILE 68
#endif

const char* inst_get_name_by_code(int code) {
  switch(code) {
    case 1:return "OP_IMPORT";
    case 2:return "OP_STRING";
    case 3:return "OP_NUMBER";
    case 4:return "OP_ADD";
    case 5:return "OP_SUB";
    case 6:return "OP_MUL";
    case 7:return "OP_DIV";
    case 8:return "OP_MOD";
    case 9:return "OP_NEG";
    case 10:return "OP_NOT";
    case 11:return "OP_GT";
    case 12:return "OP_LT";
    case 13:return "OP_GTEQ";
    case 14:return "OP_LTEQ";
    case 15:return "OP_EQEQ";
    case 16:return "OP_NOTEQ";
    case 17:return "OP_IN";
    case 18:return "OP_NOTIN";
    case 19:return "OP_AND";
    case 20:return "OP_OR";
    case 21:return "OP_SET";
    case 22:return "OP_GET";
    case 23:return "OP_SLICE";
    case 24:return "OP_NONE";
    case 25:return "OP_STORE_LOCAL";
    case 26:return "OP_STORE_GLOBAL";
    case 27:return "OP_LOAD_LOCAL";
    case 28:return "OP_LOAD_GLOBAL";
    case 29:return "OP_CONSTANT";
    case 30:return "OP_POP";
    case 31:return "OP_LIST";
    case 32:return "OP_APPEND";
    case 33:return "OP_DICT";
    case 34:return "OP_DICT_SET";
    case 35:return "OP_JUMP";
    case 36:return "OP_UP_JUMP";
    case 37:return "OP_POP_JUMP_ON_FALSE";
    case 38:return "OP_JUMP_ON_FALSE";
    case 39:return "OP_JUMP_ON_TRUE";
    case 40:return "OP_UNPACK";
    case 41:return "OP_ROT";
    case 42:return "OP_DEL";
    case 43:return "OP_FOR";
    case 44:return "OP_NEXT";
    case 45:return "OP_ITER";
    case 46:return "OP_LOAD_EX";
    case 47:return "OP_SETJUMP";
    case 48:return "OP_CALL";
    case 49:return "OP_TAILCALL";
    case 50:return "OP_APPLY";
    case 51:return "OP_DEF";
    case 52:return "OP_RETURN";
    case 53:return "OP_DEF_END";
    case 54:return "OP_CLASS";
    case 55:return "OP_CLASS_SET";
    case 56:return "OP_CLASS_END";
    case 57:return "OP_LOAD_PARAMS";
    case 58:return "OP_LOAD_NARG";
    case 59:return "OP_LOAD_PARG";
    case 60:return "OP_CLR_JUMP";
    case 61:return "OP_EOP";
    case 62:return "OP_DEBUG";
    case 63:return "OP_LINE";
    case 64:return "OP_TAG";
    case 65:return "OP_FAST_ST_GLO";
    case 66:return "OP_FAST_LD_GLO";
    case 67:return "OP_FILE";
    case 68:return "OP_PROFILE";
  }
  return "UNKNOWN";
}