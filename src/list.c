/**
 * description here
 * @author xupingmao
 * @since 2016
 * @modified 2022/06/11 20:26:29
 */
#include "include/mp.h"

/** 
 * @param cap capacity of the list
 * create a list which will not be tracked by garbage collector
 */
MpList* list_new_untracked(int cap) {
    MpList* list = mp_malloc(sizeof(MpList), "list.new_obj");
    list->len = 0;
    if (cap <= 0) {
        cap = 2;
    }
    list->cap = cap;
    list->nodes = mp_malloc(MP_OBJ_SIZE * list->cap, "list.new_nodes");
    return list;
}

MpObj list_new(int cap) {
    MpObj v;
    v.type = TYPE_LIST;
    v.value.list = list_new_untracked(cap);
    return gc_track(v);
}

MpObj list_to_obj(MpList* list) {
    MpObj obj;
    obj.type = TYPE_LIST;
    obj.value.list = list;
    return obj;
}

void list_free(MpList* list) {
    PRINT_MP_GC_INFO_START();
    mp_free(list->nodes, list->cap * MP_OBJ_SIZE);
    mp_free(list, sizeof(MpList));
    PRINT_MP_GC_INFO_END("list", list);
}

MpObj list_get(MpList* list, int n) {
    if (n < 0) {
        n += list->len;
    }
    if (n >= list->len || n < 0) {
        mp_raise("list out of range");
    }
    return list->nodes[n];
}

void list_set(MpList* list, int n, MpObj val) {
    if (n < 0) {
        n += list->len;
    }
    if (n >= list->len || n < 0) {
        mp_raise("list out of range");
    }

    ASSERT_VALID_OBJ(val);
    list->nodes[n] = val;
}

void list_clear(MpList* list) {
    list->len = 0;
}


static void _list_check_cap(MpList* list) {
    assert(list != NULL);
    
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
        list->nodes = mp_realloc(list->nodes, MP_OBJ_SIZE * ocap,
                MP_OBJ_SIZE * newsize, "list.check_cap");
        list->cap = newsize;
#if GC_DEBUG_LIST
        printf("resize list: from %d to %d\n", MP_OBJ_SIZE * ocap, MP_OBJ_SIZE * list->cap);
#endif
    }
}

void list_append(MpList* list, MpObj obj) {
    assert(list != NULL);

    _list_check_cap(list);

    ASSERT_VALID_OBJ(obj);

    list->nodes[list->len] = obj;
    list->len++;
}

MpObj list_from_array(int n, ...) {
    va_list ap;
    int i = 0;
    MpObj list = list_new(n);
    va_start(ap, n);
    for (i = 0; i < n; i++) {
        MpObj item = va_arg(ap, MpObj);
        obj_append(list, item);
    }
    va_end(ap);
    return list;
}


/**
 insert
 after node at index of *n*
 */
