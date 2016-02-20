#ifndef _TM_LIST_H
#define _TM_LIST_H
#include "object.h"
typedef struct TmList {
  int marked;
  int len;
  int cap;
  int cur;
  struct Object* nodes;
}TmList;

typedef struct TmListIterator {
    DATA_HEAD
    TmList* list;
    int cur;
}TmListIterator;


/* private */
void     listCheck(TmList*);

/* private end */

Object   listNew(int cap);
TmList*  list_alloc_untracked(int cap);
void     listSet(TmList* list, int n, Object v);
Object   listGet(TmList* list, int n);
void     listFree(TmList* );
void     listMethodsInit();
Object   listIterNew(TmList* list);
Object*  listNext(TmListIterator* iterator);

/* private functions */
Object   listAdd(TmList*, TmList*);
void     listDel(TmList*list, Object key);
int      listIndex(TmList*, Object val);
void     listAppend(TmList* list, Object v);



DataProto listIterProto = { 0 };

/* macros */
#define LIST_GET(obj, i) listGet(GET_LIST(obj), i)
#define LIST_NODES(obj) (GET_LIST(obj))->nodes
#define LIST_LEN(obj) GET_LIST(obj)->len

#endif
