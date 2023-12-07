#ifndef _MP_GC_H_
#define _MP_GC_H_

#include "object.h"

// gc functions
#define GC_DEBUG_LIST 0
void*       mp_malloc(size_t size);
void*       mp_realloc(void* o, size_t osize, size_t nsize);
void        mp_free(void* o, size_t size);
void        init_memory();
void        free_memory();

void        gc_init();
MpObj       gc_track(MpObj obj);
void        gc_destroy();
void        gc_full();
void        gc_sweep_local(int start);
void        gc_mark(MpObj);
void        gc_unmark(MpObj);
void        gc_mark_single(MpObj);
const char* gc_mark_list(MpList*);
void        gc_mark_dict(MpDict*);
void        gc_restore_local_obj_list(int size);
void        gc_native_call_sweep();
void        gc_check_native_call(int size, MpObj ret);
void gc_mark_and_check(MpObj, const char*);

#endif
