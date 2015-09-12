
/**
* HashMap实现，采用链路分离法
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
	int size = 3;
	TmDict * dict = tmMalloc(sizeof(TmDict));
	dict->nodes = dict->_nodes;
	DictNode** nodes = dict->nodes;
	// to mark that the node is not allocated.
	int i;for(i = 0; i < size; i++){
		nodes[i] = NULL;
	}
	dict->len = 0;
	dict->cap = size;
	dict->head = NULL;
	return dict;
}

Object newDict(){
	Object o;
	o.type = TYPE_DICT;
	GET_DICT(o) = DictInit();
	return gcTrack(o);
}

// reset hashdict, do not need to allocate new block.
void DictSetNode(TmDict* dict, DictNode* des){
	int hash = DictHash(des->key);
	int idx = hash % dict->cap;
//	printf("set:entry pos:%d\n", hash % dict->cap);
	DictNode* node = dict->nodes[idx];
	des->hash = hash;
	des->idx = idx;
	if (dict->head != NULL) {
		if (node == NULL) {
			dict->nodes[idx] = des;
			des->next = dict->head;
			dict->head = des;
		} else {
			// insert after first element in the slot.
			des->next = node->next;
			node->next = des;
		}
	} else {
		dict->head = des;
		des->next = NULL;
		dict->nodes[idx] = des;
	}
}

void DictCheck(TmDict* dict){
	if(dict->len < dict->cap)
		return;
	int osize = dict->cap;
	int i;
	int nsize = osize + osize / 2 + 1;
	dict->cap = nsize;
	// store old nodes to rehash the table
	DictNode** nodes = dict->nodes;
	dict->nodes = tmMalloc(nsize * sizeof(DictNode*));
	// reset new nodes
	for(i = 0; i < nsize; i++){
		dict->nodes[i] = NULL;
	}
    /* rehash nodes */
	DictNode* node = dict->head;
	dict->head = NULL;
	while(node != NULL){
		int hash = node->hash;
		int idx = hash % dict->cap;
		DictNode* next = node->next;
		node->idx = idx;
		DictNode* newNode = dict->nodes[idx];
		if (newNode == NULL) {
			dict->nodes[idx] = node;
			node->next = dict->head;
			dict->head = node;
		} else {
			node->next = newNode->next;
			newNode->next = node;
		}
		node = next;
	}
	// free old nodes.
    if(nodes != dict->_nodes){
        tmFree(nodes, osize * sizeof(DictNode*));
    }
}



void DictFreeNode(DictNode* node){
	if(node == NULL){
		return;
	}
	DictFreeNode(node->next);
	tmFree(node, sizeof(DictNode));
}

void freeDict(TmDict* dict){
	PRINT_OBJ_GC_INFO_START();
	int i;
	DictFreeNode(dict->head);
    if (dict->nodes != dict->_nodes){
        tmFree(dict->nodes, sizeof(DictNode*) * dict->cap);
    }
	tmFree(dict, sizeof(TmDict));
	PRINT_OBJ_GC_INFO_END("dict", dict);
}


void DictSet(TmDict* dict, Object key, Object val){
	DictNode* node = DictGetNode(dict, key);
	if (node != NULL) {
		node->val = val;
		return;
	}
	DictCheck(dict);
	/* add a new node , insert into the nodes */
	dict->len++;
	DictNode* new_node = tmMalloc(sizeof(DictNode));
	new_node->key = key;
	new_node->val = val;
    DictSetNode(dict, new_node);
}

DictNode* DictGetNode(TmDict* dict, Object key){
    int hash = DictHash(key);
    int idx = hash % dict->cap;
	DictNode* node = dict->nodes[idx];
	while(node != NULL && node->idx == idx){
		if (node->hash == hash && tmEquals(node->key, key)) {
			return node;
		}
		node = node->next;
	}
	return NULL;
}

Object* DictGetByStr(TmDict* dict, char* key) {
    int hash = hashSz((unsigned char*) key, strlen(key));
    int idx = hash % dict->cap;
	DictNode* node = dict->nodes[idx];
    while (node != NULL
    		&& node->idx == idx) {
    	if (hash != node->hash) {
    		node = node->next;
    		continue;
    	}
        Object k = node->key;
        if (TM_TYPE(k) != TYPE_STR) {
        	node = node->next;
        	continue;
        }
        char* ks = GET_STR(k);
        if (strcmp(ks,key) == 0) {
            return &(node->val);
        } else {
        	node = node->next;
        }
    }
    return NULL;
}

void _dictSetByStr(TmDict* dict, char* key, Object value) {
	DictSet(dict, staticString(key), value);
}

void _dictDel(TmDict* dict, Object key) {
	DictNode *node = DictGetNode(dict, key);
	if (node == NULL) {
		tmRaise("dictDel: keyError %o", key);
	} else {
        int i;
		node->idx = -1;
		dict->len--;
		DictNode* n = dict->head;
		// is first node
		if (node == n) {
			dict->head = node->next;
		}else {
			while (n != NULL) {
				if (n->next != NULL && n->next->idx == -1) {
					n->next = n->next->next;
					break;
				}
				n = n->next;
			}
		}
        for (i = 0; i < dict->cap; i++) {
            if (dict->nodes[i] != NULL && dict->nodes[i]->idx == -1) {
                if (dict->nodes[i]->next != NULL && dict->nodes[i]->next->idx == i) {
                    dict->nodes[i] = dict->nodes[i]->next;
                } else {
                    dict->nodes[i] = NULL;
                }
            }
        }
		tmFree(node, sizeof(DictNode));
	}
}

Object DictKeys(TmDict* dict){
	Object list = newList(dict->len);
	DictNode* node = dict->head;
	while(node != NULL){
		APPEND(list, node->key);
		node = node->next;
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
    Object list = newList(dict->len);
	DictNode* node = dict->head;
	while(node != NULL){
		APPEND(list, node->val);
		node = node->next;
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
	iterator->cur_node = dict->head;
	iterator->proto = getDictIterProto();
	return data;
}

Object* dictNext(TmDictIterator* iterator) {
	if (iterator->cur_node != NULL) {
		Object* key = &(iterator->cur_node->key);
		iterator->cur_node = iterator->cur_node->next;
		return key;
	}
	return NULL;
}

