/*
 * object.h
 *
 *  Created on: 2014/8/25
 *  @author: xupingmao
 *  @modified 2022/01/13 00:16:59
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

#define MP_TYPE(o) (o).type
#define MAX_FILE_SIZE 1024 * 1024 * 5 /* max file size loaded into memery */
#include <stdint.h>

/* handle like lua */
typedef union MpValue {
  double dv;
  double num;
  int    iv;
  long   lv;
  void*              ptr;
  struct MpStr*      str;
  struct MpList*     list;
  struct MpFunction* func;
  struct MpDict*     dict;
  struct MpModule*   mod;
  struct MpData*     data;
  struct MpRecycle*  gc;
  struct MpClass*    clazz;
}MpValue;


typedef struct MpObj{
  char type;
  MpValue value;
}MpObj;


struct MpRecycle {
  int marked;
  /* data */
};

typedef struct _mp_code_cache {
    int op;
    union {
        MpObj obj;
        int ival;
    } v;
    char* sval;
} MpCodeCache;

typedef struct MpModule
{
  int marked;
  int resolved;
  MpObj globals;
  MpObj code;
  MpObj file;
  MpCodeCache* cache;
  int cache_cap;
  int cache_len;
}MpModule;

typedef struct MpFunction{
  int marked;
  char resolved;
  char modifier;
  int maxlocals;
  int maxstack;
  unsigned char* code;
  unsigned char* end;
  MpCodeCache* cache;
  MpCodeCache* cache_end;
  MpObj self;
  MpObj mod; /* module, includes global, constants, etc. */
  MpObj name;
  MpObj (*native)();
}MpFunction;

typedef struct MpClass {
  int marked;
  // class name
  MpObj name;
  // class attributes
  MpObj attr_dict;
} MpClass;

typedef struct MpFrame {
  MpObj *locals;
  MpObj *stack;
  MpObj *top; /* current stack top; */
  MpObj *last_top;
  char* last_code;
  
  unsigned char* pc;
  MpCodeCache* cache;

  int stacksize;
  int maxlocals;
  int maxstack;
  int lineno;
  MpObj fnc;
  jmp_buf buf;
  int idx;
  unsigned char* jmp;
  
  MpCodeCache* cache_jmp;
  
}MpFrame;


#define FRAMES_COUNT 128
#define MAX_ARG_COUNT 10
#define STACK_SIZE 2048
typedef struct MpVm {
  char* version;
  int   debug;

  /* program arguments */
  int    argc;
  char** argv;
  
  /* compiled code */
  unsigned char * code;
  
  int exit_code; /* function call exit code, to recognize exceptions */

  MpObj ex;
  MpObj ex_line;
  MpObj ex_list;
  int   ex_index; /* index of frame where exception was thrown */

  int     frames_init_done;
  MpFrame frames[FRAMES_COUNT];

  /* current frame */
  MpFrame *frame;
  
  MpObj *stack_end;
  MpObj stack[STACK_SIZE];

  /* for builtin C functions */
  MpObj internal_arg_stack[MAX_ARG_COUNT];
  
  /* MpObj *top; */
  MpObj *arguments;
  
  // prototypes
  MpObj list_proto;
  MpObj dict_proto;
  MpObj str_proto;

  int arg_cnt;
  int arg_loaded;

  MpObj constants;  /* type: dict */
  MpObj modules;    /* type: dict */
  MpObj builtins;   /* type: dict */
  MpObj root;       /* type: list */

  int steps;   /* record opcode steps executed */
  int init;    /* modules and builtins init */

  struct MpList* all;
  struct MpList* local_obj_list;
  int allocated;
  int max_allocated;
  int gc_threshold;
  int gc_state;
  
  /* constants */
  MpObj _TRUE;
  MpObj _FALSE;
  
} MpVm;

typedef struct MpData {
    int marked;
    size_t data_size;
    
    /* for iterator */
    long cur;
    long inc;
    long end;
    MpObj cur_obj;
    MpObj* (*next)();

    /* for gc */
    void   (*mark)();
    void   (*func_free)();

    /* meta functions */
    MpObj (*str)();
    MpObj (*get)();
    void  (*set)();

    void* extend_data_ptr;
    MpObj data_ptr[1];
}MpData;

/** 
 * definition for list
 */

typedef struct MpList {
  int marked;
  int len;
  int cap;
  struct MpObj* nodes;
}MpList;


/** 
 * definition for dictionary.
 */

typedef struct DictNode{
  MpObj key;
  MpObj val;
  int hash;
  int used; /* also used for attr index */
} DictNode;

typedef struct MpDict {
  int marked;
  int len;
  int cap;
  int extend;
  struct DictNode* nodes;
  /** hash冲突的节点 **/
  struct DictNode* conflict_nodes;
} MpDict;


typedef struct MpStr {
    int marked;
    int len;
    int stype; /* string type, 1: memory; 0: static */
    char *value;
} MpStr;


/**
 * global variables
 * only one.
 */ 
MpVm* tm;

#endif /* OBJECT_H_ */
