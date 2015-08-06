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


Object           newDict();
TmDict*          DictInit();
void             freeDict(TmDict* dict);
void             DictSet(TmDict* dict, Object key, Object val);
DictNode*        DictGetNode(TmDict* dict, Object key);
Object*          DictGetByStr(TmDict* dict, char* key);
Object           DictKeys(TmDict* );
void             _dictDel(TmDict* dict, Object k);
#define dictDel(dict, k) _dictDel(GET_DICT(dict), k)
void             regDictMethods();

void _dictSetByStr(TmDict* dict, char* key, Object val);
#define dictSetByStr(dict, key, val) _dictSetByStr(GET_DICT(dict), key, val)
#define dictGetByStr(dict, key) DictGetByStr(GET_DICT(dict), key)

static DataProto dictIterProto;

DataProto* getDictIterProto();
Object dictIterNew(TmDict* dict);
Object* dictNext(TmDictIterator* iterator);

void setAttr(TmDict* dict, int constId, Object val);
Object* getAttr(TmDict* dict, int constId);

#endif
