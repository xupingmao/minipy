#include "include/exception.h"
#include <setjmp.h>

void pushException(TmFrame* f){
    Object file = GET_FUNCTION_FILE(f->fnc);
    Object fncName = GET_FUNCTION_NAME(f->fnc);
    Object ex = tmFormat("  File %o: in %o , at line %d", file, fncName,
            f->lineno);
    listAppend(GET_LIST(tm->exList), ex);
}

void traceback() {
    int i;
    Object exlist = tm->exList;
    printf("Traceback (most recent call last):\n");
    for (i = LIST_LEN(exlist) - 1; i >= 0; i--) {
        tm_println(LIST_NODES(exlist)[i]);
    }
    printf("Exception:\n  ");
    tm_println(tm->ex);
}

void tm_raise(char* fmt, ...) {
    DEBUG2("enter raise", fmt)
    va_list a;
    va_start(a, fmt);
    list_clear(GET_LIST(tm->exList));
    tm->ex = tmFormatVaList(fmt, a, 0);
    Object file = GET_FUNCTION_FILE(tm->frame->fnc);
    Object fncName = GET_FUNCTION_NAME(tm->frame->fnc);
    tm->ex = tmFormat("File %o: in %o at line %d\n  %os", 
        file, fncName, tm->frame->lineno, tm->ex);
    va_end(a);
    longjmp(tm->frame->buf, 1);
}

