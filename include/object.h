/*
 * object.h
 *
 *  Created on: 2014/8/25
 *      Author: Xu
 */

#ifndef _OBJECT_H_
#define _OBJECT_H_

#define TYPE_STR 1
#define TYPE_NUM 2
#define TYPE_LIST 3
#define TYPE_DICT 4
#define TYPE_FUNCTION 5
#define TYPE_NONE 7
#define TYPE_MODULE 9
#define TYPE_DATA 10
#define TM_TYPE(o) (o).type
#define MAX_FILE_SIZE 1024 * 1024 * 5 /* max file size loaded into memery */

typedef union TmValue {
  double dv;
  double num;
  int iv;
  long lv;
  void*              ptr;
  struct String*     str;
  struct TmList*     list;
  struct TmFunction* func;
  struct Dictonary*  dict;
  struct TmModule*   mod;
  struct TmData*     data;
  struct GC_Object*  gc;
}TmValue;

#define GET_LIST(obj) GET_VAL(obj).list

typedef struct Object{
  int type; /*marked, type, others*/
  TmValue value;
}Object;

#include "tmstring.h"

struct  GC_Object {
  int marked;
  /* data */
};

typedef struct TmModule
{
  int marked;
  int resolved;
  Object globals;
  Object code;
  Object file;
}TmModule;

typedef struct TmFunction{
  int marked;
  char resolved;
  char modifier;
  int maxlocals;
  int maxstack;
  unsigned char* code;
  unsigned char* end;
  Object self;
  Object mod; /* module, includes global, constants, etc. */
  Object name;
  Object (*native)();
}TmFunction;

typedef struct TmFrame {
  Object *locals;
  Object *stack;
  Object *top; /* current stack top; */
  Object *last_pc;
  char* last_code;
  
  unsigned char* pc;

  int stacksize;
  int maxlocals;
  int maxstack;
  int lineno;
  Object fnc;
  jmp_buf buf;
  int idx;
  unsigned char* jmp;
}TmFrame;

struct _tm_jmp_buf {
    struct _tm_jmp_buf *prev;
    jmp_buf b;
    TmFrame* f;
};

typedef struct _tm_jmp_buf tm_jmp_buf;

#define FRAMES_COUNT 128
#define MAX_ARG_COUNT 10
#define STACK_SIZE 1024
typedef struct TmVM {
  char* version;
  int debug;
  
  int exitCode; /* function call exit code, to recognize exceptions */

  Object ex;
  Object exList;

  int exIndex; /* index of frame where exception was thrown */
  int framesInitDone;
  TmFrame frames[FRAMES_COUNT];
  TmFrame *frame;
  
  Object stack[STACK_SIZE];
  Object internalArgStack[MAX_ARG_COUNT];
  /* Object *top; */
  Object *arguments;

  
  int arg_cnt;
  int arg_loaded;

  Object constants;
  Object modules;
  Object builtins;
  Object root;
  int steps;  
  int init; /* modules and builtins init */

  struct TmList* all;
  int allocated;
  int gcThreshold;

}TmVM;

/**
 * global virtual machine
 */
TmVM* tm;
Object* tm_stack_end;

int              objEqSz(Object str, const char* value);
void             tmRaise(char*fmt , ...);
#include "tmdata.h"
#include "tmdict.h"
#include "tmlist.h"
#include "function.h"

#define IS_NONE(obj) TM_TYPE(obj) == TYPE_NONE
#define IS_LIST(obj) TM_TYPE(obj) == TYPE_LIST
#define IS_FUNC(obj) TM_TYPE(obj) == TYPE_FUNCTION
#define IS_STR(obj)  TM_TYPE(obj) == TYPE_STR
#define IS_NUM(obj)  TM_TYPE(obj) == TYPE_NUM
#define IS_DATA(obj) TM_TYPE(obj) == TYPE_DATA
#define IS_NATIVE(obj) GET_FUNCTION(obj)->native != NULL

#define NOT_LIST(obj) TM_TYPE(obj) != TYPE_LIST
#define NOT_DICT(obj) TM_TYPE(obj) != TYPE_DICT
#define NOT_FUNC(obj) TM_TYPE(obj) != TYPE_FUNCTION
#define NOT_STR(obj)  TM_TYPE(obj) != TYPE_STR
#define NOT_NATIVE(obj) GET_FUNCTION(obj)->native == NULL

#endif /* OBJECT_H_ */