void list_insert(MpList* list, int n, MpObj obj) {
    assert(list != NULL);

    _list_check_cap(list);

    ASSERT_VALID_OBJ(obj);

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

int list_index(MpList* list, MpObj v) {
    int i;
    int len = list->len;
    MpObj* nodes = list->nodes;
    for (i = 0; i < len; i++) {
        if (mp_is_equals(nodes[i], v)) {
            return i;
        }
    }
    return -1;
}

MpObj _list_del(MpList* list, int index) {
    if (index < 0) {
        index += list->len;
    }
    if (index < 0 || index >= list->len) {
        mp_raise("_list_del(): index out of range, length=%d, index=%d",
                list->len, index);
    }
    MpObj obj = list->nodes[index];
    int i;
    for (i = index + 1; i < list->len; i++) {
        list->nodes[i - 1] = list->nodes[i];
    }
    (list->len)--;
    return obj;
}

MpObj list_pop(MpList* list) {
    return _list_del(list, -1);
}

void list_del(MpList* list, MpObj key) {
    mp_assert_type(key, TYPE_NUM, "list.del");
    int idx = GET_NUM(key);
    _list_del(list, idx);
}

void list_shorten(MpList* list, int len) {
    if (len > list->len || len < 0) return;
    list->len = len;
}

MpObj list_add(MpList* list1, MpList*list2) {
    int newl = list1->len + list2->len;
    MpObj newlist = list_new(newl);
    MpList* list = GET_LIST(newlist);
    list->len = newl;
    int list1_nodes_size = list1->len * MP_OBJ_SIZE;
    memcpy(list->nodes, list1->nodes, list1_nodes_size);
    memcpy(list->nodes + list1->len, list2->nodes, list2->len * MP_OBJ_SIZE);
    return newlist;
}

// belows are builtin methods
static MpObj list_builtin_append() {
    const char* sz_func = "list.append";
    MpObj self = mp_take_list_obj_arg(sz_func);
    MpObj v = mp_take_obj_arg(sz_func);
    obj_append(self, v);
    return NONE_OBJECT;
}

static MpObj list_builtin_pop() {
    MpObj self = mp_take_list_obj_arg("list.pop");
    return list_pop(GET_LIST(self));
}

static MpObj list_builtin_insert() {
    const char* sz_func = "list.insert";
    MpObj self = mp_take_list_obj_arg(sz_func);
    int n = mp_take_int_arg(sz_func);
    MpObj v = mp_take_obj_arg(sz_func);
    list_insert(GET_LIST(self), n, v);
    return self;
}

static MpObj list_builtin_index() {
    MpList* self = mp_take_list_ptr_arg("list.index");
    MpObj v = mp_take_obj_arg("list.index");
    return number_obj(list_index(self, v));
}

static MpObj list_builtin_reverse() {
    MpList* self = mp_take_list_ptr_arg("list.reverse");
    int start = 0, end = self->len - 1;
    while (end > start) {
        MpObj temp = self->nodes[start];
        self->nodes[start] = self->nodes[end];
        self->nodes[end] = temp;
        end--;
        start++;
    }
    return NONE_OBJECT;
}

static MpObj list_builtin_remove() {
    MpList* list = mp_take_list_ptr_arg("list.remove");
    MpObj obj = mp_take_obj_arg("list.remove");
    int i = 0;
    for (i = 0; i < list->len; i++) {
        MpObj item = list->nodes[i];
        if (mp_is_equals(item, obj)) {
            _list_del(list, i);
            return item;
        }
    }
    return NONE_OBJECT;
}

static MpObj list_builtin_copy() {
    MpObj self = mp_take_obj_arg("list.copy");
    MpList* list = GET_LIST(self);
    MpObj _newlist = list_new(list->cap);
    MpList* newlist = GET_LIST(_newlist);
    newlist->len = list->len;
    memcpy(newlist->nodes, list->nodes, list->len * MP_OBJ_SIZE);
    return _newlist;
}

MpObj list_builtin_clear() {
    MpObj self = mp_take_obj_arg("list.clear");
    MpList* list = GET_LIST(self);
    list->len = 0;
    return self;
}

MpObj list_builtin_extend() {
    MpObj self = mp_take_list_obj_arg("list.extend");
    MpList* other = mp_take_list_ptr_arg("list.extend");
    MpList* selfptr = GET_LIST(self);
    int i = 0;
    for (i = 0; i < other->len; i++) {
        list_append(selfptr, other->nodes[i]);
    }
    return self;
}

size_t list_sizeof(MpList* list) {
    size_t size = 0;
    size += sizeof(MpList);
    size += sizeof(MpObj) * list->cap;
    return size;
}


void MpList_InitMethods() {
    tm->list_proto = dict_new();
    MpModule_RegFunc(tm->list_proto, "append", list_builtin_append);
    MpModule_RegFunc(tm->list_proto, "pop", list_builtin_pop);
    MpModule_RegFunc(tm->list_proto, "insert", list_builtin_insert);
    MpModule_RegFunc(tm->list_proto, "index", list_builtin_index);
    MpModule_RegFunc(tm->list_proto, "reverse", list_builtin_reverse);
    MpModule_RegFunc(tm->list_proto, "remove", list_builtin_remove);
    MpModule_RegFunc(tm->list_proto, "copy", list_builtin_copy);
    MpModule_RegFunc(tm->list_proto, "clear", list_builtin_clear);
    MpModule_RegFunc(tm->list_proto, "extend", list_builtin_extend);
}

MpObj list_iter_new(MpObj list) {
    MpObj data = data_new(1);
    MpData* iterator = GET_DATA(data);
    iterator->cur = 0;
    iterator->end = LIST_LEN(list);
    iterator->inc = 1;
    iterator->next = list_next;
    iterator->data_ptr[0] = list;
    return data;
}

MpObj* list_next(MpData* iterator) {
    if(iterator->cur >= iterator->end) {
        return NULL;
    } else {
        iterator->cur += 1;
        MpList* list = GET_LIST(iterator->data_ptr[0]);
        iterator->cur_obj = list->nodes[iterator->cur-1];
        return & iterator->cur_obj;
    }
}
