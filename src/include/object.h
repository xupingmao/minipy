/*
 * object.h
 *
 *  Created on: 2014/8/25
 *  @author: xupingmao
 *  @modified 2022/06/11 20:49:45
 */

#ifndef _OBJECT_H_
#define _OBJECT_H_

#define TYPE_STR 1
#define TYPE_NUM 2
#define TYPE_LIST 3
#define TYPE_DICT 4
#define TYPE_FUNCTION 5
#define TYPE_NONE 6
#define TYPE_MODULE 7
#define TYPE_DATA 8
#define TYPE_CLASS 9
#define TYPE_PTR 10 // 指针类型

#define TYPE_MIN 1
#define TYPE_MAX 9

#define MP_TYPE(o) (o).type
#define MAX_FILE_SIZE 1024 * 1024 * 5 /* max file size loaded into memery */
#include <stdint.h>

#ifndef BOOL
  #define BOOL char
#endif

#define TRUE  1
#define FALSE 0

/* handle like lua */
typedef union MpValue {
  double dv;
  double num;
  int    iv;
  long   lv;
  void*              ptr;
  struct MpStr*      str;
  struct MpConstStr* const_str;
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
} MpObj;


struct MpRecycle {
  int marked;
  /* data */
};


#define CACHE_VTYPE_DEFAULT 0
#define CACHE_VTYPE_STR 1
#define CACHE_VTYPE_INT 2
#define CACHE_VTYPE_OBJ 3

typedef struct _mp_code_cache {
    char op;
    char vtype; /* value type: CACHE_VTYPE_XXX */
    int index;
    char* sval; /* string value */

    union {
        MpObj obj;
        int ival;
    } v;

    #if MP_PROFILE
      // 性能分析埋点
      int64_t start_time;
      int64_t total_time;
      int64_t times;
    #endif
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
  // type: None or MpModule
  // desc: Includes globals, constants, etc.
  MpObj mod; 
  MpObj name;
  MpObj (*native)();
} MpFunction;

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

#ifdef RECORD_LAST_OP
  typedef struct _codeQueue_t {
    int size;
    int start;
    int cap;
    MpCodeCache data[20];
  } CodeQueue;
#endif

typedef struct _debug_info_t {
    const char * source;
} DebugInfo;



/** 
 * definition for dictionary.
 */

typedef struct DictNode{
  MpObj key;
  MpObj val;
  int hash;
  // used值说明
  // 0: 未使用 >0:正常使用 -1:被删除
  int used; /* also used for attr index */
} DictNode;

typedef struct MpDict {
  int marked;
  int len;
  int cap;
  int slot_cap;
  int extend;
  // hash的掩码
  int mask;
  // 存放数据的节点
  DictNode* nodes;
  // 存放索引的数据
  int* slots;
  // 空闲节点的开始索引
  int free_start;
} MpDict;


#define FRAMES_COUNT 128
#define MAX_ARG_COUNT 10
#define STACK_SIZE 2048

typedef struct MpVm {
  char* version;
  int   debug;

  // mp2c模式
  BOOL  mp2c_mode;
  int   mp2c_lineno;

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
  
  /* prototypes */
  MpObj list_proto;
  MpObj dict_proto;
  MpObj str_proto;

  int arg_cnt;
  int arg_loaded;

  MpDict* constants;  /* type: dict */
  MpObj modules;      /* type: dict */
  MpObj builtins;     /* type: dict */
  MpObj root;         /* type: list */
  MpObj builtins_mod; /* 内置模块 */

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

  /* 一些内部信息 */
  int vm_size;

  #ifdef RECORD_LAST_OP
    /* 最后的指令队列 */
    CodeQueue last_op_queue;
  #endif

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
    MpObj (*str)(struct MpData*);
    MpObj (*get)(struct MpData*, MpObj);
    void  (*set)(struct MpData*, MpObj, MpObj);

    MpObj data_ptr[1]; // 这个大小是由data_new的参数决定的，使用数组是为了使内存布局更紧凑
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


typedef struct MpStr {
    int marked;
    int stype; /* string type, 1: memory; 0: static */
    int len;
    // 字符串的哈希值
    int hash;
    char *value;
} MpStr;

typedef struct MpConstStr {
    int marked;
    int stype;
    int len;
    // 字符串的哈希值
    int hash;
    const char *value;
} MpConstStr;


/**
 * global variables
 * only one.
 */ 
MpVm* tm;

MpObj mp_ptr_obj(void*ptr);

#endif /* OBJECT_H_ */
