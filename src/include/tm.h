

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

#ifdef TM_CHECK_MEM
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
Object        tm_str(Object obj);
void          string_methods_init();
Object        string_iter_new(Object s);
Object*       string_next(TmData* iterator);

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

#include "mp_micro.h"

#endif
