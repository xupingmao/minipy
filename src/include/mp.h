

#ifndef _MP_H
#define _MP_H
/** 
 *  coding standards
 *  static variable . not specified
 *  function        . underscore
 *  structure       . camel case 
 *  method          . like string_builtin_xxx, list_builtin_xxx
 *  macro           . like XXX_XXX
 *  function(used by C)  - mp_xxx    - like mp_get(MpObj a, const char* key)
 *  function(used by vm) - mp_vm_xxx - like MpObj mp_vm_func(MpObj a, MpObj b)
 *  built-in function    - bf_xxx
 * 
 *  C-language Types abbr:
 *    cstr: char*
 *  
 *  Minipy Types abbr:
 *    obj : MpObj
 *    pobj: MpObj*
 *    list: MpList
 *    str : MpStr*
 *    list: MpList*
 *    dict: MpDict*
 *    clazz: MpClass*
 *    func: MpFunction*
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

#define MP_INLINE inline

#include "object.h"
#define OBJ_SIZE sizeof(MpObj)

MpObj NONE_OBJECT;
MpObj ARRAY_CHARS;

// 函数指针
typedef MpObj (* MpNativeFunc ) ();


#include "instruction.h"

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

typedef MpObj (*BuiltinFunc)();

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
MpObj       gc_track(MpObj obj);
void        gc_destroy();
void        gc_full();
void        gc_sweep_local(int start);
MpObj       bf_get_malloc_info();
void        gc_mark(MpObj);
void        gc_unmark(MpObj);
void        gc_mark_single(MpObj);
void        gc_mark_list(MpList*);
void        gc_mark_dict(MpDict*);
void        gc_restore_local_obj_list(int size);
void        gc_native_call_sweep();
void        gc_check_native_call(int size, MpObj ret);

/**
 * string functions
 */
MpObj        string_char_new(int c);
MpObj        string_chr(int n); // get a char from char_list.
MpObj        string_alloc(char* s, int size);
MpObj        string_new(char*s);
MpObj        string_static(char*s);
#define      string_from_cstr(s) string_static(s)
MpObj        string_const(char*);
void         string_free(MpStr*);
int          string_equals(MpStr*s0, MpStr*s1);
MpObj        string_substring(MpStr* str, int start, int end) ;
void         string_methods_init();
MpObj        string_iter_new(MpObj s);
MpObj*       string_next(MpData* iterator);

// number functions
MpObj      number_obj(double v);
void       number_format(char* des, MpObj num);
double     number_get_double(MpObj num);
long long  long_value(MpObj num);

/**
 * list functions
 */
void     list_check(MpList*);
MpObj    list_new(int cap);
/* create a MpList which not tracked by Garbage Collector. */
MpList*  list_new_untracked(int cap);
void     list_set(MpList* list, int n, MpObj v);
MpObj    list_get(MpList* list, int n);
void     list_free(MpList* );
void     list_clear(MpList* list);
void     list_methods_init();
MpObj    list_iter_new(MpObj list);
MpObj*   list_next(MpData* iterator);
MpObj    list_add(MpList*, MpList*);
void     list_del(MpList*list, MpObj key);
void     list_insert(MpList*list, int index, MpObj value);
int      list_index(MpList*, MpObj val);
void     list_append(MpList* list, MpObj v);
void     list_shorten(MpList* list, int len); // shorten list.
MpObj   list_from_array(int n, ...);
MpObj   list_builtin_extend();

// dict functions
MpObj            dict_new();
MpObj            dict_new_obj();
MpDict*          dict_init();
void             dict_free(MpDict* dict);
int              dict_set0(MpDict* dict, MpObj key, MpObj val);
DictNode*        dict_get_node(MpDict* dict, MpObj key);
MpObj*           dict_get_by_cstr(MpDict* dict, char* key);
void             dict_del(MpDict* dict, MpObj k);
void             dict_methods_init();
void             dict_set_by_cstr(MpDict* dict, char* key, MpObj val);
MpObj            dict_keys(MpDict* );

#define          dict_set(d, k, v)                dict_set0(GET_DICT(d), k, v)
#define          dict_get_by_str(dict, key)       dict_get_by_cstr(GET_DICT(dict), key)

/** dict methods **/
MpObj            dict_iter_new(MpObj dict);
MpObj*           dict_next(MpData* iterator);
int              dict_set_attr(MpDict* dict, int const_id, MpObj val);
int              dict_get_attr(MpDict* dict, int const_id);


// arg functions
void    arg_insert(MpObj arg);
MpStr*  arg_take_str_ptr(const char* fnc);
void    arg_start();
void    arg_push(MpObj obj) ;
void    arg_set_arguments(MpObj* first, int len);
void    resolve_self_by_func_ptr(MpFunction *fnc);
void    print_arguments();
int     arg_has_next();
MpObj   arg_take_str_obj(const char* fnc);
char*   arg_take_cstr(const char* fnc);
MpObj   arg_take_func_obj(const char* fnc);
int     arg_take_int(const char* fnc);
double  arg_take_double(const char* fnc);
MpList* arg_take_list_ptr(const char* fnc);
MpObj  arg_take_list_obj(const char* fnc);
MpObj  arg_take_dict_obj(const char* fnc);
MpObj  arg_take_obj(const char* fnc);
MpObj  arg_take_data_obj(const char* fnc);
int arg_count() ;
int arg_remains();


