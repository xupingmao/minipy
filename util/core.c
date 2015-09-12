#include "include/tm.h"

#define DEBUG_MALLOC 0


#if DEBUG_MALLOC
    #include "map.c"
    static Smap* map;
#endif

void initMemory() {
	#if DEBUG_MALLOC
		map = mapNew();
	#endif
}

void freeMemory() {
	#if DEBUG_MALLOC
		mapFree(map);
	#endif
}

Object bfGetMallocInfo() {
#if DEBUG_MALLOC
    if (map == NULL) {
        return newDict();
    }
    return mapToDict(map);
#endif
    return newDict();
}


void* tmMalloc(size_t size) {
    void* block;
    Object* func;

	if (size <= 0) {
		tmRaise("tmMalloc, attempts to allocate a memory block of size %d!", size);
		return NULL;
	}
	block = malloc(size);
#if DEBUG_GC
	printf("%d -> %d , +%d\n", tm->allocated, tm->allocated + size, size);
#endif
	if (block == NULL) {
		tmRaise("tmMalloc: fail to malloc memory block of size %d", size);
	}
#if DEBUG_MALLOC
    int* value = mapGet(map, size);
    if (value == NULL) {
        mapSet(map, size, 1);
    } else {
        mapSet(map, size, *value + 1);
    }
#endif
	tm->allocated += size;
	return block;
}

void* tmRealloc(void* o, size_t osize, size_t nsize) {
	void* block = tmMalloc(nsize);
	memcpy(block, o, osize);
	tmFree(o, osize);
	return block;
}

void tmFree(void* o, size_t size) {
	if (o == NULL)
		return;
#if LIGHT_DEBUG_GC
	printf("Free %p, %d -> %d , -%d\n",o, tm->allocated, tm->allocated - size, size);
	if(size<=0) {
		tmRaise("tmFree: you are free a block of size %d", size);
	}
#endif
	free(o);
	tm->allocated -= size;
}

#define get_char(n) ARRAY_CHARS[n]
