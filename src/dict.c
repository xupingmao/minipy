/**
 * less interface is better than more,
 * too many interfaces with similar function will confuse the users.
 * @author xupingmao <578749341@qq.com>
 * @since 2016
 * @modified 2022/01/19 22:29:50
 */
#include "include/mp.h"


/** 
 *   New hashdict instance, with initial allocated size set to 7.
 *   better not similar to computer binary size ( as to say 2 ), 
 *   such as 2, 10 etc.
 */
static int js_hash(unsigned char* s, int len) {
    int hash = 1315423911;
    int i = 0;
    for(i = 0; i < len; i++)
       hash ^= ((hash << 5) + s[i] + (hash >> 2));
    return abs(hash);
}


int mp_hash(void* s, int len) {
    return js_hash((unsigned char*)s, len);
}

/**
 * simple hash function for dict
 * @since 2015-?
 */
int obj_hash(MpObj key) {
    switch(MP_TYPE(key)) {
    case TYPE_STR: {
        return GET_STR_OBJ(key)->hash;
    }
    case TYPE_NUM:return abs((int) GET_NUM(key));
    case TYPE_MODULE:
    case TYPE_FUNCTION:
    default: return 0;
    }

    mp_raise("obj_hash: unexpected reach");
    return 0;
}


void dict_reset(MpDict* dict) {
    // to mark that the node is not allocated.
    for(int i = 0; i < dict->cap; i++){
        dict->nodes[i].used = 0;
        dict->slots[i*2] = -1;
        dict->slots[i*2+1] = -1;
    }
}

/**
 * init dictionary
 * @since 2015
 */
MpDict* dict_init(MpDict* dict, int cap){
    dict->cap = cap;
    dict->extend = 2;
    dict->nodes = mp_malloc(sizeof(DictNode) * (dict->cap));
    dict->slots = mp_malloc(sizeof(int) * (dict->cap) * 2);
    dict->len = 0;
    dict->mask = dict->cap - 1;
    dict->free_start = 0;
    dict_reset(dict);
    return dict;
}

MpObj dict_new(){
    MpDict* dict = mp_malloc(sizeof(MpDict));
    dict_init(dict, 4);

    MpObj o;
    o.type = TYPE_DICT;
    o.value.dict = dict;
    return gc_track(o);
}

MpObj dict_new_obj() {
    return dict_new();
}

static void dict_check(MpDict* dict){
    if(dict->len < dict->cap) {
        return;
    }

    int old_cap = dict->cap;
    int old_len = dict->len;
    DictNode* old_nodes = dict->nodes;
    int* old_slots = dict->slots;
    
    // 计算新的容量
    int new_cap = 0;
    if (old_cap < 10) {
        new_cap = old_cap + 2;
    } else {
        // osize >= 10
        new_cap += old_cap + old_cap / 2 + 2;
        // 必须使用偶数，这样取址运算更有效
        new_cap = new_cap / 2 * 2;
    }
    
    dict_init(dict, new_cap);

    for (int i = 0; i < old_cap; i++) {
        DictNode node = old_nodes[i];
        if (node.used > 0) {
            dict_set0(dict, node.key, node.val);
        }
    }

    #ifdef MP_DEBUG
        if (old_len != dict->len) {
            mp_raise("dict_check: old_len=%d, dict->len=%d\n", old_len, dict->len);
        }
    #endif

    mp_free(old_nodes, old_cap * sizeof(DictNode));
    mp_free(old_slots, 2 * old_cap * sizeof(int));
}

/**
 * free dict object
 * @since 2015-?
 */
void dict_free(MpDict* dict){
    PRINT_OBJ_GC_INFO_START();
    mp_free(dict->nodes, (dict->cap) * sizeof(DictNode));
    mp_free(dict->slots, (dict->cap*2) * sizeof(int));
    mp_free(dict, sizeof(MpDict));
    PRINT_OBJ_GC_INFO_END("dict", dict);
}

/**
 * find a free entry to put dict node
 * @since 2015-?
 */
static int findfreepos(MpDict* dict) {
    for(int i = dict->free_start; i < dict->cap; i++) {
        if (dict->nodes[i].used <= 0) {
            return i;
        }
    }

    mp_raise("findfreepos: unexpected reach");
    return -1;
}


