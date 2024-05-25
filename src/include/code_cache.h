#ifndef MP_CODE_CACHE_H
#define MP_CODE_CACHE_H

#include "object.h"
#include "mp.h"

static int get_cache_int(MpCodeCache* cache) {
    return (((int)cache->a)<<8) + ((int)cache->b);
}

static void set_cache_int(MpCodeCache* cache, int value) {
    cache->a = (value>>8) & 0xff;
    cache->b = value & 0xff;
}

static MpObj get_cache_obj(MpCodeCache* cache) {
    int index = get_cache_int(cache);
    return mp_get_constant(index);
}


#endif
