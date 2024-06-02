/*
 * @Author: xupingmao
 * @email: 578749341@qq.com
 * @Date: 2023-12-07 22:35:37
 * @LastEditors: xupingmao
 * @LastEditTime: 2024-06-02 11:56:20
 * @FilePath: /minipy/src/include/function.h
 * @Description: minipy函数的定义
 */
#ifndef MP_FUNCTION_H
#define MP_FUNCTION_H

#include "object.h"

// function functions
MpObj func_new(MpObj mod, MpObj self, MpNativeFunc);
MpFunction* mp_new_native_func(MpObj module, MpNativeFunc native_func);
MpObj mp_new_native_func_obj(MpObj module, MpNativeFunc native_func);

MpObj func_get_attr(MpFunction* fnc, MpObj key);
void func_free(MpFunction*);
unsigned char* func_get_code(MpFunction*);
MpObj func_get_code_obj(MpFunction*);
void func_format(char* des, MpFunction* func);
MpObj func_get_mod_obj(MpFunction* func);
MpObj func_get_globals(MpFunction*);
unsigned char* func_resolve(MpFunction*, unsigned char*);
MpObj func_get_file_name_obj(MpObj func);
MpObj func_get_name_obj(MpObj func);

MpObj method_new(MpObj _fnc, MpObj self);
MpObj module_new(MpObj file, MpObj name, MpObj code);
void module_free(MpModule*);

MpObj class_new(MpObj name, MpObj module);
MpObj class_new_by_cstr(char* name, MpObj module);
MpInstance* class_instance(MpClass* klass);
void class_format(char* dest, MpObj clazz);
void class_free(MpClass* pclass);
void mp_resolve_code(MpModule* m, const char* code);
void class_set_attr(MpClass* klass, MpObj key, MpObj value);
MpObj mp_call_func_safe(MpObj func, int n, MpObj* args);
MpObj mp_format_instance(MpInstance* instance);

#endif