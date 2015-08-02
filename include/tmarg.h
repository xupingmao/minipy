#ifndef _TM_ARG_H
#include "tm.h"
#include "object.h"
#include "tmdata.h"
#include "tmdict.h"
#include "tmlist.h"
#include "function.h"

void argStart();
void pushArg(Object obj) ;
void tmSetArguments(Object* first, int len);
#define setArgs  tmSetArguments
void _resolveMethodSelf(TmFunction *fnc);
#define resolveMethodSelf(fnc) _resolveMethodSelf(GET_FUNCTION((fnc)))
void printArguments();
Object getArgFromVM(int type);

int hasArg();
Object getStrArg(const char* fnc);
char*  getSzArg(const char* fnc);
Object getFuncArg(const char* fnc);
int getIntArg(const char* fnc);
double getNumArg(const char* fnc);
TmList* getListPtrArg(const char* fnc);
Object getListArg(const char* fnc);
Object getDictArg(const char* fnc);
Object getObjArg(const char* fnc);
Object getDataArg(const char* fnc);
int getArgsCount() ;
void tmUngetArg();

#endif
