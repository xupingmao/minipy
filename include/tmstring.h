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
	int stype; /* 字符串类型 */
	char *value;
} String;

typedef struct StringIterator {
	DATA_HEAD
	int cur;
	String* string;
}StringIterator;

Object newChar(int c);
Object newString0(char* s, int size);
#define staticString(s) newString0(s, -1)
#define newString(s) newString0(s, strlen(s))
void StringFree(String*);
int StringEquals(String*s0, String*s1);

Object subString(String* str, int start, int end) ;
Object bfStringFormat();
Object tmStr(Object obj);
Object tmChr(int n);
Object StringJoin(Object self, Object list);
void regStringMethods();

static DataProto stringProto;
DataProto*  getStringProto();
Object 		stringIterNew(String* s);
Object* 	stringNext(StringIterator* iterator);

/* 宏函数 */
#define GET_STR(obj) (obj).value.str->value
#define GET_STR_OBJ(obj) (obj).value.str
#define GET_STR_LEN(obj) (obj).value.str->len

#endif
