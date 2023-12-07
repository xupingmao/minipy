/*
 * @Author: xupingmao
 * @email: 578749341@qq.com
 * @Date: 2023-12-07 23:04:49
 * @LastEditors: xupingmao
 * @LastEditTime: 2023-12-07 23:19:55
 * @FilePath: /minipy/src/include/dict.h
 * @Description: 描述
 */
#ifndef MP_DICT_H
#define MP_DICT_H

#include "object.h"

// dict functions
// 哈希函数
int mp_hash(void* s, int len);
int obj_hash(MpObj obj);
int obj_ptr_hash(MpObj* obj);

MpObj            dict_new();
MpObj            dict_new_obj();
MpDict*          dict_new_ptr();
MpDict*          dict_init(MpDict* dict, int cap);
void             dict_free(MpDict* dict);

/* dict_get* functions */
DictNode*        dict_get_node(MpDict* dict, MpObj key);
MpObj*           dict_get_by_cstr(MpDict* dict, char* key);
DictNode*        dict_get_node_by_index(MpDict* dict, int index);
MpObj* dict_get_by_str(MpObj obj, char* key);

/* dict_set* functions */
void dict_set(MpObj obj, MpObj key, MpObj val);
int dict_set0(MpDict* dict, MpObj key, MpObj val);
void dict_set_by_cstr(MpDict* dict, const char* key, MpObj val);

MpObj            dict_keys(MpDict* );
void  dict_print_debug_info(MpDict* dict);
MpObj dict_to_obj(MpDict* dict);

void             dict_del(MpDict* dict, MpObj k);
void             dict_methods_init();
/** dict methods **/
MpObj            dict_iter_new(MpObj dict);
MpObj*           dict_next(MpData* iterator);

#endif
