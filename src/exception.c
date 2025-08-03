/**
 * description here
 * @author xupingmao
 * @since 2016
 * @modified 2022/06/06 23:01:21
 */

#include "include/mp.h"
#include <setjmp.h>

void mp_traceback() {
    int i;
    MpObj exlist = tm->ex_list;
    printf("Traceback (most recent call last):\n");
    int cur = tm->frame - tm->frames;
    for (i = LIST_LEN(exlist) - 1; i >= cur; i--) {
        mp_println(LIST_NODES(exlist)[i]);
    }
    fprintf(stderr, "%s\n", GET_CSTR(tm->ex_line));
}

static int get_lineno() {
    if (tm->mp2c_mode == TRUE) {
        return tm->mp2c_lineno;
    }
    return tm->frame->lineno;
}

void mp_panic(char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    exit(1);
}

void mp_raise(char* fmt, ...) {
    va_list a;
    va_start(a, fmt);
    list_clear(GET_LIST(tm->ex_list));
    tm->ex = mp_format_va_list(fmt, a, 0);
    MpObj file = func_get_file_name_obj(tm->frame->fnc);
    MpObj fnc_name = func_get_name_obj(tm->frame->fnc);
    tm->ex_line = mp_format("  File %o: in %o at line %d\n%os", 
        file, fnc_name, get_lineno(), tm->ex);
    va_end(a);

    #ifdef RECORD_LAST_OP
        MpObj last_op_queue = CodeQueue_ToString(&tm->last_op_queue);
        tm->ex_line = mp_format("%os\n\n%os", tm->ex_line, last_op_queue);
    #endif

    longjmp(tm->frame->buf, 1);
}