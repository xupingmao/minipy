/*
 * @Author: xupingmao
 * @email: 578749341@qq.com
 * @Date: 2024-04-14 11:21:10
 * @LastEditors: xupingmao
 * @LastEditTime: 2024-04-14 11:37:41
 * @FilePath: /minipy/src/gc_simple.c
 * @Description: 描述
 */
/**
 * @file gc.c
 * @author 578749341@qq.com
 * @brief GC适配器
 * @version 0.1
 * @date 2024-04-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */

void mp_gc_start() {}
void mp_gc_stop() {}

void* mp_malloc(size_t size, const char* scene) {
    void* block = NULL;
    MpObj* func = NULL;

    if (size <= 0) {
        mp_raise("mp_malloc: attempts to allocate a memory block of size %d!",
                 size);
        return NULL;
    }
    block = malloc(size);
    if (block == NULL) {
        mp_raise("mp_malloc: fail to malloc memory block of size %d", size);
    }

    memset(block, 0, size);

    tm->allocated += size;
    tm->max_allocated = max(tm->max_allocated, tm->allocated);

#ifdef MP_DEBUG
    gc_debug_malloc(block, scene, size);
#endif

    return block;
}


void mp_free(void* ptr, size_t size) {
    if (ptr == NULL) {
        return;
    }

#ifdef MP_DEBUG
    gc_debug_free(ptr, size);
#endif

    memset(ptr, 0, size);

    free(ptr);
    tm->allocated -= size;
}
