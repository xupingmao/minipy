#ifndef _MP_INLINE_H
#define _MP_INLINE_H

inline
void mp_assert(int value, char* msg) {
    if (!value) {
        mp_raise("assertion failed, %s", msg);
    }
}


#endif