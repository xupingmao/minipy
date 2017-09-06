/**
 * A very simple map implementation with O(n)
 * define map structure
 * DEF_MAP will generate
 *  - Map Structure
 *  - Map new/free
 *  - Map index/get/set/del
 * @since 2016-08-21
 * @Modified {2017-09-06 22:44:56}
 */
#define DEF_MAP(MapType,KeyType,ValType)                                    \
typedef struct t##MapType##Item {                                           \
    KeyType key;                                                            \
    ValType value;                                                          \
    char flag;                                                              \
} MapType##Item;                                                            \
typedef struct t##MapType {                                                 \
    int len;                                                                \
    int cap;                                                                \
    MapType##Item *items;                                                   \
} MapType;                                                                  \
MapType* MapType##_new() {                                                  \
    MapType* map = malloc(sizeof(MapType));                                 \
    map->len = 0;                                                           \
    map->cap = 10;                                                          \
    map->items = malloc(sizeof(MapType##Item) * map->cap);                  \
    int i = 0;                                                              \
    for (i = 0; i < map->cap; i++) {                                        \
        map->items[i].flag = 0;                                             \
    }                                                                       \
    return map;                                                             \
}                                                                           \
void MapType##_free(MapType* map) {                                         \
    free(map->items);                                                       \
    free(map);                                                              \
}                                                                           \
int MapType##_index(MapType* map,KeyType key) {                             \
    int i;                                                                  \
    for(i = 0; i < map->len; i++) {                                         \
        if (map->items[i].flag && map->items[i].key == key) {               \
            return i;                                                       \
        }                                                                   \
    }                                                                       \
    return -1;                                                              \
}                                                                           \
int MapType##_freepos(MapType* map, KeyType key) {                          \
    int i;                                                                  \
    for(i = 0; i < map->len; i++) {                                         \
        if (map->items[i].flag == 0 || map->items[i].key == key) {          \
            return i;                                                       \
        }                                                                   \
    }                                                                       \
    return -1;                                                              \
}                                                                           \
ValType* MapType##_get(MapType* map, KeyType key) {                         \
    int i = MapType##_index(map, key);                                      \
    if (i < 0) {                                                            \
        return NULL;                                                        \
    } else {                                                                \
        return &(map->items[i].value);                                      \
    }                                                                       \
}                                                                           \
void MapType##_checksize(MapType* map)  {                                   \
    if (map->len == map->cap) {                                             \
        int oldcap = map->cap;                                              \
        map->cap += 10;                                                     \
        map->items   = realloc(map->items,sizeof(MapType##Item) * map->cap);\
        int i = oldcap;                                                     \
        for (; i < map->cap; i++) {                                         \
            map->items[i].flag = 0;                                         \
        }                                                                   \
    }                                                                       \
}                                                                           \
void MapType##_set(MapType* map, KeyType key, ValType value) {              \
    int i = MapType##_freepos(map, key);                                    \
    if (i < 0) {                                                            \
        MapType##_checksize(map);                                           \
        MapType##Item *item = &map->items[map->len];                        \
        item->key = key;                                                    \
        item->value = value;                                                \
        item->flag = 1;                                                     \
        map->len += 1;                                                      \
    } else {                                                                \
        map->items[i].value = value;                                        \
    }                                                                       \
}                                                                           \
void MapType##_del(MapType* map, KeyType key) {                             \
    int i = MapType##_index(map, key);                                      \
    if (i >= 0) {                                                           \
        map->items[i].flag = 0;                                             \
    }                                                                       \
}                                                                           \
