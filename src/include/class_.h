#ifndef MP_CLASS_H
#define MP_CLASS_H

#include "object.h"


MpObj class_new(MpObj name, MpModule* module);
MpObj class_new_by_cstr(char* name, MpModule* module);
MpInstance* class_instance(MpClass* klass);
MpObj mp_format_class(MpClass* clazz);
void class_free(MpClass* pclass);
void mp_resolve_code(MpModule* m, const char* code);
void MpClass_setattr(MpClass* klass, MpObj key, MpObj value);
MpObj mp_call_obj_safe(MpObj func, int n, MpObj* args);
MpObj MpInstance_str(MpInstance* instance);
int MpInstance_contains(MpInstance* instance, MpObj key);
MpObj MpInstance_getattr(MpInstance* instance, MpObj key, MpObj* default_);
void MpInstance_setattr(MpInstance* instance, MpObj key, MpObj value);
int MpInstance_len(MpInstance* instance);

MpClass* MpClass_New(char*name, MpModule*);
void MpClass_RegNativeMethod(MpClass* clazz, char* name, MpNativeFunc func);
MpObj MpClass_ToObj(MpClass* class_);

#endif

