/*
 * ops.h
 *
 *  Created on: 2014��8��23��
 *      Author: Xu
 */

#ifndef OPS_H_
#define OPS_H_

#include "tm.h"

void      tm_set(Object self, Object key, Object value);
Object    tm_get(Object self, Object key);
Object    tm_add(Object a, Object b);
Object    tm_sub(Object a, Object b);
Object    tm_mul(Object a, Object b);
Object    tm_div(Object a, Object b);
Object    tm_mod(Object a, Object b);
Object    tm_neg(Object o) ;
int tm_equals(Object a, Object b);

int tm_cmp(Object a, Object b);
Object* tm_next(Object iterator);
Object iter_new(Object collections);
Object tm_get(Object self, Object k);
void tm_set(Object self, Object k, Object v);

Object tmStr(Object obj);
int tm_bool(Object v);
int tmIter(Object self, Object *k);
void tm_del(Object self, Object k);

#endif /* OPS_H_ */
