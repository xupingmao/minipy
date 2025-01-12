#ifndef _MINI_GC
#define _MINI_GC

#include <stdint.h>

void mini_gc_free(void *ptr);
void * mini_gc_malloc(size_t req_size);

void mini_gc_garbage_collect(void);
void mini_gc_init(void);
void mini_gc_add_roots(void * start, void * end);

#endif

