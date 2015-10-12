#ifndef _TM_DICT_H
#define _TM_DICT_H
#include "object.h"
#include "tmdata.h"
typedef struct DictNode{
  Object key;
  Object val;
  int hash;
  int idx;
  struct DictNode* next;
}DictNode;

typedef struct Dictonary {
  int marked;
  int len;
  int cap;
  struct DictNode* last_node;
  struct DictNode* head;
  struct DictNode* _nodes[7];
  struct DictNode** nodes;
}TmDict;

typedef struct _TmDictIterator {
	DATA_HEAD
	TmDict* dict;
	DictNode* cur_node;
} TmDictIterator;


Object           dict_new();
TmDict*          dict_init();
void             dict_free(TmDict* dict);
void             dict_set(TmDict* dict, Object key, Object val);
DictNode*        dict_get_node(TmDict* dict, Object key);
Object*          dict_get_by_str(TmDict* dict, char* key);
Object           dict_keys(TmDict* );
#define dictKeys(obj) dict_keys(GET_DICT(obj))
void             dict_del(TmDict* dict, Object k);
#define dictDel(dict, k) dict_del(GET_DICT(dict), k)
void             dict_methods_init();
void             DictPrint(TmDict*);

void dict_set_by_str(TmDict* dict, char* key, Object val);
#define dictSetByStr(dict, key, val) dict_set_by_str(GET_DICT(dict), key, val)
#define dictGetByStr(dict, key) dict_get_by_str(GET_DICT(dict), key)

static DataProto dictIterProto;

DataProto* getDictIterProto();
Object dict_iter_new(TmDict* dict);
Object* dict_next(TmDictIterator* iterator);

#endif
