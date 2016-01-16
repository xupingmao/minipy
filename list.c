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
        tm_raise("list out of range");
    }
    return list->nodes[n];
}

void list_set(TmList* list, int n, Object val) {
    if (n < 0) {
        n += list->len;
    }
    if (n >= list->len || n < 0) {
        tm_raise("list out of range");
    }
    list->nodes[n] = val;
}

void list_clear(TmList* list) {
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

void list_append(TmList* list, Object obj) {
    _list_check(list);
    list->nodes[list->len] = obj;
    list->len++;
}

/**
 insert
 after node at index of *n*
 */
void list_insert(TmList* list, int n, Object obj) {
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

int list_index(TmList* list, Object v) {
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

Object list_remove(TmList* list, int index) {
    if (index < 0) {
        index += list->len;
    }
    if (index < 0 || index >= list->len) {
        tm_raise("list_remove(): index out of range, length=%d, index=%d",
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

Object list_pop(TmList* list) {
    return list_remove(list, -1);
}

void list_del(TmList* list, Object key) {
    tmAssertType(key, TYPE_NUM, "list.del");
    int idx = GET_NUM(key);
    list_remove(list, idx);
}

Object list_add(TmList* list1, TmList*list2) {
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


Object list_m_append() {
    const char* szFunc = "list.append";
    Object self = arg_get_list(szFunc);
    Object v = arg_get_obj(szFunc);
    tm_append(self, v);
    return NONE_OBJECT;
}

Object bmlist_pop() {
    Object self = arg_get_list("list.pop");
    return list_pop(GET_LIST(self));
}
Object bmlist_insert() {
    const char* szFunc = "list.insert";
    Object self = arg_get_list(szFunc);
    int n = arg_get_int(szFunc);
    Object v = arg_get_obj(szFunc);
    list_insert(GET_LIST(self), n, v);
    return self;
}

Object bmlist_index() {
    TmList* self = arg_get_list_p("list.index");
    Object v = arg_get_obj("list.index");
    return tm_number(list_index(self, v));
}

Object bmListReverse() {
    TmList* self = arg_get_list_p("list.reverse");
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

Object bmlist_remove() {
    TmList* list = arg_get_list_p("list.remove");
    int idx = arg_get_int("list.remove");
    list_remove(list, idx);
    return NONE_OBJECT;
}

Object bmListClone() {
    Object self = arg_get_obj("listClone");
    TmList* list = GET_LIST(self);
    Object _newlist = list_new(list->cap);
    TmList* newlist = GET_LIST(_newlist);
    newlist->len = list->len;
    memcpy(newlist->nodes, list->nodes, list->len * OBJ_SIZE);
    return _newlist;
}

void list_methods_init() {
    tm->list_proto = dict_new();
    regModFunc(tm->list_proto, "append", list_m_append);
    regModFunc(tm->list_proto, "pop", bmlist_pop);
    regModFunc(tm->list_proto, "insert", bmlist_insert);
    regModFunc(tm->list_proto, "index", bmlist_index);
    regModFunc(tm->list_proto, "reverse", bmListReverse);
    regModFunc(tm->list_proto, "remove", bmlist_remove);
    regModFunc(tm->list_proto, "clone", bmListClone);
}

void listIterMark(DataObject* data) {
    TmListIterator* iter = (TmListIterator*) data;
    gc_markList(iter->list);
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
