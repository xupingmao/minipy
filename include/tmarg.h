#ifndef _TM_ARG_H
#include "tm.h"
#include "object.h"
#include "tmdata.h"
#include "tmdict.h"
#include "tmlist.h"
#include "function.h"

void argStart();
void argPush(Object obj) ;
void argSetArguments(Object* first, int len);
#define setArgs  argSetArguments
void _resolveMethodSelf(TmFunction *fnc);
#define resolveMethodSelf(fnc) _resolveMethodSelf(GET_FUNCTION((fnc)))
void printArguments();
int hasArg();
Object argTakeStrObj(const char* fnc);
char*  argTakeSz(const char* fnc);
Object takeFuncObj(const char* fnc);
int argTakeInt(const char* fnc);
double argTakeDouble(const char* fnc);
TmList* argTakeListPtr(const char* fnc);
Object argTakeListObj(const char* fnc);
Object argTakeDictObj(const char* fnc);
Object argTakeObj(const char* fnc);
Object argTakeDataObj(const char* fnc);
int getArgsCount() ;
void tmUngetArg();

#endif
