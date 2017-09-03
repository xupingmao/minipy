

#ifndef _TM_H
#define _TM_H
/** 
 *  coding standards
 *  static variable . not specified
 *  function        . underscore
 *  structure       . camel case 
 *  method          . like string_builtin_xxx, list_builtin_xxx
 *  macro           . like XXX_XXX
 *  function(used by C)  - tm_xxx  - like tm_get(Object a, const char* key)
 *  function(used by vm) - tmV_xxx - like Object tmV_fnc(Object a, Object b)
 *  built-in function    - bf_xxx
*/

/* #pragma pack(4) */
#define DEBUG_GC 0
#define PRINT_INS 0
#define PRINT_INS_CONST 0
#define EVAL_DEBUG 1
#define DEBUG_INS 1
#define USE_NON_PARAM 1
#define RAISE_DEBUG 0

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdarg.h>
#include <time.h>
/* #include <sys/stat.h> */
#include <math.h>
#include <ctype.h>

#define TM_INLINE inline

#include "object.h"
#define OBJ_SIZE sizeof(Object)

Object NONE_OBJECT;
Object ARRAY_CHARS;

#include "instruction.h"

typedef Object (*BuiltinFunc)();

/** code functions **/
void code16(unsigned char*s, int value);
void code32(unsigned char*s, int value);
int uncode32(unsigned char**s);
int uncode16(unsigned char**s);


// gc functions
#define GC_DEBUG_LIST 0
void*       tm_malloc(size_t size);
void*       tm_realloc(void* o, size_t osize, size_t nsize);
void        tm_free(void* o, size_t size);
void        init_memory();
void        free_memory();

void        gc_init();
Object      gc_track(Object obj);
void        gc_destroy();
void        gc_full();
void        gc_sweep_local(int start);
Object      bf_get_malloc_info();
void        gc_mark(Object);
void        gc_unmark(Object);
void        gc_mark_single(Object);
void        gc_mark_list(TmList*);
void        gc_mark_dict(TmDict*);
void        gc_restore_local_obj_list(int size);
void        gc_native_call_sweep();
void        gc_check_native_call(int size, Object ret);

#if 0
    #define PRINT_OBJ_GC_INFO_START() int _gc_old = tm->allocated;
    #define PRINT_OBJ_GC_INFO_END(str, addr) \
        printf("free %s at 0x%p, %d => %d, ", str, addr, _gc_old, tm->allocated);
    #else
    #define PRINT_OBJ_GC_INFO_START()
    #define PRINT_OBJ_GC_INFO_END(str, addr)

    #define GC_LOG_START(ptr, desc)
    #define GC_LOG_END(ptr, desc)
#endif

/**
 * string functions
 */


Object        string_char_new(int c);
Object        string_chr(int n); // get a char from char_list.
Object        string_alloc(char* s, int size);
#define       sz_to_string(s) string_alloc(s, -1)
#define       string_new(s) string_alloc(s, strlen(s))
Object        string_const(char*);
void          string_free(String*);
int           string_equals(String*s0, String*s1);
Object        string_substring(String* str, int start, int end) ;
Object        bf_string_format();
Object        tm_str(Object obj);
Object        String_join(Object self, Object list);
void          string_methods_init();
Object        string_iter_new(Object s);
Object*       string_next(TmData* iterator);

/* macros */
#define GET_STR(obj) (obj).value.str->value
#define GET_SZ(obj)  (obj).value.str->value
#define GET_STR_OBJ(obj) (obj).value.str
#define GET_STR_LEN(obj) (obj).value.str->len


// number functions
Object     tm_number(double v);
void       number_format(char* des, Object num);
double     number_value(Object num);
long long  long_value(Object num);

/**
 * list functions
 */

void     list_check(TmList*);
Object   list_new(int cap);
/* create a TmList which not tracked by Garbage Collector. */
TmList*  untracked_list_new(int cap);
void     list_set(TmList* list, int n, Object v);
Object   list_get(TmList* list, int n);
void     list_free(TmList* );
void     list_clear(TmList* list);
void     list_methods_init();
Object   list_iter_new(Object list);
Object*  list_next(TmData* iterator);
Object   list_add(TmList*, TmList*);
void     list_del(TmList*list, Object key);
void     list_insert(TmList*list, int index, Object value);
int      list_index(TmList*, Object val);
void     list_append(TmList* list, Object v);
void     list_shorten(TmList* list, int len); // shorten list.
Object   array_to_list(int n, ...);
Object   list_builtin_extend();

/* macros */
#define LIST_GET(obj, i) list_get(GET_LIST(obj), i)
#define LIST_NODES(obj) (GET_LIST(obj))->nodes
#define LIST_LEN(obj) GET_LIST(obj)->len


// dict functions
Object           dict_new();
TmDict*          dict_init();
void             dict_free(TmDict* dict);
int              dict_set0(TmDict* dict, Object key, Object val);
DictNode*        dict_get_node(TmDict* dict, Object key);
Object*          dict_get_by_str0(TmDict* dict, char* key);
void             dict_del(TmDict* dict, Object k);
void             dict_methods_init();
void             dict_set_by_str0(TmDict* dict, char* key, Object val);
Object           dict_keys(TmDict* );

