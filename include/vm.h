#ifndef _VM_H
#define _VM_H

#include "tm.h"
Object callModuleFunction(char* mod, char* fnc);
void regConst(Object obj);
void registerBuiltin(char* name, Object value);
void regModFunc(Object mod, char* name, Object(*native)());
void regBuiltinFunc(char* name, Object (*native)());
void registerModAttr(char* modName,char* attr, Object value);


#endif
