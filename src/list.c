#include "include/tm.h"

TmList* untrackedListNew(int cap) {
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

Object listNew(int cap) {
    Object v;
    v.type = TYPE_LIST;
    v.value.list = untrackedListNew(cap);
    return gcTrack(v);
}

void listFree(TmList* list) {
    PRINT_OBJ_GC_INFO_START();
    tm_free(list->nodes, list->cap * OBJ_SIZE);
    tm_free(list, sizeof(TmList));
    PRINT_OBJ_GC_INFO_END("list", list);
}

Object listGet(TmList* list, int n) {
    if (n < 0) {
        n += list->len;
    }
    if (n >= list->len || n < 0) {
        tmRaise("list out of range");
    }
    return list->nodes[n];
}

void listSet(TmList* list, int n, Object val) {
    if (n < 0) {
        n += list->len;
    }
    if (n >= list->len || n < 0) {
        tmRaise("list out of range");
    }
    list->nodes[n] = val;
}

void listClear(TmList* list) {
    list->len = 0;
}


void _list_check(TmList* list) {
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

void listAppend(TmList* list, Object obj) {
    _list_check(list);
    list->nodes[list->len] = obj;
    list->len++;
}

/**
 insert
 after node at index of *n*
 */
void listInsert(TmList* list, int n, Object obj) {
    _list_check(list);
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

int listIndex(TmList* list, Object v) {
    int i;
    int len = list->len;
    Object* nodes = list->nodes;
    for (i = 0; i < len; i++) {
        if (objEquals(nodes[i], v)) {
            return i;
        }
    }
    return -1;
}

Object listRemove(TmList* list, int index) {
    if (index < 0) {
        index += list->len;
    }
    if (index < 0 || index >= list->len) {
        tmRaise("listRemove(): index out of range, length=%d, index=%d",
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

Object listPop(TmList* list) {
    return listRemove(list, -1);
}

void listDel(TmList* list, Object key) {
    tmAssertType(key, TYPE_NUM, "list.del");
    int idx = GET_NUM(key);
    listRemove(list, idx);
}

Object listAdd(TmList* list1, TmList*list2) {
    int newl = list1->len + list2->len;
    Object newlist = listNew(newl);
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


Object list_append() {
    const char* szFunc = "list.append";
    Object self = argTakeListObj(szFunc);
    Object v = argTakeObj(szFunc);
    objAppend(self, v);
    return NONE_OBJECT;
}

Object list_pop() {
    Object self = argTakeListObj("list.pop");
    return listPop(GET_LIST(self));
}
Object list_insert() {
    const char* szFunc = "list.insert";
    Object self = argTakeListObj(szFunc);
    int n = argTakeInt(szFunc);
    Object v = argTakeObj(szFunc);
    listInsert(GET_LIST(self), n, v);
    return self;
}

Object list_index() {
    TmList* self = argTakeListPtr("list.index");
    Object v = argTakeObj("list.index");
    return tmNumber(listIndex(self, v));
}

Object list_reverse() {
    TmList* self = argTakeListPtr("list.reverse");
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

Object list_remove() {
    TmList* list = argTakeListPtr("list.remove");
    Object obj = argTakeObj("list.remove");
    int i = 0;
    for (i = 0; i < list->len; i++) {
        Object item = list->nodes[i];
        if (objEquals(item, obj)) {
            listRemove(list, i);
            return item;
        }
    }
    return NONE_OBJECT;
}

Object list_clone() {
    Object self = argTakeObj("listClone");
    TmList* list = GET_LIST(self);
    Object _newlist = listNew(list->cap);
    TmList* newlist = GET_LIST(_newlist);
    newlist->len = list->len;
    memcpy(newlist->nodes, list->nodes, list->len * OBJ_SIZE);
    return _newlist;
}

Object list_clear() {
    Object self = argTakeObj("list.clear");
    TmList* list = GET_LIST(self);
    list->len = 0;
    return self;
}

void listMethodsInit() {
    tm->list_proto = dictNew();
    regModFunc(tm->list_proto, "append", list_append);
    regModFunc(tm->list_proto, "pop", list_pop);
    regModFunc(tm->list_proto, "insert", list_insert);
    regModFunc(tm->list_proto, "index", list_index);
    regModFunc(tm->list_proto, "reverse", list_reverse);
    regModFunc(tm->list_proto, "remove", list_remove);
    regModFunc(tm->list_proto, "clone", list_clone);
    regModFunc(tm->list_proto, "clear", list_clear);
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
