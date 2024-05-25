/*
 * @Author: xupingmao 578749341@qq.com
 * @Date: 2024-04-14 12:29:46
 * @LastEditors: xupingmao
 * @LastEditTime: 2024-05-25 16:50:12
 * @FilePath: /minipy/src/include/gc.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef _MP_GC_H_
#define _MP_GC_H_

#include "object.h"

// gc functions
#define GC_DEBUG_LIST 0
void*       mp_malloc(size_t size, const char* scene);
void*       mp_realloc(void* o, size_t osize, size_t nsize, const char* scene);
void        mp_free(void* o, size_t size);
void        init_memory();
void        free_memory();

void        gc_init();
MpObj       gc_track(MpObj obj);
void        gc_destroy();
void        gc_full();
void        gc_mark(MpObj);
void        gc_unmark(MpObj);
void        gc_mark_single(MpObj);
const char* gc_mark_list(MpList*);
void        gc_mark_dict(MpDict*);
void        gc_restore_local_obj_list(int size);
void        gc_native_call_sweep();
void        gc_check_native_call(int size, MpObj ret);
void gc_mark_and_check(MpObj, const char*);
size_t obj_sizeof(MpObj);

#endif
