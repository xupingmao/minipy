#ifndef _FRAME_H
#define _FRAME_H

#include "tm.h"
#include "object.h"


typedef struct _FunctionDefine {
    Object fnc;
    int len;
} FunctionDefine;

#define TM_PUSH(x) *(++top) = (x); if(top > tm_stack_end) tmRaise("stack overflow");
#define TM_POP() *(top--)
#define TM_TOP() (*top)
#define GET_CONST(i) GET_DICT(tm->constants)->nodes[i].key
Object  callUnsafe(Object fnc);
Object callFunction(Object func);
Object  tm_eval(TmFrame*);

TmFrame* pushFrame(Object fnc);
void popFrame();

#endif