// 调试功能
static void print_debug_info(MpDict* dict) {
    for (int i = 0; i < dict->cap; i++) {
        mp_printf("nodes[%d].key=%o\n", i, dict->nodes[i].key);
        mp_printf("nodes[%d].used=%d\n", i, dict->nodes[i].used);
    }
    for (int i = 0; i < dict->cap * 2; i++) {
        mp_printf("slots[%d]=%d\n", i, dict->slots[i]);
    }
}

/**
 * @return node index
 */
int dict_set0(MpDict* dict, MpObj key, MpObj val){
    DictNode* node = dict_get_node(dict, key);
    if (node != NULL) {
        node->val = val;
        return (node - dict->nodes);
    }
    dict_check(dict);

    int pos = findfreepos(dict);
    dict->len++;

    // 插入新数据
    int hash = obj_hash(key);
    dict->nodes[pos].hash = hash;
    dict->nodes[pos].used = 1;
    dict->nodes[pos].key = key;
    dict->nodes[pos].val = val;

    int start = hash & dict->mask;
    int is_slot_set = FALSE;
    for (int i = start; i < start + dict->cap; i++) {
        if (dict->slots[i] < 0) {
            dict->slots[i] = pos;
            is_slot_set = TRUE;
            break;
        }
    }

    if (is_slot_set == FALSE) {
        print_debug_info(dict);
        mp_raise("dict_set0: can not found valid slot!");
    }

    // 更新空闲索引下标
    dict->free_start = pos + 1;
    return pos;
}

/**
 * @since 2016-11-27
 */
int dict_get0(MpDict* dict, MpObj key) {
    DictNode* node = dict_get_node(dict, key);
    if (node != NULL) {
        return node - dict->nodes;
    }
    return -1;
}


DictNode* dict_get_node_old(MpDict* dict, MpObj key){
    int i = 0;
    int hash = obj_hash(key);
    DictNode* nodes = dict->nodes;
    for (i = 0; i < dict->cap; i++) {
        // 为空或者被删除
        if (nodes[i].used <= 0) {
            continue;
        }

        if (hash != nodes[i].hash) {
            continue;
        }

        if (is_obj_equals(nodes[i].key, key)) {
            return nodes + i;
        }
    }
    return NULL;
}

DictNode* dict_get_node_new(MpDict* dict, MpObj key){
    int hash = obj_hash(key);
    DictNode* nodes = dict->nodes;
    // 计算开始位置
    int start = hash & dict->mask;

    // mp_printf("dict_get_node: start=%d, mask=%d, len=%d, cap=%d, key=%o\n", 
    //     start, dict->mask, dict->len, dict->cap, key);

    for (int i = start; i < start + dict->cap; i++) {
        int index = dict->slots[i];

        if (index == -1) {
            // 没有值，退出
            return NULL;
            // continue;
        }

        #ifdef MP_DEBUG
            if (index < 0 || index >= dict->cap) {
                mp_raise("dict_get_node: unexpected index = %d", index);
            }
        #endif

        DictNode* node = nodes+index;

        // 为空或者被删除
        if (node->used <= 0) {
            continue;
        }

        if (hash != node->hash) {
            continue;
        }

        if (is_obj_equals(node->key, key)) {
            // mp_printf("dict_get_node: found node, key=%o, index=%d\n", key, index);
            return nodes + index;
        }
    }
    return NULL;
}

MpObj* dict_get_by_cstr(MpDict* dict, char* key) {
    //int hash = hash_cstr((unsigned char*) key, strlen(key));
    //int idx = hash % dict->cap;
    int i = 0;
    DictNode* nodes = dict->nodes;
    for (i = 0; i < dict->cap; i++) {
        // 0:未使用 -1:被删除
        if (nodes[i].used <= 0) {
            continue;
        }

        if (IS_STR(nodes[i].key)
                && strcmp(GET_CSTR(nodes[i].key), key) == 0) {
            return &nodes[i].val;
        }
    } 
    return NULL;
}

void dict_set_by_cstr(MpDict* dict, const char* key, MpObj value) {
    dict_set0(dict, string_from_cstr(key), value);
}

MpObj dict_pop(MpDict* dict, MpObj key) {
    DictNode* node = dict_get_node(dict, key);
    if (node == NULL) {
        mp_raise("dict_pop: key_error %o", key);
    }
    node->used = -1;
    dict->len--;
    int node_index = node - dict->nodes;
    dict->free_start = MIN(dict->free_start, node_index);
    return node->val;
}

