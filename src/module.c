#include "include/mp.h"

void module_free(MpModule* mod){
    if (mod->cache != NULL) {
        mp_free(mod->cache, sizeof(MpCodeCache) * mod->cache_cap);
    }
    mp_free(mod, sizeof(MpModule));
}

/**
 * @param file filename
 * @name  __name__
 */
MpObj module_new(MpObj fname, MpObj name, MpObj code){
  MpModule *mod = mp_malloc(sizeof(MpModule), "module.new");
  mod->file = fname;
  mod->code = code;
  mod->resolved = 0;
  mod->cache = NULL;
  /*mod->constants = list_new(20);*/
  /*list_append(GET_LIST(mod->constants), NONE_OBJECT);*/
  mod->globals = dict_new();
  MpObj m = gc_track(mp_to_obj(TYPE_MODULE, mod));
  /* set module */
  obj_set(tm->modules, fname, mod->globals);
  obj_set(mod->globals, string_static("__name__"), name);
  return m;
}


/**
 * register module
 */
MpObj mp_new_native_module(char* name) {
    MpObj module = dict_new();
    MpObj module_name = string_new(name);

    obj_set(module, string_from_cstr("__name__"), module_name);
    obj_set(tm->modules, module_name, module);
    return module;
}

/**
 * register module function
 * @param mod, module object, dict
 */
void MpModule_RegFunc(MpObj mod, char* name, MpObj (*native)()) {
    assert(MP_TYPE(mod) == TYPE_DICT);
    MpObj func = func_new(tm->builtins_mod, NONE_OBJECT, native);
    GET_FUNCTION(func)->name = string_from_cstr(name);
    obj_set(mod,GET_FUNCTION(func)->name, func);
}

// 注册模块的变量
void MpModule_RegAttr(MpObj module, char* name, MpObj value) {
    // assert(MP_TYPE(module) == TYPE_MODULE);
    obj_set(module, string_from_cstr(name), value);
}
