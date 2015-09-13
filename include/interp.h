#ifndef _FRAME_H
#define _FRAME_H

#include "tm.h"
#include "object.h"


typedef struct _FunctionDefine {
	Object fnc;
	int len;
} FunctionDefine;

/*
struct _Sopcode {
    char type;
    int value;
};

typedef union _tm_opcode {
	char* str;
	double* number;
	struct _Sopcode opcode;
} tm_opcode;

typedef struct _GlobalCacheNode {
    int idx;
    DictNode* val;
} GlobalCacheNode;

typedef struct _GlobalCache {
    struct GlobalCacheNode* nodes;
    Object globals;
} GlobalCache;
*/
#define TM_PUSH(x) *(++top) = (x); if(top > tm_stack_end) tmRaise("stack overflow");
#define TM_POP() *(top--)
#define TM_TOP() (*top)
#define GET_CONST(i) GET_DICT(tm->constants)->nodes[i].key
/*Object* defineModuleConstant(Object fnc, Object v) ;*/
Object  callUnsafe(Object fnc);
void popFrame();
Object callFunction(Object func);
Object callFunction2(Object func);
Object  tmEval(TmFrame*);

TmFrame*         pushFrame(Object fnc);
Object getInstrDesc(unsigned char* pc, int count);

#endif
