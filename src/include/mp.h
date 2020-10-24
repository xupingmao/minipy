

#ifndef _MP_H
#define _MP_H
/** 
 *  coding standards
 *  static variable . not specified
 *  function        . underscore
 *  structure       . camel case 
 *  method          . like string_builtin_xxx, list_builtin_xxx
 *  macro           . like XXX_XXX
 *  function(used by C)  - mp_xxx  - like mp_get(Object a, const char* key)
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
void*       mp_malloc(size_t size);
void*       mp_realloc(void* o, size_t osize, size_t nsize);
void        mp_free(void* o, size_t size);
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
void        gc_mark_list(MpList*);
void        gc_mark_dict(MpDict*);
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
Object        string_new(char*s);
#define       string_from_sz(s) string_alloc(s, -1)
#define       string_static(s)  string_alloc(s, -1)
Object        string_const(char*);
void          string_free(String*);
int           string_equals(String*s0, String*s1);
Object        string_substring(String* str, int start, int end) ;
void          string_methods_init();
Object        string_iter_new(Object s);
Object*       string_next(MpData* iterator);

// number functions
Object     number_obj(double v);
void       number_format(char* des, Object num);
double     number_get_double(Object num);
long long  long_value(Object num);

/**
 * list functions
 */

void     list_check(MpList*);
Object   list_new(int cap);
/* create a MpList which not tracked by Garbage Collector. */
MpList*  list_new_untracked(int cap);
void     list_set(MpList* list, int n, Object v);
Object   list_get(MpList* list, int n);
void     list_free(MpList* );
void     list_clear(MpList* list);
void     list_methods_init();
Object   list_iter_new(Object list);
Object*  list_next(MpData* iterator);
Object   list_add(MpList*, MpList*);
void     list_del(MpList*list, Object key);
void     list_insert(MpList*list, int index, Object value);
int      list_index(MpList*, Object val);
void     list_append(MpList* list, Object v);
void     list_shorten(MpList* list, int len); // shorten list.
Object   list_from_array(int n, ...);
Object   list_builtin_extend();

// dict functions
Object           dict_new();
MpDict*          dict_init();
void             dict_free(MpDict* dict);
int              dict_set0(MpDict* dict, Object key, Object val);
DictNode*        dict_get_node(MpDict* dict, Object key);
Object*          dict_get_by_str0(MpDict* dict, char* key);
void             dict_del(MpDict* dict, Object k);
void             dict_methods_init();
void             dict_set_by_str0(MpDict* dict, char* key, Object val);
Object           dict_keys(MpDict* );

#define          dict_set(d, k, v)                dict_set0(GET_DICT(d), k, v)
#define          dict_set_by_str(dict, key, val)  dict_set_by_str0(GET_DICT(dict), key, val)
#define          dict_get_by_str(dict, key)       dict_get_by_str0(GET_DICT(dict), key)

/** dict methods **/
Object           dict_keys();
Object           dict_values();
Object           dict_iter_new(Object dict);
Object*          dict_next(MpData* iterator);
int              dict_set_attr(MpDict* dict, int const_id, Object val);
int              dict_get_attr(MpDict* dict, int const_id);


// arg functions
void    arg_insert(Object arg);
String* arg_take_str_ptr(const char* fnc);
void    arg_start();
void    arg_push(Object obj) ;
void    arg_set_arguments(Object* first, int len);
void    _resolve_method_self(MpFunction *fnc);
#define resolve_method_self(fnc) _resolve_method_self(GET_FUNCTION((fnc)))
void    print_arguments();
int     arg_has_next();
Object  arg_take_str_obj(const char* fnc);
char*   arg_take_sz(const char* fnc);
Object  arg_take_func_obj(const char* fnc);
int     arg_take_int(const char* fnc);
double  arg_take_double(const char* fnc);
MpList* arg_take_list_ptr(const char* fnc);
Object  arg_take_list_obj(const char* fnc);
Object  arg_take_dict_obj(const char* fnc);
Object  arg_take_obj(const char* fnc);
Object  arg_take_data_obj(const char* fnc);
int     arg_count() ;
int     arg_remains();