#define          dict_set(d, k, v)                dict_set0(GET_DICT(d), k, v)
#define          dict_set_by_str(dict, key, val)  dict_set_by_str0(GET_DICT(dict), key, val)
#define          dict_get_by_str(dict, key)       dict_get_by_str0(GET_DICT(dict), key)

/** dict methods **/
Object           dict_keys();
Object           dict_values();

Object           dict_iter_new(Object dict);
Object*          dict_next(TmData* iterator);
int              dict_set_attr(TmDict* dict, int const_id, Object val);
int              dict_get_attr(TmDict* dict, int const_id);



// arg functions
void    arg_insert(Object arg);
String* arg_take_str_ptr(const char* fnc);
void    arg_start();
void    arg_push(Object obj) ;
void    arg_set_arguments(Object* first, int len);
void    _resolve_method_self(TmFunction *fnc);
#define resolve_method_self(fnc) _resolve_method_self(GET_FUNCTION((fnc)))
void    print_arguments();
int     arg_has_next();
Object  arg_take_str_obj(const char* fnc);
char*   arg_take_sz(const char* fnc);
Object  arg_take_func_obj(const char* fnc);
int     arg_take_int(const char* fnc);
double  arg_take_double(const char* fnc);
TmList* arg_take_list_ptr(const char* fnc);
Object  arg_take_list_obj(const char* fnc);
Object  arg_take_dict_obj(const char* fnc);
Object  arg_take_obj(const char* fnc);
Object  arg_take_data_obj(const char* fnc);
int     get_args_count() ;
int     arg_remains();
void    tm_unget_arg();



// function functions


Object           func_new(Object mod,Object self,Object (*native_func)());
Object           get_func_attr(TmFunction* fnc, Object key);
void             func_free(TmFunction*);
Object           method_new(Object _fnc, Object self);
Object           module_new(Object file, Object name, Object code);
void             module_free(TmModule*);
unsigned char*   get_function_code(TmFunction*);
void             func_format(char* des, TmFunction* func);
TmModule*        get_func_mod(TmFunction* func);
Object           class_new(Object dict);
Object           get_function_globals(TmFunction*);
unsigned char*   func_resolve(TmFunction*, unsigned char*);
Object           get_file_name_obj(Object func);
Object           get_func_name_obj(Object func);
#define GET_FUNCTION(obj) (obj.value.func)
#define GET_FUNC(obj) ((obj).value.func)
#define IS_FUNCTION(o) TM_TYPE(o)==TYPE_FUNCTION
#define GET_FUNCTIONTION_MODULE_PTR(fnc) GET_MODULE(GET_FUNCTION(fnc)->mod)
#define GET_FUNC_CONSTANTS(fnc) GET_FUNCTIONTION_MODULE_PTR(fnc)->constants
#define GET_FUNC_MOD_NAME(fnc) GET_FUNCTIONTION_MODULE_PTR(fnc)->file
#define GET_FUNC_CONSTANTS_NODES(fnc) LIST_NODES(GET_FUNC_CONSTANTS(fnc))
#define GET_FUNCTION_NAME(fnc) GET_FUNCTION(fnc)->name
#define get_globals(func) get_function_globals(GET_FUNCTION(func))
#define function_format(des, func) func_format(des, GET_FUNCTION(func))


Object      data_new(size_t size);
void        data_mark();
void        data_free();
void        data_set(Object, Object, Object);
Object      data_get(Object, Object);
Object      data_str(Object self);
void        obj_free(Object o);
Object      obj_new(int type, void* value);


/** ops functions
 *  general object operation
 *  some tools
 */
const char* tm_type(int type);
void        obj_set(Object self, Object key, Object value);
Object      obj_get(Object self, Object key);
Object      obj_add(Object a, Object b);
Object      obj_sub(Object a, Object b);
Object      obj_mul(Object a, Object b);
Object      obj_div(Object a, Object b);
Object      obj_mod(Object a, Object b);
Object      obj_neg(Object o) ;
int         obj_in(Object key, Object collection);
int         obj_equals(Object a, Object b);
int         obj_cmp(Object a, Object b);
Object      obj_slice(Object self, Object first, Object second);
Object*     next_ptr(Object iterator);
Object      iter_new(Object collections);
void        obj_set(Object self, Object k, Object v);
char*       obj_to_sz(Object obj);

Object      tm_str(Object obj);
int         tm_len(Object obj);
int         is_true_obj(Object v);
int         tm_iter(Object self, Object *k);
void        obj_del(Object self, Object k);
Object      tm_get_global(Object globals, char* key);
Object      tm_call_builtin(BuiltinFunc func, int n, ...);

