#include "include/tm.h"
#include "include/map.h"

/** create a map structure */
DEF_MAP(Smap, void*, int);

static Smap *debug_map = NULL;
static Smap *debug_protect_map = NULL;
static int debug_steps = 0;
static int leaks_count = 0;
/**
 * init debug map
 * @since 2016-08-21
 */
void debug_init() {
    debug_map = Smap_new();
    debug_steps = 0;
}

/**
 * make a summary of memory leak
 * @since 2016-08-21
 */
void debug_summary() {
    printf("leaks_count  = %d\n", leaks_count);
    printf("total_blocks = %d\n", debug_map->len);
}

/**
 * destroy debug map
 * @since 2016-808-21
 */
void debug_destroy() {
    if (debug_map != NULL) {
        debug_summary();
        Smap_free(debug_map);
    }
    if (debug_protect_map != NULL) {
        Smap_free(debug_protect_map);
    }
    debug_map = NULL;
}

/**
 * protect a object by adding it to protect_map
 * @since 2016-08-21
 */
void debug_protect(Object o) {
    if (TM_TYPE(o) == TYPE_NUM || TM_TYPE(o) == TYPE_NONE) {
        return;
    }
    if (debug_protect_map == NULL) {
        debug_protect_map = Smap_new();
    }
    Smap_set(debug_protect_map, GET_PTR(o), 1);
}

/**
 * record memory alloc
 * @since 2016-08-21
 */
void debug_malloc(void*ptr) {
    if (debug_map == NULL) {
        debug_init();
    }
    Smap_set(debug_map, ptr, 1);
    debug_steps += 1;
}


/**
 * check whether there are memory leaks
 * @since 2016-08-21
 */
int debug_free(void* ptr) {
    int* v = Smap_get(debug_map, ptr);
    // if memory is already freed
    if (v == NULL || *v == 0) {
        leaks_count += 1;
        printf("memory leaks!!! addr=%p\n", ptr);
        return 1;
    } else {
        Smap_set(debug_map, ptr, 0);
    }
    debug_steps += 1;
    return 0;
}

/**
 * check whether there are memory leaks, and print object.
 * @since 2016-08-21
 */
void debug_free2(void*ptr, Object obj) {
    if (debug_free(ptr)) {
        // char buf[256];
        // tm_inspect_obj(obj);
        // printf("(DEBUG)");
        // scanf("%s", buf);
    }
}