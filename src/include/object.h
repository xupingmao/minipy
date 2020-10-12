/*
 * object.h
 *
 *  Created on: 2014/8/25
 *  @author: xupingmao
 *  @modified 2020/10/12 23:41:28
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
#define TYPE_CLASS 11

#define TM_TYPE(o) (o).type
#define MAX_FILE_SIZE 1024 * 1024 * 5 /* max file size loaded into memery */
#include <stdint.h>

/* handle like lua */
typedef union TmValue {
  double dv;
  double num;
  int    iv;
  long   lv;
  void*              ptr;
  struct String*     str;
  struct TmList*     list;
  struct TmFunction* func;
  struct TmDict*     dict;
  struct TmModule*   mod;
  struct TmData*     data;
  struct TmRecycle*  gc;
  struct MpClass*    clazz;
}TmValue;


typedef struct Object{
  char type;
  TmValue value;
}Object;


struct TmRecycle {
  int marked;
  /* data */
};

typedef struct _tm_code_cache {
    int op;
    union {
        Object obj;
        int ival;
    } v;
    char* sval;
} TmCodeCache;

typedef struct TmModule
{
  int marked;
  int resolved;
  Object globals;
  Object code;
  Object file;
  TmCodeCache* cache;
  int cache_cap;
  int cache_len;
}TmModule;

typedef struct TmFunction{
  int marked;
  char resolved;
  char modifier;
  int maxlocals;
  int maxstack;
  unsigned char* code;
  unsigned char* end;
  TmCodeCache* cache;
  TmCodeCache* cache_end;
  Object self;
  Object mod; /* module, includes global, constants, etc. */
  Object name;
  Object (*native)();
}TmFunction;

typedef struct MpClass {
  int marked;
  // class name
  Object name;
  // class attributes
  Object attr_dict;
} MpClass;

typedef struct TmFrame {
  Object *locals;
  Object *stack;
  Object *top; /* current stack top; */
  Object *last_top;
  char* last_code;
  
  unsigned char* pc;
  TmCodeCache* cache;

  int stacksize;
  int maxlocals;
  int maxstack;
  int lineno;
  Object fnc;
  jmp_buf buf;
  int idx;
  unsigned char* jmp;
  
  TmCodeCache* cache_jmp;
  
}TmFrame;


#define FRAMES_COUNT 128
#define MAX_ARG_COUNT 10
#define STACK_SIZE 2048
typedef struct TmVm {
  char* version;
  int   debug;

  /* program arguments */
  int    argc;
  char** argv;
  
  /* compiled code */
  unsigned char * code;
  
  int exit_code; /* function call exit code, to recognize exceptions */

  Object ex;
  Object ex_line;
  Object ex_list;
  int    ex_index; /* index of frame where exception was thrown */

  int frames_init_done;
  TmFrame frames[FRAMES_COUNT];
  /* current frame */
  TmFrame *frame;
  
  Object *stack_end;
  Object stack[STACK_SIZE];

  /* for builtin C functions */
  Object internal_arg_stack[MAX_ARG_COUNT];
  
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

  int steps;   /* record opcode steps executed */
  int init;    /* modules and builtins init */

  struct TmList* all;
  struct TmList* local_obj_list;
  int allocated;
  int max_allocated;
  int gc_threshold;
  int gc_state;
  
  /* constants */
  Object _TRUE;
  Object _FALSE;
  
} TmVm;

typedef struct TmData {
    int marked;
    size_t data_size;
    
    /* for iterator */
    long cur;
    long inc;
    long end;
    Object cur_obj;
    Object* (*next)();

    /* for gc */
    void   (*mark)();
    void   (*func_free)();

    /* meta functions */
    Object (*str)();
    Object (*get)();
    void   (*set)();

    void* extend_data_ptr;
    Object data_ptr[1];
}TmData;

/** 
 * definition for list
 */

typedef struct TmList {
  int marked;
  int len;
  int cap;
  struct Object* nodes;
}TmList;


/** 
 * definition for dictionary.
 */

typedef struct DictNode{
  Object key;
  Object val;
  int hash;
  int used; /* also used for attr index */
} DictNode;

typedef struct TmDict {
  int marked;
  int len;
  int cap;
  int extend;
  struct DictNode* nodes;
} TmDict;


typedef struct String {
    int marked;
    int len;
    int stype; /* string type, static or not */
    char *value;
} String;


/**
 * global variables
 * only one.
 */ 
TmVm* tm;

#endif /* OBJECT_H_ */
