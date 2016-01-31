/**
 * Created : 2015/1/22
 * Author: Xu
 * Email: 578749341@qq.com
 */
#ifndef _TM_STRING_H
#define _TM_STRING_H

#include "object.h"
#include "tmdata.h"

typedef struct {
    char *value;
    int len;
}Chars;

typedef struct String {
    int marked;
    int len;
    int stype; /* string type, static or not */
    char *value;
} String;

typedef struct StringIterator {
    DATA_HEAD
    int cur;
    String* string;
}StringIterator;

Object strinGET_CHAR(int c);
Object stringAlloc(char* s, int size);
#define szToString(s) stringAlloc(s, -1)
#define stringNew(s) stringAlloc(s, strlen(s))
void string_free(String*);
int StringEquals(String*s0, String*s1);

Object string_substring(String* str, int start, int end) ;
Object bf_stringFormat();
Object tmStr(Object obj);
Object string_chr(int n);
Object StringJoin(Object self, Object list);
void stringMethodsInit();

static DataProto stringProto;
DataProto*  getStringProto();
Object         stringIterNew(String* s);
Object*     stringNext(StringIterator* iterator);

/* macros */
#define GET_STR(obj) (obj).value.str->value
#define GET_STR_OBJ(obj) (obj).value.str
#define GET_STR_LEN(obj) (obj).value.str->len

#endif
