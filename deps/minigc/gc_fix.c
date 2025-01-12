#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

#ifdef _WIN32

void* sbrk(size_t size) {
    void* p = malloc(size);
    if (p == NULL) {
        errno = ENOMEM;
        return (void*)-1;
    }
    return p;
}

#endif
