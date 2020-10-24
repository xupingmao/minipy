/*
 * mp_mico.h
 *
 *  Created on: 2020/09/21
 *  @author: xupingmao
 *  @modified 2020/10/13 00:04:38
 */

#ifndef MP_MICRO_H_
#define MP_MICRO_H_

/* c micros */
#define strequals(a, b) (a == b || strcmp(a,b) == 0)
#define max(a, b) (a) > (b) ? (a) : (b)


/* macros */
#define GET_STR(obj) (obj).value.str->value
#define GET_SZ(obj)  (obj).value.str->value
#define GET_STR_OBJ(obj) (obj).value.str
#define GET_STR_LEN(obj) (obj).value.str->len


/* list micros */
#define LIST_GET(obj, i) list_get(GET_LIST(obj), i)
#define LIST_NODES(obj) (GET_LIST(obj))->nodes
#define LIST_LEN(obj) GET_LIST(obj)->len


/* function micros */
#define GET_FUNCTION(obj) (obj.value.func)
#define GET_FUNC(obj) ((obj).value.func)
#define IS_FUNCTION(o) MP_TYPE(o)==TYPE_FUNCTION
#define GET_FUNCTIONTION_MODULE_PTR(fnc) GET_MODULE(GET_FUNCTION(fnc)->mod)
#define GET_FUNC_CONSTANTS(fnc) GET_FUNCTIONTION_MODULE_PTR(fnc)->constants
#define GET_FUNC_MOD_NAME(fnc) GET_FUNCTIONTION_MODULE_PTR(fnc)->file
#define GET_FUNC_CONSTANTS_NODES(fnc) LIST_NODES(GET_FUNC_CONSTANTS(fnc))
#define GET_FUNCTION_NAME(fnc) GET_FUNCTION(fnc)->name
#define GET_GLOBALS(func) get_function_globals(GET_FUNCTION(func))
#define FUNCTION_FORMAT(des, func) func_format(des, GET_FUNCTION(func))


#define GET_VAL(obj) (obj).value
#define GET_PTR(obj) (obj).value.ptr
#define GET_DATA(obj) (obj).value.data
#define GET_DATA_PROTO(obj) (obj).value.data->proto
#define GET_DICT(obj) GET_VAL(obj).dict
#define GET_DICT_ATTR(dict, key)  *dict_get_by_str0(GET_DICT(dict), key)
#define GET_MODULE(obj) GET_VAL(obj).mod
#define GET_MOD(obj) GET_VAL(obj).mod
#define GET_LIST(obj) GET_VAL(obj).list
#define GET_CLASS(obj) GET_VAL(obj).clazz

#define DICT_LEN(obj)  GET_DICT(obj)->len
#define DICT_NODES(obj) GET_DICT(obj)->nodes
#define ptr_addr(ptr) (long) (ptr) / sizeof(char*)
#define GET_NUM(obj) (obj).value.dv


#define IS_NONE(obj)   MP_TYPE(obj) == TYPE_NONE
#define IS_LIST(obj)   MP_TYPE(obj) == TYPE_LIST
#define IS_FUNC(obj)   MP_TYPE(obj) == TYPE_FUNCTION
#define IS_CLASS(obj)  MP_TYPE(obj) == TYPE_CLASS
#define IS_DICT(o)     MP_TYPE(o)==TYPE_DICT
#define IS_STR(obj)    MP_TYPE(obj) == TYPE_STR
#define IS_NUM(obj)    MP_TYPE(obj) == TYPE_NUM
#define IS_DATA(obj)   MP_TYPE(obj) == TYPE_DATA
#define IS_NATIVE(obj) GET_FUNCTION(obj)->native != NULL

#define NOT_NONE(obj) MP_TYPE(obj) != TYPE_NONE
#define NOT_LIST(obj) MP_TYPE(obj) != TYPE_LIST
#define NOT_DICT(obj) MP_TYPE(obj) != TYPE_DICT
#define NOT_FUNC(obj) MP_TYPE(obj) != TYPE_FUNCTION
#define NOT_STR(obj)  MP_TYPE(obj) != TYPE_STR
#define NOT_NATIVE(obj) GET_FUNCTION(obj)->native == NULL



#define ASSERT_TYPE_WITH_INFO(obj, type, info) \
    if(MP_TYPE(obj)!=type){                    \
        mp_raise(info, obj);                    \
    }


/* for instruction read */

#define GET_FUNCTION_FILE(fnc) GET_MODULE(GET_FUNCTION(fnc)->mod)->file
#define GET_FUNCTION_NAME(fnc) GET_FUNCTION(fnc)->name
#define GET_FUNCTION_GLOBALS(fnc) GET_MODULE(GET_FUNCTION(fnc)->mod)->globals

/* assert macro */
#define MP_ASSERT(cond, msg) if(!(cond)) {mp_raise( msg );}

/* gcc process ++ from right to left */
#define READ_BYTE(s) *s++
#define READ_SHORT(s) ((*s) << 8 | *(s+1)); s+= 2;
/* #define next_short( s ) (((*s++) << 8) + *(s++)); */

/* debug and log functions */
#define DEBUG(msg) \
    printf("--DEBUG %s\n", msg);

#define DEBUG2(msg1, msg2) \
    printf("--DEBUG %s %s\n", msg1, msg2);

#define LOG_INFO printf

#if LIGHT_DEBUG_GC
    #define TRACE_GC(out) ;
#else
    #define TRACE_GC(out) puts(out);
#endif

#if LIGHT_DEBUG_GC
    #define CHECK_MEM_USAGE(msg) printf("%s, press enter to continue", msg);getchar();
#else
    #define CHECK_MEM_USAGE(msg) ;
#endif

#endif