void dict_del(MpDict* dict, MpObj key) {
    dict_pop(dict, key);
}

MpObj dict_keys(MpDict* dict){
    MpObj list = list_new(dict->len);
    int i;
    for(i = 0; i < dict->cap; i++) {
        if (dict->nodes[i].used > 0) {
            obj_append(list, dict->nodes[i].key);
        }
    }
    return list;
}

MpObj dict_builtin_keys(){
    MpObj dict = arg_take_dict_obj("dict.keys");
    return dict_keys(GET_DICT(dict));
}

MpObj dict_builtin_values() {
    MpObj _d = arg_take_dict_obj("dict.values");
    MpDict* dict = GET_DICT(_d);
    MpObj list = list_new(dict->len);
    for(int i = 0; i < dict->cap; i++) {
        if (dict->nodes[i].used > 0) {
            obj_append(list, dict->nodes[i].val);
        }
    }
    return list;
}

MpObj dict_builtin_copy() {
    MpObj dict = arg_take_dict_obj("dict.copy");
    MpObj new_dict = dict_new();

    for(int i = 0; i < GET_DICT(dict)->cap; i++) {
        if (GET_DICT(dict)->nodes[i].used > 0) {
            dict_set0(GET_DICT(new_dict), GET_DICT(dict)->nodes[i].key, GET_DICT(dict)->nodes[i].val);
        }
    }
    return new_dict;
}

MpObj dict_builtin_update() {
    MpObj self = arg_take_dict_obj("dict.update");
    MpObj other = arg_take_dict_obj("dict.update");

    for (int i = 0; i < DICT_LEN(other); i++) {
        dict_set0(GET_DICT(self), GET_DICT(other)->nodes[i].key, GET_DICT(other)->nodes[i].val);
    }
    return self;
}

MpObj dict_builtin_pop() {
    MpObj self = arg_take_dict_obj("dict.pop");
    MpObj key  = arg_take_obj("dict.pop");
    return dict_pop(GET_DICT(self), key);
}

/**
 * init dict methods
 * @since 2015-?
 */
void dict_methods_init() {
    tm->dict_proto = dict_new();
    /* build dict class */
    reg_mod_func(tm->dict_proto, "keys",   dict_builtin_keys);
    reg_mod_func(tm->dict_proto, "values", dict_builtin_values);
    reg_mod_func(tm->dict_proto, "copy",   dict_builtin_copy);
    reg_mod_func(tm->dict_proto, "update", dict_builtin_update);
    reg_mod_func(tm->dict_proto, "pop",    dict_builtin_pop);
}

/**
 * create a dict iterator from dict object
 * @since 2016-?
 */
MpObj dict_iter_new(MpObj dict) {
    mp_assert_type(dict, TYPE_DICT, "dict_iter_new");
    MpObj data = data_new(1);
    MpData* iterator = GET_DATA(data);
    iterator->cur = 0;
    iterator->end = DICT_LEN(dict);
    iterator->inc = 1;
    iterator->data_ptr[0] = dict;
    iterator->next = dict_next;
    return data;
}

/**
 * get next node pointer
 * @since 2015-?
 */
MpObj* dict_next(MpData* iterator) {
    mp_assert_type(iterator->data_ptr[0], TYPE_DICT, "dict_next");
    MpDict* dict = GET_DICT(iterator->data_ptr[0]);
    if (iterator->cur < dict->cap) {
        for(int i = iterator->cur; i < dict->cap; i++) {
            if (dict->nodes[i].used > 0) {
                iterator->cur = i + 1;
                return &dict->nodes[i].key;
            }
        }
        return NULL;
    }
    return NULL;
}


/**
 * get attribute by char array
 * @since 2016-09-02
 */
MpObj mp_getattr(MpObj obj, char* key) {
    // TODO optimize string find
    MpObj obj_key = string_new(key);
    return obj_get(obj, obj_key);
}


/**
 * get attribute by char array
 * @since 2016-09-02
 */
void mp_setattr(MpObj obj, char* key, MpObj value) {
    MpObj obj_key = string_new(key);
    obj_set(obj, obj_key, value);
}

int mp_hasattr(MpObj obj, char* key) {
    mp_assert_type(obj, TYPE_DICT, "mp_hasattr");

    MpObj obj_key = string_const(key);
    DictNode* node = dict_get_node(GET_DICT(obj), obj_key);
    if (node == NULL) {
        return 0;
    }
    return 1;
}
