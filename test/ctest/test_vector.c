#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../src/include/vector.h"

DEF_VECTOR(IntVector, int)

#define ASSERT(exp)                    \
    if (exp) {                         \
        printf("PASS -- %s\n", #exp);  \
    } else {                           \
        printf("FAIL -- %s\n", #exp);  \
    }

void print_vec(IntVector* vec) {
    int i;
    for(i=0;i<vec->len;i++) {
        printf("%d\n", * IntVector_get(vec, i));
    }
}

int main(int argc, char const *argv[])
{
    IntVector* int_vec = IntVector_new();
    
    IntVector_push(int_vec, 10);
    IntVector_push(int_vec, 20);
    IntVector_push(int_vec, 15);
    
    print_vec(int_vec);
    
    IntVector_push(int_vec, 10);
    IntVector_push(int_vec, 20);
    IntVector_push(int_vec, 15);
    IntVector_push(int_vec, 12);
    
    print_vec(int_vec);
    
    IntVector_set(int_vec, 2, 10);
    
    ASSERT (*IntVector_get(int_vec, 2) == 10);
    
    IntVector_push(int_vec, 54);
    ASSERT (*IntVector_pop(int_vec) == 54);
    
    IntVector_free(int_vec);
    return 0;
}