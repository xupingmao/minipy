#include "include/tm.h"
#include <setjmp.h>

void push_exception(TmFrame* f){
    Object file = get_file_name_obj(f->fnc);
    Object fnc_name = get_func_name_obj(f->fnc);
    Object ex = tm_format("  File %o: in %o , at line %d", file, fnc_name,
            f->lineno);
    list_append(GET_LIST(tm->ex_list), ex);
}

void traceback() {
    int i;
    Object exlist = tm->ex_list;
    printf("Traceback (most recent call last):\n");
    int cur = tm->frame - tm->frames;
    for (i = LIST_LEN(exlist) - 1; i >= cur; i--) {
        tm_println(LIST_NODES(exlist)[i]);
    }
    printf("Exception:\n  ");
    tm_println(tm->ex_line);
}

void tm_raise(char* fmt, ...) {
    va_list a;
    va_start(a, fmt);
    list_clear(GET_LIST(tm->ex_list));
    tm->ex = tm_format_va_list(fmt, a, 0);
    Object file = get_file_name_obj(tm->frame->fnc);
    Object fnc_name = get_func_name_obj(tm->frame->fnc);
    tm->ex_line = tm_format("File %o: in %o at line %d\n  %os", 
        file, fnc_name, tm->frame->lineno, tm->ex);
    va_end(a);
    longjmp(tm->frame->buf, 1);
}













