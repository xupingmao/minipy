#include "include/exception.h"
#include <setjmp.h>

void pushException(TmFrame* f){
    Object file = getFileNameObj(f->fnc);
    Object fncName = getFuncNameObj(f->fnc);
    Object ex = tmFormat("  File %o: in %o , at line %d", file, fncName,
            f->lineno);
    listAppend(GET_LIST(tm->exList), ex);
}

void traceback() {
    int i;
    Object exlist = tm->exList;
    printf("Traceback (most recent call last):\n");
    int cur = tm->frame - tm->frames;
    for (i = LIST_LEN(exlist) - 1; i >= cur; i--) {
        tmPrintln(LIST_NODES(exlist)[i]);
    }
    printf("Exception:\n  ");
    tmPrintln(tm->ex);
}

void tmRaise(char* fmt, ...) {
    va_list a;
    va_start(a, fmt);
    listClear(GET_LIST(tm->exList));
    tm->ex = tmFormatVaList(fmt, a, 0);
    Object file = getFileNameObj(tm->frame->fnc);
    Object fncName = getFuncNameObj(tm->frame->fnc);
    tm->ex = tmFormat("File %o: in %o at line %d\n  %os", 
        file, fncName, tm->frame->lineno, tm->ex);
    va_end(a);
    longjmp(tm->frame->buf, 1);
}