// function functions
Object           func_new(Object mod,Object self,Object (*native_func)());
Object           func_get_attr(MpFunction* fnc, Object key);
void             func_free(MpFunction*);
unsigned char*   func_get_code(MpFunction*);
Object           func_get_code_obj(MpFunction*);
void             func_format(char* des, MpFunction* func);
MpModule*        get_func_mod(MpFunction* func);
Object           get_function_globals(MpFunction*);
unsigned char*   func_resolve(MpFunction*, unsigned char*);
Object           get_file_name_obj(Object func);
Object           get_func_name_obj(Object func);

Object           method_new(Object _fnc, Object self);
Object           module_new(Object file, Object name, Object code);
void             module_free(MpModule*);

Object           class_new(Object name);
Object           class_instance(Object dict);
void             class_format(char* dest, Object clazz);
void             class_free(MpClass* pclass);


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
const char* mp_type(int type);
void        obj_set(Object self, Object key, Object value);
void        obj_del(Object self, Object k);
Object      obj_get(Object self, Object key);
Object      obj_add(Object a, Object b);
Object      obj_sub(Object a, Object b);
Object      obj_mul(Object a, Object b);
Object      obj_div(Object a, Object b);
Object      obj_mod(Object a, Object b);
Object      obj_neg(Object o) ;
Object      obj_cmp(Object a, Object b);
Object      obj_in(Object left, Object right);
Object      obj_slice(Object self, Object first, Object second);
Object      iter_new(Object collections);
Object      mp_str(Object obj);
Object      obj_append(Object a, Object item);

Object*     next_ptr(Object iterator);
char*       obj_to_sz(Object obj);
int         mp_cmp(Object a, Object b);
int         mp_in(Object key, Object collection);
int         obj_equals(Object a, Object b);
int         mp_len(Object obj);
int         is_true_obj(Object v);
int         mp_iter(Object self, Object *k);
Object      mp_get_global(Object globals, char* key);
Object      mp_call_builtin(BuiltinFunc func, int n, ...);

// vm functions
Object call_module_function(char* mod, char* fnc);
void   reg_builtin(char* name, Object value);
void   reg_builtin_func(char* name, Object (*native)());
void   reg_mod_func(Object mod, char* name, Object(*native)());
void   reg_mod_attr(char* mod_name,char* attr, Object value);
int    obj_eq_sz(Object str, const char* value);
void   mp_raise(char*fmt , ...);
void   vm_destroy();



#define  TM_PUSH(x) *(++top) = (x); if(top > tm->stack_end) mp_raise("stack overflow");
#define  TM_POP() *(top--)
#define  TM_TOP() (*top)
#define  GET_CONST(i) GET_DICT(tm->constants)->nodes[i].key
Object   call_unsafe(Object fnc);
Object   call_function(Object func);
Object   load_file_module(Object filename, Object code, Object name);
Object   mp_eval(MpFrame*);
MpFrame* push_frame(Object fnc);
void     pop_frame();

// exception functions
void mp_assert_type(Object o, int type, char* msg) ;
void mp_assert_int(double value, char* msg) ;
void push_exception(MpFrame* f);
void traceback();
void mp_raise(char* fmt, ...);

// builtin functions
void      mp_print(Object v);
void      mp_println(Object v);
Object    mp_format_va_list(char* fmt, va_list ap, int appendln);
Object    mp_format_va_list_check_length(char* fmt, va_list ap, int ap_length, int appendln);
Object    mp_format(char*fmt, ...);
Object    mp_format_check_length(char*fmt, int ap_length, ...);
void      mp_inspect_obj(Object o);
void      mp_printf(char* fmt, ...);
/* avoid '\0' in char array, which will be regarded as end by c lang */
/* Chars     Object_info(char*,Object,int); */
Object    mp_load(char* fname); // load the content of a file.
Object    bf_load();
Object    bf_save(); // save(fname, content);
Object    bf_int();
Object    bf_float();
Object    bf_system();
Object    bf_print();
void      builtins_init();
Object*   get_builtin(char* key);


// vm.c
Object    call_mod_func(char* modname, char* funcname);

#include "mp_micro.h"

#endif
