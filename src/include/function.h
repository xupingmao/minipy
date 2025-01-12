/*
 * @Author: xupingmao
 * @email: 578749341@qq.com
 * @Date: 2023-12-07 22:35:37
 * @LastEditors: xupingmao
 * @LastEditTime: 2024-06-02 17:23:42
 * @FilePath: /minipy/src/include/function.h
 * @Description: minipy函数的定义
 */
#ifndef MP_FUNCTION_H
#define MP_FUNCTION_H

#include "object.h"

// function functions
MpObj func_new(MpModule* mod, MpObj self, MpNativeFunc);
MpFunction* mp_new_native_func(MpModule* module, MpNativeFunc native_func);
MpObj mp_new_native_func_obj(MpModule* module, MpNativeFunc native_func);

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
MpModule* module_new(MpObj file, MpObj name, MpObj code);
void module_free(MpModule*);


#endif