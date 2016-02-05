/*
 * ops.h
 *
 *  Created on: 2014��8��23��
 *      Author: Xu
 */

#ifndef OPS_H_
#define OPS_H_

#include "tm.h"

void      objSet(Object self, Object key, Object value);
Object    objGet(Object self, Object key);
Object    objAdd(Object a, Object b);
Object    objSub(Object a, Object b);
Object    objMul(Object a, Object b);
Object    objDiv(Object a, Object b);
Object    objMod(Object a, Object b);
Object    objNeg(Object o) ;

int    objIn(Object key, Object collection);
int objEquals(Object a, Object b);

int objCmp(Object a, Object b);
Object* nextPtr(Object iterator);
Object iterNew(Object collections);
Object objGet(Object self, Object k);
void objSet(Object self, Object k, Object v);

Object tmStr(Object obj);
int isTrueObj(Object v);
int tmIter(Object self, Object *k);
void objDel(Object self, Object k);

#endif /* OPS_H_ */
