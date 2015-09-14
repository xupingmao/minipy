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
TmDict*          DictInit();
void             freeDict(TmDict* dict);
void             DictSet(TmDict* dict, Object key, Object val);
DictNode*        DictGetNode(TmDict* dict, Object key);
Object*          DictGetByStr(TmDict* dict, char* key);
Object           DictKeys(TmDict* );
#define dictKeys(obj) DictKeys(GET_DICT(obj))
void             _dictDel(TmDict* dict, Object k);
#define dictDel(dict, k) _dictDel(GET_DICT(dict), k)
void             regDictMethods();
void             DictPrint(TmDict*);

void _dictSetByStr(TmDict* dict, char* key, Object val);
#define dictSetByStr(dict, key, val) _dictSetByStr(GET_DICT(dict), key, val)
#define dictGetByStr(dict, key) DictGetByStr(GET_DICT(dict), key)

static DataProto dictIterProto;

DataProto* getDictIterProto();
Object dictIterNew(TmDict* dict);
Object* dictNext(TmDictIterator* iterator);

#endif
