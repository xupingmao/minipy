#ifndef _TM_DICT_H
#define _TM_DICT_H
#include "object.h"
#include "tmdata.h"
typedef struct DictNode{
  Object key;
  Object val;
  int hash;
  int used; /* also used for attr index */
}DictNode;

typedef struct Dictonary {
  int marked;
  int len;
  int cap;
  int extend;
  struct DictNode* nodes;
}TmDict;

typedef struct _TmDictIterator {
    DATA_HEAD
    TmDict* dict;
    int idx;
} TmDictIterator;


Object           dictNew();
TmDict*          dict_init();
void             dict_free(TmDict* dict);
int             dict_set(TmDict* dict, Object key, Object val);
#define dictSet(d, k, v) dict_set(GET_DICT(d), k, v)
DictNode*        dictGetNode(TmDict* dict, Object key);
Object*          dict_get_by_str(TmDict* dict, char* key);
Object           dict_keys(TmDict* );
void             dict_del(TmDict* dict, Object k);
#define dictDel(dict, k) dict_del(GET_DICT(dict), k)
void             dictMethodsInit();

void dict_set_by_str(TmDict* dict, char* key, Object val);
#define dictSetByStr(dict, key, val) dict_set_by_str(GET_DICT(dict), key, val)
#define dictGetByStr(dict, key) dict_get_by_str(GET_DICT(dict), key)

static DataProto dictIterProto;

DataProto* getDictIterProto();
Object dict_iterNew(TmDict* dict);
Object* dict_next(TmDictIterator* iterator);

int dict_set_attr(TmDict* dict, int constId, Object val);
int dict_get_attr(TmDict* dict, int constId);

#endif
