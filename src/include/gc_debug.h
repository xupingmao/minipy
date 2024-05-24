#ifndef MP_GC_DEBUG_H
#define MP_GC_DEBUG_H

#include "object.h"


void gc_debug_init();
void gc_debug_malloc(void* block, const char* scene, size_t size);
void gc_debug_mark();
void gc_debug_free(void* block, size_t size);
void gc_debug_obj_track(MpObj obj);
void gc_debug_str_value(char* value);

#endif