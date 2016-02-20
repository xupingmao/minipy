
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


TmDict* dict_init(){
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

Object dictNew(){
    Object o;
    o.type = TYPE_DICT;
    GET_DICT(o) = dict_init();
    return gcTrack(o);
}


void dict_check(TmDict* dict){
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

void dict_free(TmDict* dict){
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
#if USE_IDX
int dictSetAttr2(TmDict* dict, Object key, Object val) {
    int i = 0;
    DictNode* nodes = dict->nodes;
    for (i = 0; i < dict->cap; i++) {
        if (nodes[i].used && nodes[i].key.idx == key.idx) {
            nodes[i].val = val;
            return i;
        }
    }
    return -1;
}

int dictGetAttr2(TmDict* dict, Object key) {
    int i = 0;
    DictNode* nodes = dict->nodes;
    for (i = 0; i < dict->cap; i++) {
        if (nodes[i].used && nodes[i].key.idx == key.idx) {
            return i;
        }
    }
    return -1;
}
#endif

int dict_set(TmDict* dict, Object key, Object val){
    int i;
    #if USE_IDX
    if (key.idx > 0) {
        // tmPrintf("idx=%d, obj=%o\n", key.idx, key);
        i = dictSetAttr2(dict, key, val);
        if (i > 0) return i;
    }
    #endif
    DictNode* node = dictGetNode(dict, key);
    if (node != NULL) {
        node->val = val;
        return (node - dict->nodes);
    }
    dict_check(dict);
    i = findfreepos(dict);
    dict->len++;
    dict->nodes[i].used = 1;
    dict->nodes[i].key = key;
    dict->nodes[i].val = val;
    return i;
}

int dictSetAttr(TmDict* dict, int constId, Object val) {
    int i;
    DictNode* nodes = dict->nodes;
    constId += 2; /* start from 2, as 0,1 are used by normal node. */
    for (i = 0; i < dict->cap; i++) {
        if (nodes[i].used == constId) {
            nodes[i].val = val;
            return i;
        }
    }
    Object key = GET_CONST(constId-2);
    i = dict_set(dict, key, val);
    dict->nodes[i].used = constId;
    return i;
}

int dictGetAttr(TmDict* dict, int constId) {
    int i;
    DictNode* nodes = dict->nodes;
    constId += 2; /* prevent first const to be 0, and normal dict node to be 1. */
    for (i = 0; i < dict->cap; i++) {
        if (nodes[i].used == constId) {
            return i;
        }
    }
    DictNode* node = dictGetNode(dict, GET_CONST(constId-2));
    if (node != NULL) {
        node->used = constId;
        return node - nodes;
    }
    return -1;
}

DictNode* dictGetNode(TmDict* dict, Object key){
    //int hash = DictHash(key);
    //int idx = hash % dict->cap;
    int i;
    #if USE_IDX
    if (key.idx > 0) {
        i = dictGetAttr2(dict, key);
        if (i > 0) {
            return dict->nodes + i;
        }
    }
    #endif
    DictNode* nodes = dict->nodes;
    for (i = 0; i < dict->cap; i++) {
        if (nodes[i].used && objEquals(nodes[i].key, key)) {
            return nodes + i;
        }
    }
    return NULL;
}

Object* dict_get_by_str(TmDict* dict, char* key) {
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

void dict_set_by_str(TmDict* dict, char* key, Object value) {
    dict_set(dict, szToString(key), value);
}

void dict_del(TmDict* dict, Object key) {
    DictNode* node = dictGetNode(dict, key);
    if (node == NULL) {
        tmRaise("objDel: keyError %o", key);
    }
    node->used = 0;
    dict->len--;
    return;
}

Object dict_keys(TmDict* dict){
    Object list = listNew(dict->len);
    int i;
    for(i = 0; i < dict->cap; i++) {
        if (dict->nodes[i].used) {
            objAppend(list, dict->nodes[i].key);
        }
    }
    return list;
}

Object dict_m_keys(){
    Object dict = argTakeDictObj("dict.keys");
    return dict_keys(GET_DICT(dict));
}

Object dict_m_values() {
    Object _d = argTakeDictObj("dict.values");
    TmDict* dict = GET_DICT(_d);
    Object list = listNew(dict->len);
    int i;
    for(i = 0; i < dict->cap; i++) {
        if (dict->nodes[i].used) {
            objAppend(list, dict->nodes[i].val);
        }
    }
    return list;
}

void dictMethodsInit() {
    tm->dict_proto = dictNew();
    /* build dict class */
    regModFunc(tm->dict_proto, "keys", dict_m_keys);
    regModFunc(tm->dict_proto, "values", dict_m_values);
}

void dict_iter_mark(DataObject* data) {
    TmDictIterator* iter = (TmDictIterator*) data;
    gcMarkDict(iter->dict);
}

DataProto* getDictIterProto() {
    if(!dictIterProto.init) {
        initDataProto(&dictIterProto);
        dictIterProto.dataSize = sizeof(TmDictIterator);
        dictIterProto.next = dict_next;
        dictIterProto.mark = dict_iter_mark;
    }
    return &dictIterProto;
}


Object dict_iterNew(TmDict* dict) {
    /*
    Object *__iter__ = dict_get_by_str(dict, "__iter__");
    if (__iter__ != NULL) {
        Object *next = dict_get_by_str(dict, "next");
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

Object* dict_next(TmDictIterator* iterator) {
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

