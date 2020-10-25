/**
 * less interface is better than more,
 * too many interfaces with similar function will confuse the users.
 * @author xupingmao <578749341@qq.com>
 * @since 2016
 * @modified 2020/09/23 02:05:00
 */
#include "include/mp.h"

/** new hashdict instance, with initial allocated size set to 7.
*   better not similar to computer binary size ( as to say 2 ), 
*   such as 2, 10 etc.
*/
int js_hash(unsigned char* s, int len) {
    int hash = 1315423911;
    int i = 0;
    for(i = 0; i < len; i++)
       hash ^= ((hash << 5) + s[i] + (hash >> 2));
    return abs(hash);
}

/**
 * simple hash function for dict, not used yet.
 * @since 2015-?
 */
int mp_hash(MpObj key) {
    switch(MP_TYPE(key)) {
    case TYPE_STR:return js_hash((unsigned char*) GET_STR(key), GET_STR_LEN(key));
    case TYPE_NUM:return abs((int) GET_NUM(key));
    default: return 0;
    }
    return 0;
}

/**
 * init dictionary
 * @since 2015-?
 */
MpDict* dict_init(){
    int i;
    MpDict * dict = mp_malloc(sizeof(MpDict));
    dict->cap = 3;
    dict->extend = 2;
    dict->nodes = mp_malloc(sizeof(DictNode) * (dict->cap));
    // to mark that the node is not allocated.
    for(i = 0; i < dict->cap; i++){
        dict->nodes[i].used = 0;
    }
    dict->len = 0;
    return dict;
}

MpObj dict_new(){
    MpObj o;
    o.type = TYPE_DICT;
    GET_DICT(o) = dict_init();
    return gc_track(o);
}


void dict_check(MpDict* dict){
    if(dict->len < dict->cap)
        return;
    int osize = dict->cap;
    int i, j, nsize;
    if (osize < 10) {
        nsize = osize + 2;
    } else {
        nsize = osize + osize / 2 + 1;
    }
    DictNode* nodes = mp_malloc(nsize * sizeof(DictNode));
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
    mp_free(temp, osize * sizeof(DictNode));
}

/**
 * free dict object
 * @since 2015-?
 */
void dict_free(MpDict* dict){
    PRINT_OBJ_GC_INFO_START();
    mp_free(dict->nodes, (dict->cap) * sizeof(DictNode));
    mp_free(dict, sizeof(MpDict));
    PRINT_OBJ_GC_INFO_END("dict", dict);
}

/**
 * find a free entry to put dict node
 * @since 2015-?
 */
int findfreepos(MpDict* dict) {
    int i;
    for(i = 0; i < dict->cap; i++) {
        if (dict->nodes[i].used == 0) {
            return i;
        }
    }
    return -1;
}

/**
 * @return node index
 */
