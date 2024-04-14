/*
 * @Author: xupingmao 578749341@qq.com
 * @Date: 2024-04-14 16:07:45
 * @LastEditors: xupingmao 578749341@qq.com
 * @LastEditTime: 2024-04-14 19:08:29
 * @FilePath: /minipy/src/gc_debug.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#include "include/mp.h"

static void gc_debug_init();
static void gc_debug_add_pointer(void* block, const char* scene);
static void gc_debug_remove_pointer(void* block);
static void gc_debug_mark();
static void gc_debug_free();

static void gc_debug_init() {
    tm->gc_debug_dict = dict_new_no_gc();
}

static void gc_debug_add_pointer(void* block, const char* scene) {
    if (tm->gc_debug_dict == NULL) {
        return;
    }
    
    // printf("gc_debug_add_pointer start: %p %s\n", block, scene);

    int64_t block_addr = (int64_t) block;
    MpObj pointer = number_obj((double)block_addr);
    MpObj scene_obj = string_const(scene);
    
    // printf("gc_debug_add_pointer step2: %p %s\n", block, scene);

    dict_set0(tm->gc_debug_dict, pointer, scene_obj);

    // printf("gc_debug_add_pointer end: %p %s\n", block, scene);
}

static void gc_debug_remove_pointer(void* block) {
    if (tm->gc_debug_dict == NULL) {
        return;
    }
    // printf("gc_debug_remove_pointer start: %p\n", block);

    int64_t block_addr = (int64_t) block;
    MpObj pointer = number_obj((double)block_addr);
    dict_del(tm->gc_debug_dict, pointer);

    // printf("gc_debug_remove_pointer end: %p\n", block);
}

static void gc_debug_mark() {
    if (tm->gc_debug_dict == NULL) {
        return;
    }
    gc_mark_dict(tm->gc_debug_dict);
}

static void gc_debug_free() {
    if (tm->gc_debug_dict == NULL) {
        return;
    }
    dict_free_internal(tm->gc_debug_dict);
    tm->gc_debug_dict = NULL;
}

static void gc_debug_print() {
    if (tm->gc_debug_dict == NULL) {
        return;
    }

    dict_print_debug_info(tm->gc_debug_dict);
}