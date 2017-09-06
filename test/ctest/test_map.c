#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../../src/include/map.h"

DEF_MAP(Map, int, int)


void test_map_get(Map* m, int key) {
    int* v = Map_get(m, key);
    if (v == NULL) {
        printf("map[%d]=NULL\n", key);
    } else {
        printf("map[%d]=%d\n", key, *v);
    }
}

int main(int argc, char const *argv[])
{
    Map* m = Map_new();

    Map_set(m, 10, 5);
    Map_set(m, 20, 11);

    int i;
    for (i = 11; i < 100; i++) {
        Map_set(m, i, i);
    }

    Map_del(m, 12);
    Map_set(m, 13, 20);

    assert(Map_get(m, 1) == NULL);
    assert(*Map_get(m, 10) == 5);
    assert(*Map_get(m, 13) == 20);
    assert(*Map_get(m, 16) == 16);
    assert(*Map_get(m, 20) == 20);
    assert(Map_get(m, 12) == NULL);
    
    test_map_get(m, 1);
    test_map_get(m, 10);
    test_map_get(m, 13);
    test_map_get(m, 16);
    test_map_get(m, 20);
    test_map_get(m, 12);

    Map_free(m);
    return 0;
}