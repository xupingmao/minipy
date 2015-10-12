#ifndef _FUNCTION_H
#define _FUNCTION_H

#include "tm.h"
#define GET_FUNCTION(obj) (obj.value.func)
#define IS_FUNCTION(o) TM_TYPE(o)==TYPE_FUNCTION
#define GET_FUNCTIONTION_MODULE_PTR(fnc) GET_MODULE(GET_FUNCTION(fnc)->mod)
#define GET_FUNC_CONSTANTS(fnc) GET_FUNCTIONTION_MODULE_PTR(fnc)->constants
#define GET_FUNC_MOD_NAME(fnc) GET_FUNCTIONTION_MODULE_PTR(fnc)->file
#define GET_FUNC_CONSTANTS_NODES(fnc) LIST_NODES(GET_FUNC_CONSTANTS(fnc))
#define GET_FUNCTION_NAME(fnc) GET_FUNCTION(fnc)->name

#define DB_FUNC 1

Object           func_new(Object mod,Object self,Object (*native_func)());
Object           getFuncAttr(TmFunction* fnc, Object key);
void             func_free(TmFunction*);
Object           method_new(Object _fnc, Object self);
Object           module_new(Object file, Object name, Object code);
void             module_free(TmModule*);
// CodeCheckResult  resolveCode(TmModule *mod,  unsigned char*s , int isFuncDef);
unsigned char*   getFunctionCode(TmFunction*);
void             func_format(char* des, TmFunction* func);
#define          functionFormat(des, func) func_format(des, GET_FUNCTION(func))
TmModule*        getFuncMod(TmFunction* func);
#if DB_FUNC
char* getFuncNameSz(Object func);
char* getFuncFileSz(Object func);
#endif
Object           class_new(Object dict);
Object           getFunctionGlobals(TmFunction*);
unsigned char* func_resolve(TmFunction*, unsigned char*);
#define getGlobals(func) getFunctionGlobals(GET_FUNCTION(func))
#endif
