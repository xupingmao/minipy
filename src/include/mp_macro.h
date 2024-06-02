/*
 * mp_mico.h
 *
 *  Created on: 2020/09/21
 *  @author: xupingmao
 *  @modified 2022/06/11 20:25:54
 */

#ifndef MP_MACRO_H_
#define MP_MACRO_H_

#include <assert.h>

/* c micros */
#define strequals(a, b) (a == b || strcmp(a,b) == 0)
#define max(a, b) (a) > (b) ? (a) : (b)
#define MAX(a, b) (a) > (b) ? (a) : (b)
#define MIN(a, b) (a) > (b) ? (b) : (a)
#define string_const2 string_const_with_len

#define MP_INLINE inline


/* macros */
#define GET_CSTR(obj)    (obj).value.str->value
#define STR_TO_CSTR(obj) (obj).value.str->value
#define GET_STR_OBJ(obj) (obj).value.str
#define GET_STR_LEN(obj) (obj).value.str->len
#define GET_STR_CHAR(obj, index) (obj).value.str->value[index]


/* list micros */
#define LIST_GET(obj, i) list_get(GET_LIST(obj), i)
#define LIST_NODES(obj) (GET_LIST(obj))->nodes
#define LIST_LEN(obj) GET_LIST(obj)->len


/* function micros */
#define GET_FUNCTION(obj) (obj.value.func)
#define GET_FUNC(obj) ((obj).value.func)
#define IS_FUNCTION(o) (MP_TYPE(o)==TYPE_FUNCTION)
#define GET_FUNCTIONTION_MODULE_PTR(fnc) GET_MODULE(GET_FUNCTION(fnc)->mod)
#define GET_FUNC_CONSTANTS(fnc) GET_FUNCTIONTION_MODULE_PTR(fnc)->constants
#define GET_FUNC_MOD_NAME(fnc) GET_FUNCTIONTION_MODULE_PTR(fnc)->file
#define GET_FUNC_CONSTANTS_NODES(fnc) LIST_NODES(GET_FUNC_CONSTANTS(fnc))
#define GET_FUNCTION_NAME(fnc) GET_FUNCTION(fnc)->name
// #define GET_GLOBALS(func) func_get_globals(GET_FUNCTION(func))
#define FUNCTION_FORMAT(des, func) func_format(des, GET_FUNCTION(func))


/* Get value micros */
#define GET_VAL(obj) (obj).value
#define GET_PTR(obj) (obj).value.ptr
#define GET_DATA(obj) (obj).value.data
#define GET_DATA_PROTO(obj) (obj).value.data->proto
#define GET_DICT(obj) GET_VAL(obj).dict
#define GET_DICT_ATTR(dict, key)  *dict_get_by_cstr(GET_DICT(dict), key)
#define GET_MODULE(obj) GET_VAL(obj).mod
#define GET_MOD(obj) GET_VAL(obj).mod
#define GET_LIST(obj) GET_VAL(obj).list
#define GET_CLASS(obj) GET_VAL(obj).clazz
#define GET_INSTANCE(obj) GET_VAL(obj).instance

#define DICT_LEN(obj)  GET_DICT(obj)->len
#define DICT_NODES(obj) GET_DICT(obj)->nodes
#define ptr_addr(ptr) (long) (ptr) / sizeof(char*)
#define GET_NUM(obj) (obj).value.dv
#define IS_DICT_NODE_USED(node) ((node).used == 1)

/* Check object type micros */
#define IS_NONE(obj)   (MP_TYPE(obj) == TYPE_NONE)
#define IS_LIST(obj)   (MP_TYPE(obj) == TYPE_LIST)
#define IS_FUNC(obj)   (MP_TYPE(obj) == TYPE_FUNCTION)
#define IS_CLASS(obj)  (MP_TYPE(obj) == TYPE_CLASS)
#define IS_DICT(o)     (MP_TYPE(o)==TYPE_DICT)
#define IS_STR(obj)    (MP_TYPE(obj) == TYPE_STR)
#define IS_NUM(obj)    (MP_TYPE(obj) == TYPE_NUM)
#define IS_DATA(obj)   (MP_TYPE(obj) == TYPE_DATA)
#define IS_NATIVE(obj) (GET_FUNCTION(obj)->native != NULL)
#define IS_MODULE(obj) (MP_TYPE(obj) == TYPE_MODULE)
#define IS_INSTANCE(obj) (MP_TYPE(obj) == TYPE_INSTANTCE)

