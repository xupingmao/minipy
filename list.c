#include "include/tm.h"
#include "include/tmlist.h"

TmList* list_alloc_untracked(int cap) {
	TmList* list = tm_malloc(sizeof(TmList));
	list->len = 0;
	if (cap <= 0) {
		cap = 2;
	}
	list->cap = cap;
	list->nodes = tm_malloc(OBJ_SIZE * list->cap);
	list->cur = 0;
	return list;
}

Object list_new(int cap) {
	Object v;
	v.type = TYPE_LIST;
	v.value.list = list_alloc_untracked(cap);
	return gc_track(v);
}

/* build list of length n from object list */
Object list_newFromVaList(int n, ...) {
	Object list = list_new(n);
	va_list a;
	va_start(a, n);
	int i;
	for (i = 0; i < n; i++) {
		APPEND(list, va_arg(a, Object));
	}
	va_end(a);
	return list;
}

/* build list from Object arrays */
Object list_newFromArray(int n, Object* items) {
	Object obj = list_new(n);
	int i;
	for (i = 0; i < n; i++) {
		APPEND(obj, items[i]);
	}
	return obj;
}

void list_free(TmList* list) {
	PRINT_OBJ_GC_INFO_START();
	tm_free(list->nodes, list->cap * OBJ_SIZE);
	tm_free(list, sizeof(TmList));
	PRINT_OBJ_GC_INFO_END("list", list);
}

Object list_get(TmList* list, int n) {
	if (n < 0) {
		n += list->len;
	}
	if (n >= list->len || n < 0) {
		tmRaise("list out of range");
	}
	return list->nodes[n];
}

void list_set(TmList* list, int n, Object val) {
	if (n < 0) {
		n += list->len;
	}
	if (n >= list->len || n < 0) {
		tmRaise("list out of range");
	}
	list->nodes[n] = val;
}


void listCheck(TmList* list) {
	if (list->len >= list->cap) {
		int ocap = list->cap;
		int newsize;
        /* in case malloc crash */
        if (ocap < 10) {
        	newsize = ocap + 2;
        } else {
        	newsize = ocap / 2 + ocap;
        }
        /*int newsize = list->cap * 3 / 2 + 1;*/
		list->nodes = tm_realloc(list->nodes, OBJ_SIZE * ocap,
				OBJ_SIZE * newsize);
        list->cap = newsize;
#if GC_DEBUG_LIST
		printf("resize list: from %d to %d\n", OBJ_SIZE * ocap, OBJ_SIZE * list->cap);
#endif
	}
}

void _listAppend(TmList* list, Object obj) {
	listCheck(list);
	list->nodes[list->len] = obj;
	list->len++;
}

/**
 insert
 after node at index of *n*
 */
void ListInsert(TmList* list, int n, Object obj) {
	listCheck(list);
	if (n < 0)
		n += list->len;
	if (n < list->len || n < 0) {
		int i = list->len;
		for (; i > n && i >= 0; i--) {
			list->nodes[i] = list->nodes[i - 1];
		}
		list->nodes[n] = obj;
		list->len++;
	} else {
		list->nodes[list->len] = obj;
		list->len++;
	}
}

int _listIndex(TmList* list, Object v) {
	int i;
	int len = list->len;
	Object* nodes = list->nodes;
	for (i = 0; i < len; i++) {
		if (tm_equals(nodes[i], v)) {
			return i;
		}
	}
	return -1;
}

Object ListRemove(TmList* list, int index) {
	if (index < 0) {
		index += list->len;
	}
	if (index < 0 || index >= list->len) {
		tmRaise("ListRemove(): index out of range, length=%d, index=%d",
				list->len, index);
	}
	Object obj = list->nodes[index];
	int i;
	for (i = index + 1; i < list->len; i++) {
		list->nodes[i - 1] = list->nodes[i];
	}
	(list->len)--;
	return obj;
}

Object ListPop(TmList* list) {
	return ListRemove(list, -1);
}

