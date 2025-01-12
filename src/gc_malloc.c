/**
 * TODO 还在调试中
 * @file gc_bdwgc.c
 * @author 578749341@qq.com
 * @brief bdwgc的适配器
 * @version 0.1
 * @date 2024-04-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */

// #include "../deps/gc/src/gc.h"
// #include "../deps/gc/src/gc.c"
// #include "../deps/gc/src/log.c"
#include "include/mp.h"
#include "../deps/minigc/gc.h"
#include "../deps/minigc/gc.c"

/**
 * get stack start address
 * from https://github.com/authorNari/minigc/blob/master/gc.c
 */
static void* get_stack_start() {
    long dummy;
    /* referenced bdw-gc mark_rts.c */
    dummy = 42;
    void* stack_start = &dummy;
    return stack_start;
}

void mp_gc_start() {
    mini_gc_init();
    void* root_start = tm;
    void* root_end = tm + sizeof(MpVm);
    mini_gc_add_roots(root_start, root_end);
}

void mp_gc_stop() {
    mini_gc_garbage_collect();
}

void* mp_malloc(size_t size, const char* scene) {
    return mini_gc_malloc(size);
}

void mp_free(void* p, size_t size) {
    mini_gc_free(p);
}