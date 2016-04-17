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
#include <stdint.h>

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

#define USE_IDX 0

#if USE_IDX
    #define SET_IDX(obj, v) (obj).idx = (v)
#else
    #define SET_IDX(obj, v) /**/
#endif

typedef struct Object{
  char type;
  #if USE_IDX
  char idx;
  #endif
  TmValue value;
}Object;

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
  Object *last_top;
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
#define STACK_SIZE 2048
typedef struct TmVM {
  char* version;
  int debug;
  
  int exitCode; /* function call exit code, to recognize exceptions */

  Object ex;
  Object exLine;
  Object exList;

  int exIndex; /* index of frame where exception was thrown */
  int framesInitDone;
  TmFrame frames[FRAMES_COUNT];
  TmFrame *frame;
  
  Object stack[STACK_SIZE];
  Object internalArgStack[MAX_ARG_COUNT];
  /* Object *top; */
  Object *arguments;
  
  // prototypes
  Object list_proto;
  Object dict_proto;
  Object str_proto;

  
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
  int maxAllocated;
  int gcThreshold;
  int gcState;

}TmVM;

/** 
 * definition for data
 */


#define DATA_OBJECT_HEAD     \
    int marked;              \
    size_t dataSize;         \
    int init;                \
    void (*mark)();          \
    void (*free)();          \
    Object (*str)();         \
    Object (*get)();         \
    void (*set)();           \
    Object* (*next)();

typedef struct DataProto {
    DATA_OBJECT_HEAD
}DataProto;

#define DATA_HEAD    int marked;      \
    DataProto* proto;

typedef struct TmData {
    DATA_HEAD
}TmData;

typedef struct DataObject {
  DATA_OBJECT_HEAD
}DataObject;

typedef struct _TmBaseIterator {
    DATA_HEAD
    Object func;
  Object ret;
}TmBaseIterator;

/** 
 * definition for list
 */

typedef struct TmList {
  int marked;
  int len;
  int cap;
  int cur;
  struct Object* nodes;
}TmList;

typedef struct TmListIterator {
    DATA_HEAD
    TmList* list;
    int cur;
}TmListIterator;


/** 
 * definition for dictionary.
 */

typedef struct DictNode{
  Object key;
  Object val;
  int hash;
  int used; /* also used for attr index */
}DictNode;

typedef struct Dictonary {
  int marked;
  int len;
  int cap;
  int extend;
  struct DictNode* nodes;
}TmDict;

typedef struct _TmDictIterator {
    DATA_HEAD
    TmDict* dict;
    int idx;
} TmDictIterator;


/**
 * definition of string
 */
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


/**
 * global variables
 *
 */ 
TmVM* tm;
Object* tm_stack_end;



#endif /* OBJECT_H_ */
