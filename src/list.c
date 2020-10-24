/**
 * description here
 * @author xupingmao
 * @since 2016
 * @modified 2020/10/21 01:33:45
 */
#include "include/mp.h"

/** 
 * @param cap capacity of the list
 * create a list which will not be tracked by garbage collector
 */
MpList* list_new_untracked(int cap) {
    MpList* list = mp_malloc(sizeof(MpList));
    list->len = 0;
    if (cap <= 0) {
        cap = 2;
    }
    list->cap = cap;
    list->nodes = mp_malloc(OBJ_SIZE * list->cap);
    return list;
}

Object list_new(int cap) {
    Object v;
    v.type = TYPE_LIST;
    v.value.list = list_new_untracked(cap);
    return gc_track(v);
}

void list_free(MpList* list) {
    PRINT_OBJ_GC_INFO_START();
    mp_free(list->nodes, list->cap * OBJ_SIZE);
    mp_free(list, sizeof(MpList));
    PRINT_OBJ_GC_INFO_END("list", list);
}

Object list_get(MpList* list, int n) {
    if (n < 0) {
        n += list->len;
    }
    if (n >= list->len || n < 0) {
        mp_raise("list out of range");
    }
    return list->nodes[n];
}

void list_set(MpList* list, int n, Object val) {
    if (n < 0) {
        n += list->len;
    }
    if (n >= list->len || n < 0) {
        mp_raise("list out of range");
    }
    list->nodes[n] = val;
}

void list_clear(MpList* list) {
    list->len = 0;
}


void _list_check(MpList* list) {
    if (list->len >= list->cap) {
        int ocap = list->cap;
        int newsize;
        /* in case malloc crash */
        if (ocap < 10) {
            newsize = ocap + 2;
        } else {
            newsize = ocap / 2 + ocap + 1;
        }
        /*int newsize = list->cap * 3 / 2 + 1;*/
        list->nodes = mp_realloc(list->nodes, OBJ_SIZE * ocap,
                OBJ_SIZE * newsize);
        list->cap = newsize;
#if GC_DEBUG_LIST
        printf("resize list: from %d to %d\n", OBJ_SIZE * ocap, OBJ_SIZE * list->cap);
#endif
    }
}

void list_append(MpList* list, Object obj) {
    _list_check(list);
    list->nodes[list->len] = obj;
    list->len++;
}

Object list_from_array(int n, ...) {
    va_list ap;
    int i = 0;
    Object list = list_new(n);
    va_start(ap, n);
    for (i = 0; i < n; i++) {
        Object item = va_arg(ap, Object);
        obj_append(list, item);
    }
    va_end(ap);
    return list;
}


/**
 insert
 after node at index of *n*
 */
void list_insert(MpList* list, int n, Object obj) {
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

int list_index(MpList* list, Object v) {
    int i;
    int len = list->len;
    Object* nodes = list->nodes;
    for (i = 0; i < len; i++) {
        if (obj_equals(nodes[i], v)) {
            return i;
        }
    }
    return -1;
}

Object _list_del(MpList* list, int index) {
    if (index < 0) {
        index += list->len;
    }
    if (index < 0 || index >= list->len) {
        mp_raise("_list_del(): index out of range, length=%d, index=%d",
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

Object list_pop(MpList* list) {
    return _list_del(list, -1);
}

void list_del(MpList* list, Object key) {
    mp_assert_type(key, TYPE_NUM, "list.del");
    int idx = GET_NUM(key);
    _list_del(list, idx);
}

void list_shorten(MpList* list, int len) {
    if (len > list->len || len < 0) return;
    list->len = len;
}

Object list_add(MpList* list1, MpList*list2) {
    int newl = list1->len + list2->len;
    Object newlist = list_new(newl);
    MpList* list = GET_LIST(newlist);
    list->len = newl;
    int list1_nodes_size = list1->len * OBJ_SIZE;
    memcpy(list->nodes, list1->nodes, list1_nodes_size);
    memcpy(list->nodes + list1->len, list2->nodes, list2->len * OBJ_SIZE);
    return newlist;
}

// belows are builtin methods

Object list_builtin_append() {
    const char* sz_func = "list.append";
    Object self = arg_take_list_obj(sz_func);
    Object v = arg_take_obj(sz_func);
    obj_append(self, v);
    return NONE_OBJECT;
}

Object list_builtin_pop() {
    Object self = arg_take_list_obj("list.pop");
    return list_pop(GET_LIST(self));
}
Object list_builtin_insert() {
    const char* sz_func = "list.insert";
    Object self = arg_take_list_obj(sz_func);
    int n = arg_take_int(sz_func);
    Object v = arg_take_obj(sz_func);
    list_insert(GET_LIST(self), n, v);
    return self;
}

Object list_builtin_index() {
    MpList* self = arg_take_list_ptr("list.index");
    Object v = arg_take_obj("list.index");
    return number_obj(list_index(self, v));
}

Object list_builtin_reverse() {
    MpList* self = arg_take_list_ptr("list.reverse");
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

Object list_builtin_remove() {
    MpList* list = arg_take_list_ptr("list.remove");
    Object obj = arg_take_obj("list.remove");
    int i = 0;
    for (i = 0; i < list->len; i++) {
        Object item = list->nodes[i];
        if (obj_equals(item, obj)) {
            _list_del(list, i);
            return item;
        }
    }
    return NONE_OBJECT;
}

Object list_builtin_copy() {
    Object self = arg_take_obj("list.copy");
    MpList* list = GET_LIST(self);
    Object _newlist = list_new(list->cap);
    MpList* newlist = GET_LIST(_newlist);
    newlist->len = list->len;
    memcpy(newlist->nodes, list->nodes, list->len * OBJ_SIZE);
    return _newlist;
}

Object list_builtin_clear() {
    Object self = arg_take_obj("list.clear");
    MpList* list = GET_LIST(self);
    list->len = 0;
    return self;
}

Object list_builtin_extend() {
    Object self = arg_take_list_obj("list.extend");
    MpList* other = arg_take_list_ptr("list.extend");
    MpList* selfptr = GET_LIST(self);
    int i = 0;
    for (i = 0; i < other->len; i++) {
        list_append(selfptr, other->nodes[i]);
    }
    return self;
}

void list_methods_init() {
    tm->list_proto = dict_new();
    reg_mod_func(tm->list_proto, "append", list_builtin_append);
    reg_mod_func(tm->list_proto, "pop", list_builtin_pop);
    reg_mod_func(tm->list_proto, "insert", list_builtin_insert);
    reg_mod_func(tm->list_proto, "index", list_builtin_index);
    reg_mod_func(tm->list_proto, "reverse", list_builtin_reverse);
    reg_mod_func(tm->list_proto, "remove", list_builtin_remove);
    reg_mod_func(tm->list_proto, "copy", list_builtin_copy);
    reg_mod_func(tm->list_proto, "clear", list_builtin_clear);
    reg_mod_func(tm->list_proto, "extend", list_builtin_extend);
}

Object list_iter_new(Object list) {
    Object data = data_new(1);
    MpData* iterator = GET_DATA(data);
    iterator->cur = 0;
    iterator->end = LIST_LEN(list);
    iterator->inc = 1;
    iterator->next = list_next;
    iterator->data_ptr[0] = list;
    return data;
}

Object* list_next(MpData* iterator) {
    if(iterator->cur >= iterator->end) {
        return NULL;
    } else {
        iterator->cur += 1;
        MpList* list = GET_LIST(iterator->data_ptr[0]);
        iterator->cur_obj = list->nodes[iterator->cur-1];
        return & iterator->cur_obj;
    }
}
