/*
 * @Author: xupingmao
 * @email: 578749341@qq.com
 * @Date: 2023-12-07 22:03:29
 * @LastEditors: xupingmao
 * @LastEditTime: 2023-12-09 11:24:47
 * @FilePath: /minipy/src/include/mp.h
 * @Description: 描述
 */
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
#include <assert.h>

#define MP_INLINE inline

#include "object.h"

#define OBJ_SIZE sizeof(MpObj)

MpObj NONE_OBJECT;
MpObj ARRAY_CHARS;

// 函数指针
typedef MpObj (*MpNativeFunc)();
typedef MpObj (*BuiltinFunc)();

#include "instruction.h"

/** code functions **/
void code16(unsigned char*s, int value);
void code32(unsigned char*s, int value);
int  uncode32(unsigned char**s);
int  uncode16(unsigned char**s);

#include "gc.h"
#include "string.h"

// number functions
MpObj      number_obj(double v);
void       number_format(char* des, MpObj num);
double     number_get_double(MpObj num);

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
MpObj    list_from_array(int n, ...);
MpObj    list_builtin_extend();

#include "dict.h"

// arg functions
void    arg_insert(MpObj arg);
MpStr*  arg_take_str_ptr(const char* fnc);
void    arg_start();
void    arg_push(MpObj obj) ;
void    arg_set_arguments(MpObj* first, int len);
void    resolve_self_by_func_ptr(MpFunction *fnc);
void    arg_print();
int     arg_has_next();
MpObj   arg_take_str_obj(const char* fnc);
char*   arg_take_cstr(const char* fnc);
MpObj   arg_take_func_obj(const char* fnc);
int     arg_take_int(const char* fnc);
double  arg_take_double(const char* fnc);
MpList* arg_take_list_ptr(const char* fnc);
MpObj   arg_take_list_obj(const char* fnc);
MpObj   arg_take_dict_obj(const char* fnc);
MpDict* arg_take_dict_ptr(const char* fnc);
MpObj   arg_take_obj(const char* fnc);
MpObj   arg_take_data_obj(const char* fnc);
int arg_count() ;
int arg_remains();

#include "function.h"

MpObj      data_new(size_t size);
MpData* data_new_ptr(size_t size);
MpObj data_ptr_to_obj(MpData* ptr);
void       data_mark(MpData* ptr);
void       data_free(MpData* ptr);
void       data_set(MpData*, MpObj, MpObj);
MpObj      data_get(MpData*, MpObj);
MpObj      data_str(MpData* self);
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
const char* obj_to_cstr(MpObj a);
MpObj      obj_append(MpObj a, MpObj item);
MpObj      obj_get_globals(MpObj obj);

MpObj*      obj_next(MpObj iterator);
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


#define  GET_CONST(i) tm->constants->nodes[i].key
MpObj    call_unsafe(MpObj fnc);

#ifdef MP_DEBUG
MpObj    obj_call(MpObj, const char*, int);
#else
MpObj    obj_call(MpObj);
#endif

MpObj    obj_call_nargs(MpObj func, int n, MpObj* args);
MpObj    obj_apply(MpObj func, MpObj args);
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
void     mp_print(MpObj v);
void     mp_println(MpObj v);
MpObj    mp_format_va_list(char* fmt, va_list ap, int appendln);
MpObj    mp_format_va_list_check_length(char* fmt, va_list ap, int ap_length, int appendln);
MpObj    mp_format(char*fmt, ...);
MpObj    mp_format_check_length(char*fmt, int ap_length, ...);
void     mp_inspect_obj(MpObj o);
void     mp_printf(char* fmt, ...);

/* avoid '\0' in char array, which will be regarded as end by c lang */
/* Chars     MpObj_info(char*,MpObj,int); */
MpObj    mp_load(char* fname); // load the content of a file.
void     builtins_init();
MpObj*   get_builtin(char* key);


/* vm.c */
MpObj vm_call_mod_func(const char* modname, const char* funcname);

// time.c
int64_t  time_get_milli_seconds();

// import.c
MpObj  obj_import(MpObj globals, MpObj name);
MpObj  obj_import_attr(MpObj globals, MpObj module_name, MpObj attr);

const char* CodeCache_ToString(MpCodeCache* cache);

#include "mp_macro.h"
#include "mp_log.h"
#include "mp_debug.h"

#endif