#define NOT_NONE(obj) (MP_TYPE(obj) != TYPE_NONE)
#define NOT_LIST(obj) (MP_TYPE(obj) != TYPE_LIST)
#define NOT_DICT(obj) (MP_TYPE(obj) != TYPE_DICT)
#define NOT_FUNC(obj) (MP_TYPE(obj) != TYPE_FUNCTION)
#define NOT_STR(obj)  (MP_TYPE(obj) != TYPE_STR)
#define NOT_NATIVE(obj) (GET_FUNCTION(obj)->native == NULL)
#define NOT_CLASS(obj) (MP_TYPE(obj) != TYPE_CLASS)

#define ASSERT_VALID_OBJ(obj) assert((obj).type >= TYPE_MIN && (obj).type <= TYPE_MAX)

#define ASSERT_TYPE_WITH_INFO(obj, type, info) \
    if(MP_TYPE(obj)!=type){                    \
        mp_raise(info, obj);                   \
    }


/* for instruction read */

#define GET_FUNCTION_FILE(fnc) GET_MODULE(GET_FUNCTION(fnc)->mod)->file
#define GET_FUNCTION_NAME(fnc) GET_FUNCTION(fnc)->name
// #define GET_FUNCTION_GLOBALS(fnc) GET_MODULE(GET_FUNCTION(fnc)->mod)->globals

/* assert macro */
#define MP_ASSERT(cond, msg) if(!(cond)) {mp_raise( msg );}
#define RESOLVE_METHOD_SELF(fnc) mp_resolve_self_by_func_ptr(GET_FUNCTION((fnc)))

/* gcc process ++ from right to left */
#define READ_BYTE(s) *s++
#define READ_SHORT(s) ((*s) << 8 | *(s+1)); s+= 2;
/* #define next_short( s ) (((*s++) << 8) + *(s++)); */

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


#ifdef MP_DEBUG
    #define MP_CALL_EX(a) mp_call_obj((a), __FILE__, __LINE__)
#else
    #define MP_CALL_EX(a) mp_call_obj((a))
#endif


#ifdef MP_CHECK_MEM
    /* #include <execinfo.h> */
    #include "map.h"

    #define sys_malloc malloc
    #define sys_free   free
    
    DEF_MAP(PtrMap, void*, int);

    PtrMap* ptr_map;
    int ptr_map_malloc_cnt = 0;
    int ptr_map_free_cnt = 0;

    void* PtrMap_malloc(int size, const char* file, int line) {
        ptr_map_malloc_cnt++;
        /* printf("%s:%d malloc(%d)\n", file, line, size); */
        void* mem = sys_malloc(size);
        PtrMap_set(ptr_map, mem, 1);
        return mem;
    }

    void PtrMap_free_mem(void* mem, const char* file, int line) {
        ptr_map_free_cnt++;
        int *v = PtrMap_get(ptr_map, mem);
        if (v == NULL) {
            fprintf(stderr, "%s:%d (%p) invalid free!\n", file, line, mem);
            /*
            int j, nptrs;
            void* buffer[100];
            char** strings;
            nptrs = backtrace(buffer, 100);
            strings = backtrace_symbols(buffer, nptrs);
            printf("backtrace() returned %d addresses\n", nptrs);
            if (strings != NULL) {            
                for (j = 0; j < nptrs; j++) {
                    printf("%s\n", strings[j]);
                }
                free(strings);
            }
            */
            exit(1);
        } else {
            sys_free(mem);
            PtrMap_del(ptr_map, mem);
        }
    }

    void* PtrMap_realloc(void* mem, int nsize, const char* file, int line) {
        int *v = PtrMap_get(ptr_map, mem);
        if (v == NULL) {
            fprintf(stderr, "%s:%d (%p) invalid realloc!\n", file, line, mem);
            exit(1);
        }
        PtrMap_del(ptr_map, mem);
        void* new_mem = realloc(mem, nsize);
        PtrMap_set(ptr_map, new_mem, 1);
        return new_mem;
    }

    #define malloc(size)  PtrMap_malloc(size, __FILE__, __LINE__)
    #define free(mem)     PtrMap_free_mem(mem, __FILE__, __LINE__)
    #define realloc(mem, nsize) PtrMap_realloc(mem, nsize, __FILE__, __LINE__)

#endif

#endif

