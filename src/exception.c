/**
 * description here
 * @author xupingmao
 * @since 2016
 * @modified 2022/06/05 19:43:28
 */

#include "include/mp.h"
#include <setjmp.h>

void mp_push_exception(MpFrame* f){
    MpObj file = func_get_file_name_obj(f->fnc);
    MpObj fnc_name = func_get_name_obj(f->fnc);
    MpObj ex = mp_format("  File %o: in %o , at line %d", file, fnc_name,
            f->lineno);
    list_append(GET_LIST(tm->ex_list), ex);
}

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
    longjmp(tm->frame->buf, 1);
}