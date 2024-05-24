/*
 * @Author: xupingmao 578749341@qq.com
 * @Date: 2024-04-14 16:07:45
 * @LastEditors: xupingmao
 * @LastEditTime: 2024-05-25 00:23:55
 * @FilePath: /minipy/src/gc_debug.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#include "include/mp.h"
#include "include/mp_log.h"

static void gc_debug_init();
static void gc_debug_malloc(void* block, const char* scene, size_t size);
static void gc_debug_mark();
static void gc_debug_free(void* block, size_t size);

static void gc_debug_init() {
}

static void gc_debug_malloc(void* block, const char* scene, size_t size) {
    // printf("gc_debug_add_pointer step2: %p %s\n", block, scene);
    log_debug("malloc:%p,scene:%s,size:%lld", block, scene, size);
}

static void gc_debug_mark() {
}

static void gc_debug_free(void* block, size_t size) {
    log_debug("free:%p,size:%lld", block, size);
}

static void gc_debug_print() {
}