// function functions
MpObj           func_new(MpObj mod,MpObj self,MpObj (*native_func)());
MpObj           func_get_attr(MpFunction* fnc, MpObj key);
void             func_free(MpFunction*);
unsigned char*   func_get_code(MpFunction*);
MpObj           func_get_code_obj(MpFunction*);
void             func_format(char* des, MpFunction* func);
MpModule*        get_func_mod(MpFunction* func);
MpObj           func_get_globals(MpFunction*);
unsigned char*   func_resolve(MpFunction*, unsigned char*);
MpObj           func_get_file_name_obj(MpObj func);
MpObj           func_get_name_obj(MpObj func);

MpObj           method_new(MpObj _fnc, MpObj self);
MpObj           module_new(MpObj file, MpObj name, MpObj code);
void            module_free(MpModule*);

MpObj           class_new(MpObj name);
MpObj           class_new_by_cstr(char* name);
MpObj           class_instance(MpObj dict);
void            class_format(char* dest, MpObj clazz);
void            class_free(MpClass* pclass);


MpObj      data_new(size_t size);
void       data_mark();
void       data_free();
void       data_set(MpObj, MpObj, MpObj);
MpObj      data_get(MpObj, MpObj);
MpObj      data_str(MpObj self);
void       obj_free(MpObj o);
MpObj      obj_new(int type, void* value);


/** 
 * ops functions
 *  general object operation
 *  some tools
 */
const char* get_type_cstr(int type);
const char* get_object_type_cstr(MpObj object);
void        obj_set(MpObj self, MpObj key, MpObj value);
void        obj_set_by_cstr(MpObj self, char* key, MpObj value);
void        obj_del(MpObj self, MpObj k);
MpObj      obj_get(MpObj self, MpObj key);
MpObj      obj_add(MpObj a, MpObj b);
MpObj      obj_sub(MpObj a, MpObj b);
MpObj      obj_mul(MpObj a, MpObj b);
MpObj      obj_div(MpObj a, MpObj b);
MpObj      obj_mod(MpObj a, MpObj b);
MpObj      obj_or (MpObj a, MpObj b);
MpObj      obj_neg(MpObj o);
MpObj      obj_cmp(MpObj a, MpObj b);
MpObj      obj_is_in(MpObj left, MpObj right);
MpObj      obj_slice(MpObj self, MpObj first, MpObj second);
MpObj      iter_new(MpObj collections);
MpObj      obj_str(MpObj obj);
MpObj      obj_append(MpObj a, MpObj item);
MpObj      obj_get_globals(MpObj obj);

MpObj*      obj_next(MpObj iterator);
char*       obj_to_cstr(MpObj obj);
int         mp_cmp(MpObj a, MpObj b);
int         mp_is_in(MpObj key, MpObj collection);
int         is_obj_equals(MpObj a, MpObj b);
int         mp_len(MpObj obj);
int         is_true_obj(MpObj v);
int         mp_iter(MpObj self, MpObj *k);
MpObj       mp_get_global_by_cstr(MpObj globals, char* key);
MpObj       mp_call_builtin(BuiltinFunc func, int n, ...);

// vm functions
MpObj  call_module_function(char* mod, char* fnc);
void   reg_builtin(char* name, MpObj value);
void   reg_builtin_func(char* name, MpObj(*native_func)());
void   reg_mod(char* name, MpObj module);
void   reg_mod_func(MpObj mod, char* name, MpObj(*native_func)());
void   reg_mod_attr(MpObj mod, char* attr, MpObj value);
void   reg_method_by_cstr(MpObj clazz, char* name, MpObj(*native_func)());
int    obj_eq_cstr(MpObj str, const char* value);
void   mp_raise(char*fmt , ...);
void   vm_destroy();


#define  GET_CONST(i) GET_DICT(tm->constants)->nodes[i].key
MpObj    call_unsafe(MpObj fnc);
MpObj    obj_call(MpObj func);
MpObj    load_file_module(MpObj filename, MpObj code, MpObj name);
MpObj    mp_eval(MpFrame*);
MpFrame* push_frame(MpObj fnc);
void     pop_frame();

// exception functions
void mp_assert(int value, char*msg);
void mp_assert_type(MpObj o, int type, char* msg) ;
void mp_assert_int(double value, char* msg) ;
void mp_push_exception(MpFrame* f);
void mp_traceback();
void mp_raise(char* fmt, ...);

// builtin functions
void      mp_print(MpObj v);
void      mp_println(MpObj v);
MpObj    mp_format_va_list(char* fmt, va_list ap, int appendln);
MpObj    mp_format_va_list_check_length(char* fmt, va_list ap, int ap_length, int appendln);
MpObj    mp_format(char*fmt, ...);
MpObj    mp_format_check_length(char*fmt, int ap_length, ...);
void      mp_inspect_obj(MpObj o);
void      mp_printf(char* fmt, ...);

/* avoid '\0' in char array, which will be regarded as end by c lang */
/* Chars     MpObj_info(char*,MpObj,int); */
MpObj    mp_load(char* fname); // load the content of a file.
MpObj    bf_load();
MpObj    bf_save(); // save(fname, content);
MpObj    bf_int();
MpObj    bf_float();
MpObj    bf_system();
MpObj    bf_print();
void     builtins_init();
MpObj*   get_builtin(char* key);


// vm.c
MpObj    vm_call_mod_func(char* modname, char* funcname);

// time.c
int64_t  time_get_milli_seconds();

#include "mp_micro.h"
#include "mp_log.h"

#endif
