#include <assert.h>
#include "include/mp.h"

MpObj mp_ptr_obj(void* ptr) {
    MpObj result;
    result.type = TYPE_PTR;
    result.value.ptr = ptr;
    return result;
}