// vm functions
Object call_module_function(char* mod, char* fnc);
void   reg_builtin(char* name, Object value);
void   reg_mod_func(Object mod, char* name, Object(*native)());
void   reg_builtin_func(char* name, Object (*native)());
void   reg_mod_attr(char* mod_name,char* attr, Object value);
int    obj_eq_sz(Object str, const char* value);
void   tm_raise(char*fmt , ...);
Object obj_append(Object a, Object item);
void   vm_destroy();



#define  TM_PUSH(x) *(++top) = (x); if(top > tm->stack_end) tm_raise("stack overflow");
#define  TM_POP() *(top--)
#define  TM_TOP() (*top)
#define  GET_CONST(i) GET_DICT(tm->constants)->nodes[i].key
Object   call_unsafe(Object fnc);
Object   call_function(Object func);
Object   tm_load_module(Object filename, Object code, Object name);
Object   tm_eval(TmFrame*);
TmFrame* push_frame(Object fnc);
void     pop_frame();

// exception functions
void tm_assert_type(Object o, int type, char* msg) ;
void tm_assert_int(double value, char* msg) ;
void push_exception(TmFrame* f);
void traceback();
void tm_raise(char* fmt, ...);

// builtin functions
void      tm_print(Object v);
void      tm_println(Object v);
Object    tm_format_va_list(char* fmt, va_list ap, int appendln);
Object    tm_format(char*fmt, ...);
void      tm_inspect_obj(Object o);
void      tm_printf(char* fmt, ...);
/* avoid '\0' in char array, which will be regarded as end by c lang */
/* Chars     Object_info(char*,Object,int); */
Object    tm_load(char* fname); // load the content of a file.
Object    bf_load();
Object    bf_save(); // save(fname, content);
Object    bf_int();
Object    bf_float();
Object    bf_system();
Object    bf_print();
Object    blt__add_type_method();
void      builtins_init();
Object*   get_builtin(char* key);


// vm.c
Object    call_mod_func(char* modname, char* funcname);

// macros
#define strequals(a, b) (a == b || strcmp(a,b) == 0)

#define GET_VAL(obj) (obj).value
#define GET_PTR(obj) (obj).value.ptr
#define GET_DATA(obj) (obj).value.data
#define GET_DATA_PROTO(obj) (obj).value.data->proto
#define GET_DICT(obj) GET_VAL(obj).dict
#define GET_MODULE(obj) GET_VAL(obj).mod
#define GET_MOD(obj) GET_VAL(obj).mod
#define GET_LIST(obj) GET_VAL(obj).list

#define DICT_LEN(obj)  GET_DICT(obj)->len
#define DICT_NODES(obj) GET_DICT(obj)->nodes
#define ptr_addr(ptr) (long) (ptr) / sizeof(char*)
#define GET_NUM(obj) (obj).value.dv


#define IS_NONE(obj)   TM_TYPE(obj) == TYPE_NONE
#define IS_LIST(obj)   TM_TYPE(obj) == TYPE_LIST
#define IS_FUNC(obj)   TM_TYPE(obj) == TYPE_FUNCTION
#define IS_DICT(o)     TM_TYPE(o)==TYPE_DICT
#define IS_STR(obj)    TM_TYPE(obj) == TYPE_STR
#define IS_NUM(obj)    TM_TYPE(obj) == TYPE_NUM
#define IS_DATA(obj)   TM_TYPE(obj) == TYPE_DATA
#define IS_NATIVE(obj) GET_FUNCTION(obj)->native != NULL

#define NOT_NONE(obj) TM_TYPE(obj) != TYPE_NONE
#define NOT_LIST(obj) TM_TYPE(obj) != TYPE_LIST
#define NOT_DICT(obj) TM_TYPE(obj) != TYPE_DICT
#define NOT_FUNC(obj) TM_TYPE(obj) != TYPE_FUNCTION
#define NOT_STR(obj)  TM_TYPE(obj) != TYPE_STR
#define NOT_NATIVE(obj) GET_FUNCTION(obj)->native == NULL



#define ASSERT_TYPE_WITH_INFO(obj, type, info) \
    if(TM_TYPE(obj)!=type){                    \
        tm_raise(info, obj);                    \
    }
/* for instruction read */

/* gcc process ++ from right to left */
#define READ_BYTE(s) *s++
#define READ_SHORT(s) ((*s) << 8 | *(s+1)); s+= 2;
/* #define next_short( s ) (((*s++) << 8) + *(s++)); */

/* for math */
#define max(a, b) (a) > (b) ? (a) : (b)

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

#define GET_FUNCTION_FILE(fnc) GET_MODULE(GET_FUNCTION(fnc)->mod)->file
#define GET_FUNCTION_NAME(fnc) GET_FUNCTION(fnc)->name
#define GET_FUNCTION_GLOBALS(fnc) GET_MODULE(GET_FUNCTION(fnc)->mod)->globals

/* assert macro */
#define TM_ASSERT(cond, msg) if(!(cond)) {tm_raise( msg );}


#define DEBUG(msg) \
    printf("--DEBUG %s\n", msg);

#define DEBUG2(msg1, msg2) \
    printf("--DEBUG %s %s\n", msg1, msg2);
    
#endif

#define LOG_INFO printf