int dict_set0(MpDict* dict, MpObj key, MpObj val){
    int i;
    DictNode* node = dict_get_node(dict, key);
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

int dict_set_attr(MpDict* dict, int const_id, MpObj val) {
    int i;
    DictNode* nodes = dict->nodes;
    const_id += 2; /* start from 2, as 0,1 are used by normal node. */
    for (i = 0; i < dict->cap; i++) {
        if (nodes[i].used == const_id) {
            nodes[i].val = val;
            return i;
        }
    }
    MpObj key = GET_CONST(const_id-2);
    i = dict_set0(dict, key, val);
    dict->nodes[i].used = const_id;
    return i;
}

int dict_get_attr(MpDict* dict, int const_id) {
    int i;
    DictNode* nodes = dict->nodes;
    const_id += 2; /* prevent first const to be 0, and normal dict node to be 1. */
    for (i = 0; i < dict->cap; i++) {
        if (nodes[i].used == const_id) {
            return i;
        }
    }
    DictNode* node = dict_get_node(dict, GET_CONST(const_id-2));
    if (node != NULL) {
        node->used = const_id;
        return node - nodes;
    }
    return -1;
}

DictNode* dict_get_node(MpDict* dict, MpObj key){
    //int hash = Dict_hash(key);
    //int idx = hash % dict->cap;
    int i;
    DictNode* nodes = dict->nodes;
    for (i = 0; i < dict->cap; i++) {
        if (nodes[i].used && obj_equals(nodes[i].key, key)) {
            return nodes + i;
        }
    }
    return NULL;
}

MpObj* dict_get_by_str0(MpDict* dict, char* key) {
    //int hash = hash_sz((unsigned char*) key, strlen(key));
    //int idx = hash % dict->cap;
    int i;
    DictNode* nodes = dict->nodes;
    for (i = 0; i < dict->cap; i++) {
        if (nodes[i].used && IS_STR(nodes[i].key)
                && strcmp(GET_STR(nodes[i].key), key) == 0) {
            return &nodes[i].val;
        }
    } 
    return NULL;
}

void dict_set_by_str0(MpDict* dict, char* key, MpObj value) {
    dict_set0(dict, string_from_sz(key), value);
}

void dict_del(MpDict* dict, MpObj key) {
    DictNode* node = dict_get_node(dict, key);
    if (node == NULL) {
        mp_raise("obj_del: key_error %o", key);
    }
    node->used = 0;
    dict->len--;
    return;
}

MpObj dict_keys(MpDict* dict){
    MpObj list = list_new(dict->len);
    int i;
    for(i = 0; i < dict->cap; i++) {
        if (dict->nodes[i].used) {
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
    int i;
    for(i = 0; i < dict->cap; i++) {
        if (dict->nodes[i].used) {
            obj_append(list, dict->nodes[i].val);
        }
    }
    return list;
}

MpObj dict_builtin_copy() {
    MpObj dict = arg_take_dict_obj("dict.copy");
    MpObj new_dict = dict_new();
    int i;
    for(i = 0; i < GET_DICT(dict)->cap; i++) {
        if (GET_DICT(dict)->nodes[i].used) {
            dict_set0(GET_DICT(new_dict), GET_DICT(dict)->nodes[i].key, GET_DICT(dict)->nodes[i].val);
        }
    }
    return new_dict;
}

MpObj dict_builtin_update() {
    MpObj self = arg_take_dict_obj("dict.update");
    MpObj other = arg_take_dict_obj("dict.update");
    int i = 0;

    for (i = 0; i < DICT_LEN(other); i++) {
        dict_set0(GET_DICT(self), GET_DICT(other)->nodes[i].key, GET_DICT(other)->nodes[i].val);
    }
    return self;
}

MpObj dict_builtin_pop() {
    MpObj self = arg_take_dict_obj("dict.pop");
    MpObj key  = arg_take_obj("dict.pop");
    DictNode* node = dict_get_node(GET_DICT(self), key);
    if (node == NULL) {    
        if (arg_has_next()) {
            return arg_take_obj("dict.pop");
        } else {
            mp_raise("KeyError %o", key);
            return NONE_OBJECT;
        }
    } else {
        node->used = 0;
        return node->val;
    }
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
    /*
    MpObj *__iter__ = dict_get_by_str(dict, "__iter__");
    if (__iter__ != NULL) {
        MpObj *next = dict_get_by_str(dict, "next");
        MpObj data = data_new(sizeof(Mp_base_iterator));
        Mp_base_iterator* base_iterator = (Mp_base_iterator*)GET_DATA(data);
        base_iterator->func = *next;
        base_iterator->proto = get_base_iter_proto();
        return data;
    }*/
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
    MpDict* dict = GET_DICT(iterator->data_ptr[0]);
    if (iterator->cur < dict->cap) {
        int i;
        for(i = iterator->cur; i < dict->cap; i++) {
            if (dict->nodes[i].used) {
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
    MpObj obj_key = string_const(key);
    DictNode* node = dict_get_node(GET_DICT(obj), obj_key);
    if (node == NULL) {
        return 0;
    }
    return 1;
}
