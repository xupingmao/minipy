#ifndef MP_ARGUMENT_H
#define MP_ARGUMENT_H

#include "object.h"

typedef struct MpArgument {
    MpObj *arguments;
    int arg_loaded;
    int arg_cnt;
} MpArgument;

MpArgument mp_save_args();
void mp_restore_args(MpArgument arg);

MpBool mp_take_optional_int_arg(int*, const char*);

#endif
