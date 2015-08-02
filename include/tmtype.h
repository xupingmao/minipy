#ifndef _TM_TYPE_H
#define _TM_TYPE_H
#include "object.h"

const char* getTypeByInt(int type) {
    switch (type) {
    case TYPE_STR:
        return "string";
    case TYPE_NUM:
        return "number";
    case TYPE_LIST:
        return "list";
    case TYPE_FUNCTION:
        return "function";
    case TYPE_DICT:
        return "dict";
    case TYPE_NONE:
        return "none";
    }
    return "unknown";
}

const char* getTypeByObj(Object obj) {
    return getTypeByInt(TM_TYPE(obj));
}

#endif
