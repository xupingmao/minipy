/*
 * ops.h
 *
 *  Created on: 2014��8��23��
 *      Author: Xu
 */

#ifndef OPS_H_
#define OPS_H_

#include "tm.h"

void      tmSet(Object self, Object key, Object value);
Object    tmGet(Object self, Object key);
Object    tmAdd(Object a, Object b);
Object    tmSub(Object a, Object b);
Object    tmMul(Object a, Object b);
Object    tmDiv(Object a, Object b);
Object    tmMod(Object a, Object b);
Object    tmNeg(Object o) ;
int bObjEq(Object a, Object b);

/*
Object    tmAnd(Object a, Object b);
Object    tmOr (Object a, Object b);
Object objEquals(Object, Object);
Object tmLessThan(Object, Object);
Object tmGreaterThan(Object, Object);
Object tmLessEqual(Object, Object);
Object tmGreaterEqual(Object, Object);
Object objNotEquals(Object, Object);
*/

int tmCmp(Object a, Object b);
Object* tmNext(Object iterator);
Object iterNew(Object collections);
Object tmGet(Object self, Object k);
void tmSet(Object self, Object k, Object v);

Object tmStr(Object obj);
int tmLen(Object o);
Object tmHas(Object a, Object b);

int tmBool(Object v);
int tmIter(Object self, Object *k);
void tmDel(Object self, Object k);

#endif /* OPS_H_ */
