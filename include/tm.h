

#ifndef _TM_H
#define _TM_H
/** 
 *  coding standards
 *  static variable . not specified
 *  global variable . like _XXX
 *  function        . camel case starts with lower case
 *  structure       . camel case starts with upper case
 *  macro           . like XXX_XXX
*/

/* #pragma pack(4) */
#define DEBUG_GC 0
#define PRINT_INS 0
#define PRINT_INS_CONST 0
#define EVAL_DEBUG 1
#define DEBUG_INS 1
#define LOG_LEVEL 1
#define USE_NON_PARAM 1
#define RAISE_DEBUG 0

#ifdef _WIN32
#define TM_NT 1
#endif

#define RET_NONE return NONE_OBJECT;
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdarg.h>
#include <time.h>
/* #include <sys/stat.h> */
#include <math.h>

#define TM_INLINE inline
typedef char instruction;

#include "object.h"
#define OBJ_SIZE sizeof(Object)

Object NUMBER_TRUE;
Object NUMBER_FALSE;
Object NONE_OBJECT;
Object UNDEF;
Object ARRAY_CHARS;


#include "tmtype.h"
#include "tmarg.h"
#include "number.h"
#include "gc.h"
#include "macros.h"
#include "ops.h"
#include "instruction.h"
#include "builtins.h"
#include "code.h"
#include "tmassert.h"

// list functions
void list_clear(TmList* list);


// arg functions
void arg_insert(Object arg);

// ops functions
Object tmCall(Object func, int args, ...);
Object objAppend(Object a, Object b);
Object tmGetGlobal(Object globals, Object key);
Object tmTakeArg();
Object objGetlocal(int fidx, int lidx);
Object objGetstack(int fidx, int sidx);
Object tm_string(char* str);
Object arrayToList(int n, ...);
TmFrame* objGetframe(int fidx);
Object tmGetfname(Object func);
void tmSetattr(Object a, char* key, Object value);
void defFunc(Object globals, Object a, Object(*func)());
void defMethod(Object dict, Object name, Object(*func)());


#define DEBUG(msg) \
    printf("--DEBUG %s\n", msg);

#define DEBUG2(msg1, msg2) \
    printf("--DEBUG %s %s\n", msg1, msg2);
    
#endif

