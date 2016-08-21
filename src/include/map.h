
/**
 * define map structure
 * DEF_MAP will generate
 *  - Map Structure
 *  - Map new/free
 *  - Map index/get/set
 * @since 2016-08-21
 */
#define DEF_MAP(MapType,KeyType,ValType)          \
typedef struct t##MapType {                       \
    int len;                                      \
    int cap;                                      \
    KeyType* keys;                                \
    ValType* values;                              \
} MapType;                                        \
MapType* MapType##_new() {                                     \
    MapType* map = malloc(sizeof(MapType));                    \
    map->len = 0;                                        \
    map->cap = 10;                                       \
    map->keys = malloc(sizeof(KeyType) * map->cap);      \
    map->values = malloc(sizeof(ValType) * map->cap);    \
    return map;                                          \
}                                                        \
void MapType##_free(MapType* map) {                      \
    free(map->keys);       \
    free(map->values);     \
    free(map);             \
}                          \
int MapType##_index(MapType* map,KeyType key) {      \
    int i;                                  \
    for(i = 0; i < map->len; i++) {         \
        if (map->keys[i] == key) {          \
            return i;                       \
        }                                   \
    }                                       \
    return -1;                              \
}                                           \
ValType* MapType##_get(MapType* map, KeyType key) {   \
    int i = MapType##_index(map, key);                \
    if (i < 0) {                                      \
        return NULL;                           \
    } else {                                   \
        return &map->values[i];                \
    }                                          \
}                                              \
void MapType##_checksize(MapType* map)  {      \
    if (map->len == map->cap) {                \
        int oldcap = map->cap;                 \
        map->cap += 10;                        \
        map->keys   = realloc(map->keys,   sizeof(KeyType) * map->cap); \
        map->values = realloc(map->values, sizeof(ValType) * map->cap); \
    }                                                                   \
}                                                                       \
void MapType##_set(MapType* map, KeyType key, ValType value) {          \
    int i = MapType##_index(map, key);                                  \
    if (i < 0) {                                                        \
        MapType##_checksize(map);                                       \
        map->keys[map->len] = key;                                      \
        map->values[map->len] = value;                                  \
        map->len += 1;                                                  \
    } else {                                                            \
        map->values[i] = value;                                         \
    }                                                                   \
}

