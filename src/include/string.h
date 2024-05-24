/*
 * @Author: xupingmao 578749341@qq.com
 * @Date: 2024-04-14 12:29:46
 * @LastEditors: xupingmao
 * @LastEditTime: 2024-05-25 01:42:06
 * @FilePath: /minipy/src/include/string.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef MP_STRING_H
#define MP_STRING_H

#include "object.h"

/**
 * string functions
 */
MpObj        string_char_new(int c);
MpObj        string_chr(int n); // get a char from char_list.
MpObj        string_alloc(char* s, int size);
MpObj        string_new(char*s);
MpObj        string_static(const char*s);
MpObj        string_from_cstr(const char*);
MpObj        string_const(const char*);
void         string_free(MpStr*);
int          string_equals(MpStr*s0, MpStr*s1);
MpObj        string_substring(MpStr* str, int start, int end) ;
void         string_methods_init();
MpObj        string_iter_new(MpObj s);
MpObj*       string_next(MpData* iterator);
int          string_hash(MpStr* str);

#endif
