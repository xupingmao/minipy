

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

int enable_debug = 0;

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

Object CLASS_STRING;
Object CLASS_LIST;
Object CLASS_DICT;

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

#endif

