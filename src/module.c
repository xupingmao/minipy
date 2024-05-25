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
  MpObj m = gc_track(obj_new(TYPE_MODULE, mod));
  /* set module */
  obj_set(tm->modules, fname, mod->globals);
  obj_set(mod->globals, string_static("__name__"), name);
  return m;
}
