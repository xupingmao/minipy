
/**
* @Author xupingmao <578749341@qq.com>
*/


/**
* less interface is better than more,
* because many interface with similar function will confuse the users.
*/

#include "include/tm.h"

/** new hashdict instance, with initial allocated size set to 7.
*   better not similar to computer binary size ( as to say 2 ), 
*   such as 2, 10 etc.
*/


int hashSz(unsigned char* s, int len) {
	int hash = 1315423911;
	int i = 0;
	for(i = 0; i < len; i++)
	   hash ^= ((hash << 5) + s[i] + (hash >> 2));
	return abs(hash);
}

int DictHash(Object key) {
	switch(TM_TYPE(key)) {
	case TYPE_STR:return hashSz((unsigned char*) GET_STR(key), GET_STR_LEN(key));
	case TYPE_NUM:return abs((int) GET_NUM(key));
	default: return 0;
	}
	return 0;
}


TmDict* DictInit(){
    int i;
	TmDict * dict = tm_malloc(sizeof(TmDict));
    dict->cap = 3;
    dict->extend = 2;
	dict->nodes = tm_malloc(sizeof(DictNode) * (dict->cap));
	// to mark that the node is not allocated.
	for(i = 0; i < dict->cap; i++){
		dict->nodes[i].used = 0;
	}
	dict->len = 0;
	return dict;
}

Object newDict(){
	Object o;
	o.type = TYPE_DICT;
	GET_DICT(o) = DictInit();
	return gcTrack(o);
}


void DictCheck(TmDict* dict){
	if(dict->len < dict->cap)
		return;
	int osize = dict->cap;
	int i, j, nsize;
    if (osize < 10) {
        nsize = osize + 2;
    } else {
        nsize = osize + osize / 2 + 1;
    }
	DictNode* nodes = tm_malloc(nsize * sizeof(DictNode));
    for(i = 0; i < nsize; i++) {
        nodes[i].used = 0;
    }
    j = 0;
    for(i = 0; i < osize; i++) {
        if (dict->nodes[i].used) {
            nodes[j] = dict->nodes[i];
            j++;
        }
    }
    DictNode* temp = dict->nodes;
    dict->nodes = nodes;
    dict->cap = nsize;
    tm_free(temp, osize * sizeof(DictNode));
}

void freeDict(TmDict* dict){
	PRINT_OBJ_GC_INFO_START();
	tm_free(dict->nodes, (dict->cap) * sizeof(DictNode));
	tm_free(dict, sizeof(TmDict));
	PRINT_OBJ_GC_INFO_END("dict", dict);
}

int findfreepos(TmDict* dict) {
    int i;
    for(i = 0; i < dict->cap; i++) {
        if (dict->nodes[i].used == 0) {
            return i;
        }
    }
    return -1;
}

int DictSet(TmDict* dict, Object key, Object val){
	DictNode* node = DictGetNode(dict, key);
	if (node != NULL) {
		node->val = val;
		return (node - dict->nodes);
	}
    DictCheck(dict);
	int i = findfreepos(dict);
    dict->len++;
    dict->nodes[i].used = 1;
    dict->nodes[i].key = key;
    dict->nodes[i].val = val;
    return i;
}

int setAttr(TmDict* dict, int constId, Object val) {
    int i;
	DictNode* nodes = dict->nodes;
    constId += 2; /* prevent first const to be 0, and normal dict node to be 1. */
	for (i = 0; i < dict->cap; i++) {
        if (nodes[i].used == constId) {
            nodes[i].val = val;
            return i;
        }
    }
    Object key = GET_CONST(constId-2);
    i = DictSet(dict, key, val);
    dict->nodes[i].used = constId;
    return i;
}

int getAttr(TmDict* dict, int constId) {
    int i;
	DictNode* nodes = dict->nodes;
    constId += 2; /* prevent first const to be 0, and normal dict node to be 1. */
	for (i = 0; i < dict->cap; i++) {
        if (nodes[i].used == constId) {
            return i;
        }
    }
    DictNode* node = DictGetNode(dict, GET_CONST(constId-2));
    if (node != NULL) {
        node->used = constId;
        return node - nodes;
    }
    return -1;
}

