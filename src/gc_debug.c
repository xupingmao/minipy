/*
 * @Author: xupingmao 578749341@qq.com
 * @Date: 2024-04-14 16:07:45
 * @LastEditors: xupingmao
 * @LastEditTime: 2024-05-25 06:43:46
 * @FilePath: /minipy/src/gc_debug.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#include "include/mp.h"
#include "include/mp_log.h"

void gc_debug_init() {
}

void gc_debug_malloc(void* block, const char* scene, size_t size) {
    // printf("gc_debug_add_pointer step2: %p %s\n", block, scene);
    log_debug("malloc:%p,scene:%s,size:%d", block, scene, size);
}

void gc_debug_str_value(char* value) {
    char buf[51];
    strncpy(buf, value, 50);
    buf[50] = 0;
    log_debug("str_ptr:%p,str_value:%s", value, buf);
}

void gc_debug_mark() {
}

void gc_debug_free(void* block, size_t size) {
    log_debug("free:%p,size:%d", block, size);
}

void gc_debug_print() {
}

void gc_debug_obj_track(MpObj obj) {
    if (IS_STR(obj)) {
        log_debug("track:%p", obj.value.str);
        if (obj.value.str->stype == STR_TYPE_DYNAMIC) {
            log_debug("track:%p", STR_TO_CSTR(obj));
        }
    }

    if (IS_DICT(obj)) {
        log_debug("track:%p", obj.value.dict);
        log_debug("track:%p", obj.value.dict->nodes);
        log_debug("track:%p", obj.value.dict->extend);
    }

    if (IS_LIST(obj)) {
        log_debug("track:%p", obj.value.list);
        log_debug("track:%p", obj.value.list->nodes);
    }

    if (IS_FUNC(obj)) {
        log_debug("track:%p", obj.value.func);
    }
}