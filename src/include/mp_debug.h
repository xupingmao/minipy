/**
 * 调试专用
 * @author xupingmao
 * @since 2022/06/05 23:28:29
 * @modified 2022/06/11 19:17:03
 */

#ifndef MP_DEBUG_H
#define MP_DEBUG_H

#define MP_DEBUG_FUNC_RESOLVE 1

#ifdef RECORD_LAST_OP

CodeQueue* CodeQueue_Init(CodeQueue* queue, int cap);
void  CodeQueue_Append(CodeQueue* queue, MpCodeCache cache);
MpObj CodeQueue_ToString(CodeQueue* queue);

#endif



#define STACK_TRACE_CAP 10
typedef struct _stack_trace_t {
  int size;
  int start;
  const char data[STACK_TRACE_CAP][1024];
} StackTrace;

void StackTrace_Init(StackTrace *st);
void StackTrace_Append(StackTrace *st, const char* msg);
void StackTrace_Print(StackTrace *st);

#endif
