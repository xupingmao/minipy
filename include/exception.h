#ifndef _EXCEPTION_H
#define _EXCEPTION_H

#include "tm.h"
#include "interp.h"

void pushException(TmFrame* f);
void traceback();
void tmRaise(char* fmt, ...);

#endif
