#ifndef _TM_ARG_H
#include "tm.h"
#include "object.h"
#include "tmdata.h"
#include "tmdict.h"
#include "tmlist.h"
#include "function.h"

void arg_start();
void arg_push(Object obj) ;
void objSetArguments(Object* first, int len);
#define setArgs  objSetArguments
void _resolveMethodSelf(TmFunction *fnc);
#define resolveMethodSelf(fnc) _resolveMethodSelf(GET_FUNCTION((fnc)))
void printArguments();
int hasArg();
Object arg_get_str(const char* fnc);
char*  arg_get_sz(const char* fnc);
Object arg_get_func(const char* fnc);
int arg_get_int(const char* fnc);
double arg_get_double(const char* fnc);
TmList* arg_get_list_p(const char* fnc);
Object arg_get_list(const char* fnc);
Object arg_get_dict(const char* fnc);
Object arg_get_obj(const char* fnc);
Object arg_get_data(const char* fnc);
int getArgsCount() ;
void tmUngetArg();

#endif
