#include "include/tm.h"
#include "include/tmdict.h"
#include "include/ops.h"


typedef struct _Smap {
    int len;
    int cap;
    int* keys;
    int* values;
} Smap;
void mapAdd(Smap* map, int key, int value);

Smap* mapNew() {
    Smap* map = malloc(sizeof(Smap));
    map->len = 0;
    map->cap = 10;
    map->keys = malloc(sizeof(int) * map->cap);
    map->values = malloc(sizeof(int) * map->cap);
    return map;
}

void mapFree(Smap* map) {
    free(map->keys);
    free(map->values);
    free(map);
}

int mapGetIdx(Smap* map,int key) {
    int i;
    for(i = 0; i < map->len; i++) {
        if (map->keys[i] == key) {
            return i;
        }
    }
    return -1;
}

int* mapGet(Smap* map, int key) {
    int i = mapGetIdx(map, key);
    if (i < 0) {
        return NULL;
    } else {
        return &map->values[i];
    }
}

void mapSet(Smap* map, int key, int value) {
    int i = mapGetIdx(map, key);
    if (i < 0) {
        mapAdd(map, key, value);
    } else {
        map->values[i] = value;
    }
}

void mapAdd(Smap* map, int key, int value) {
    map->cap += 10;
    map->keys = realloc(map->keys, sizeof(int) * map->cap);
    map->values = realloc(map->values, sizeof(int) * map->cap);
    map->keys[map->len] = key;
    map->values[map->len] = value;
    map->len += 1;
}

Object mapToDict(Smap* map) {
    Object dict = newDict();
    int i;
    for(i = 0; i < map->len; i++) {
        tmSet(dict, newNumber(map->keys[i]), newNumber(map->values[i]));
    }
    return dict;
}