void _listDel(TmList* list, Object key) {
	tmAssertType(key, TYPE_NUM, "listDel");
	int idx = GET_NUM(key);
	ListRemove(list, idx);
}

Object _listAdd(TmList* list1, TmList*list2) {
	int newl = list1->len + list2->len;
	Object newlist = list_new(newl);
	TmList* list = GET_LIST(newlist);
	list->len = newl;
	int list1_nodes_size = list1->len * OBJ_SIZE;
	memcpy(list->nodes, list1->nodes, list1_nodes_size);
	memcpy(list->nodes + list1->len, list2->nodes, list2->len * OBJ_SIZE);
	return newlist;
}

int ListComputeSize(TmList* list){
	return OBJ_SIZE * list->cap + sizeof(TmList);
}

// belows are builtin methods
//


Object bm_listAppend() {
	const char* szFunc = "list.append";
	Object self = getListArg(szFunc);
	Object v = getObjArg(szFunc);
	APPEND((self), v);
	return NONE_OBJECT;
}

Object bmListPop() {
	Object self = getListArg("list.pop");
	return ListPop(GET_LIST(self));
}
Object bmListInsert() {
	const char* szFunc = "list.insert";
	Object self = getListArg(szFunc);
	int n = getIntArg(szFunc);
	Object v = getObjArg(szFunc);
	ListInsert(GET_LIST(self), n, v);
	return self;
}

Object bm_listIndex() {
	TmList* self = getListPtrArg("listIndex");
	Object v = getObjArg("listIndex");
	return tm_number(_listIndex(self, v));
}

Object bmListReverse() {
	TmList* self = getListPtrArg("listReverse");
	int start = 0, end = self->len - 1;
	while (end > start) {
		Object temp = self->nodes[start];
		self->nodes[start] = self->nodes[end];
		self->nodes[end] = temp;
		end--;
		start++;
	}
	return NONE_OBJECT;
}

Object bmListRemove() {
	TmList* list = getListPtrArg("listRemove");
	int idx = getIntArg("listRemove");
	ListRemove(list, idx);
	return NONE_OBJECT;
}

Object bmListClone() {
	Object self = getObjArg("listClone");
	TmList* list = GET_LIST(self);
	Object _newlist = list_new(list->cap);
	TmList* newlist = GET_LIST(_newlist);
	newlist->len = list->len;
	memcpy(newlist->nodes, list->nodes, list->len * OBJ_SIZE);
	return _newlist;
}

void regListMethods() {
	CLASS_LIST = dict_new();
	regConst(CLASS_LIST);
	regModFunc(CLASS_LIST, "append", bm_listAppend);
	regModFunc(CLASS_LIST, "pop", bmListPop);
	regModFunc(CLASS_LIST, "insert", bmListInsert);
	regModFunc(CLASS_LIST, "index", bm_listIndex);
	regModFunc(CLASS_LIST, "reverse", bmListReverse);
	regModFunc(CLASS_LIST, "remove", bmListRemove);
	regModFunc(CLASS_LIST, "clone", bmListClone);
}

void listIterMark(DataObject* data) {
    TmListIterator* iter = (TmListIterator*) data;
    gcMarkList(iter->list);
}

DataProto* getListIterProto() {
	if(!listIterProto.init) {
		initDataProto(&listIterProto);
		listIterProto.next = listNext;
		listIterProto.dataSize = sizeof(TmListIterator);
		listIterProto.mark = listIterMark;
	}
	return &listIterProto;
}

Object listIterNew(TmList* list) {
	Object data = dataNew(sizeof(TmListIterator));
	TmListIterator *iterator = (TmListIterator*) GET_DATA(data);
	iterator->cur = 0;
	iterator->list = list;
	iterator->proto = getListIterProto();
	return data;
}

Object* listNext(TmListIterator* iterator) {
	if(iterator->cur >= iterator->list->len) {
		return NULL;
	} else {
		iterator->cur += 1;
		return & (iterator->list->nodes[iterator->cur-1]);
	}
}
