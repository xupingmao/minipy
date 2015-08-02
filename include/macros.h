
#ifndef _MACROS_H
#define _MACROS_H
#include "tm.h"

#define strequals(a, b) a == b || strcmp(a,b) == 0

#define GET_VAL(obj) (obj).value
#define GET_DATA(obj) (obj).value.data
#define GET_DATA_PROTO(obj) (obj).value.data->proto
#define GET_DICT(obj) GET_VAL(obj).dict
#define GET_MODULE(obj) GET_VAL(obj).mod
#define DICT_LEN(obj)  GET_DICT(obj)->len
#define ptr_addr(ptr) (long) (ptr) / sizeof(char*)

#define IS_DICT(o) TM_TYPE(o)==TYPE_DICT

#define ASSERT_TYPE_WITH_INFO(obj, type, info) \
	if(TM_TYPE(obj)!=type){                    \
		tmRaise(info, obj);                    \
	}
/* for instruction read */
#define next_char(s) *s++
#define next_byte(s) *s++
/* gcc process ++ from right to left */
#define next_short(s) (((*s) << 8) + *(s+1));s+=2;
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
#define TM_ASSERT(cond, msg) if(!cond) {tmRaise( msg );}
#endif


