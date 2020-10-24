/**
 * description here
 * @author xupingmao
 * @since 2016
 * @modified 2020/10/11 18:25:35
 */

#include "include/mp.h"
#include <setjmp.h>

void push_exception(MpFrame* f){
    Object file = get_file_name_obj(f->fnc);
    Object fnc_name = get_func_name_obj(f->fnc);
    Object ex = mp_format("  File %o: in %o , at line %d", file, fnc_name,
            f->lineno);
    list_append(GET_LIST(tm->ex_list), ex);
}

void traceback() {
    int i;
    Object exlist = tm->ex_list;
    printf("Traceback (most recent call last):\n");
    int cur = tm->frame - tm->frames;
    for (i = LIST_LEN(exlist) - 1; i >= cur; i--) {
        mp_println(LIST_NODES(exlist)[i]);
    }
    fprintf(stderr, "Exception:\n  ");
    fprintf(stderr, "%s\n", GET_SZ(tm->ex_line));
}

void mp_raise(char* fmt, ...) {
    va_list a;
    va_start(a, fmt);
    list_clear(GET_LIST(tm->ex_list));
    tm->ex = mp_format_va_list(fmt, a, 0);
    Object file = get_file_name_obj(tm->frame->fnc);
    Object fnc_name = get_func_name_obj(tm->frame->fnc);
    tm->ex_line = mp_format("File %o: in %o at line %d\n  %os", 
        file, fnc_name, tm->frame->lineno, tm->ex);
    va_end(a);
    longjmp(tm->frame->buf, 1);
}