/*
 * 常量池
 *
 *  Created on: 2022/06/10 22:46:34
 *  @author: xupingmao
 *  @modified 2022/06/10 22:53:42
 */

#include "include/mp.h"


int MpConstant_Put(MpObj value) {
	return dict_set0(tm->constants, value, tm->_TRUE);
}

MpObj MpConstant_Get(int index) {
	DictNode *node = dict_get_node_by_index(tm->constants, index);
	assert (node != NULL);
	return node->key;
}