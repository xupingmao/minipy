#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef T_KEY
    #define T_KEY int
#endif
#ifndef T_VALUE
    #define T_VALUE int
#endif

typedef struct _Smap {
    int len;
    int cap;
    T_KEY* keys;
    T_VALUE* values;
} Smap;

Smap* map_new() {
    Smap* map = malloc(sizeof(Smap));
    map->len = 0;
    map->cap = 10;
    map->keys = malloc(sizeof(T_KEY) * map->cap);
    map->values = malloc(sizeof(T_VALUE) * map->cap);
    return map;
}

void map_free(Smap* map) {
    free(map->keys);
    free(map->values);
    free(map);
}

int map_index(Smap* map,T_KEY key) {
    int i;
    for(i = 0; i < map->len; i++) {
        #ifdef SMAP_EQL
        if (SMAP_EQL(map->keys[i], key)) {
        #else
        if (map->keys[i] == key) {
        #endif
            return i;
        }
    }
    return -1;
}

T_VALUE* map_get(Smap* map, T_KEY key) {
    int i = map_index(map, key);
    if (i < 0) {
        return NULL;
    } else {
        return &map->values[i];
    }
}

void map_set(Smap* map, T_KEY key, T_VALUE value) {
    int i = map_index(map, key);
    if (i < 0) {
        map->cap += 10;
        map->keys = realloc(map->keys, sizeof(T_KEY) * map->cap);
        map->values = realloc(map->values, sizeof(T_VALUE) * map->cap);
        map->keys[map->len] = key;
        map->values[map->len] = value;
        map->len += 1;
    } else {
        map->values[i] = value;
    }
}

int main(int argc, char* argv[]) {
    Smap* map = map_new();
    map_set(map, 1, 10);
    map_set(map, 2, 12);
    map_set(map, 1, -1);
    int i;
    for (i = 0; i < map->len; i++) {
        printf("%d=%d\n", map->keys[i], map->values[i]);
    }
    map_free(map);
}