DictNode* DictGetNode(TmDict* dict, Object key){
    //int hash = DictHash(key);
    //int idx = hash % dict->cap;
    int i;
	DictNode* nodes = dict->nodes;
	for (i = 0; i < dict->cap; i++) {
        if (nodes[i].used && tmEquals(nodes[i].key, key)) {
            return nodes + i;
        }
    }
	return NULL;
}

Object* DictGetByStr(TmDict* dict, char* key) {
    //int hash = hashSz((unsigned char*) key, strlen(key));
    //int idx = hash % dict->cap;
    int i;
	DictNode* nodes = dict->nodes;
    for (i = 0; i < dict->cap; i++) {
        if (nodes[i].used && TM_TYPE(nodes[i].key) == TYPE_STR && 
            strcmp(GET_STR(nodes[i].key), key) == 0) {
            return &nodes[i].val;
        }
    } 
    return NULL;
}

void _dictSetByStr(TmDict* dict, char* key, Object value) {
	DictSet(dict, string_static(key), value);
}

void _dictDel(TmDict* dict, Object key) {
	DictNode* node = DictGetNode(dict, key);
    if (node == NULL) {
        tmRaise("tm_del: keyError %o", key);
    }
    node->used = 0;
    dict->len--;
    return;
}

Object DictKeys(TmDict* dict){
    Object list = list_new(dict->len);
    int i;
    for(i = 0; i < dict->cap; i++) {
        if (dict->nodes[i].used) {
            APPEND(list, dict->nodes[i].key);
        }
    }
	return list;
}

Object bmDictKeys(){
	Object dict = getDictArg("dict.keys");
	return DictKeys(GET_DICT(dict));
}

Object bmDictValues() {
    Object _d = getDictArg("dict.values");
    TmDict* dict = GET_DICT(_d);
    Object list = list_new(dict->len);
    int i;
    for(i = 0; i < dict->cap; i++) {
        if (dict->nodes[i].used) {
            APPEND(list, dict->nodes[i].val);
        }
    }
	return list;
}

void regDictMethods() {
	CLASS_DICT = newDict();
	/* build dict class */
	regConst(CLASS_DICT);
	regModFunc(CLASS_DICT, "keys", bmDictKeys);
    regModFunc(CLASS_DICT, "values", bmDictValues);
}

void dictIterMark(DataObject* data) {
    TmDictIterator* iter = (TmDictIterator*) data;
    gcMarkDict(iter->dict);
}

DataProto* getDictIterProto() {
	if(!dictIterProto.init) {
		initDataProto(&dictIterProto);
		dictIterProto.dataSize = sizeof(TmDictIterator);
		dictIterProto.next = dictNext;
        dictIterProto.mark = dictIterMark;
	}
	return &dictIterProto;
}


Object dictIterNew(TmDict* dict) {
    /*
    Object *__iter__ = DictGetByStr(dict, "__iter__");
    if (__iter__ != NULL) {
        Object *next = DictGetByStr(dict, "next");
        Object data = dataNew(sizeof(TmBaseIterator));
        TmBaseIterator* baseIterator = (TmBaseIterator*)GET_DATA(data);
        baseIterator->func = *next;
        baseIterator->proto = getBaseIterProto();
        return data;
    }*/
	Object data = dataNew(sizeof(TmDictIterator));
	TmDictIterator* iterator = (TmDictIterator*)GET_DATA(data);
	iterator->dict = dict;
	iterator->idx = 0;
	iterator->proto = getDictIterProto();
	return data;
}

Object* dictNext(TmDictIterator* iterator) {
	if (iterator->idx < iterator->dict->cap) {
		int i;
        for(i = iterator->idx; i < iterator->dict->cap; i++) {
            if (iterator->dict->nodes[i].used) {
                iterator->idx = i + 1;
                return &iterator->dict->nodes[i].key;
            }
        }
		return NULL;
	}
	return NULL;
}

