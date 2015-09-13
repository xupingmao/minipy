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
void listCheck(TmList*);

/* private end */

Object           list_new(int cap);
TmList*         list_alloc_untracked(int cap);

void             list_set(TmList* list, int n, Object v);
Object list_get(TmList* list, int n);
void list_free(TmList* );
Object list_newFromVaList(int n, ...);
void regListMethods();



DataProto listIterProto;
Object listIterNew(TmList* list);
Object* listNext(TmListIterator* iterator);

/* private functions */
Object _listAdd(TmList*, TmList*);
void _listDel(TmList*list, Object key);
int _listIndex(TmList*, Object val);
void _listAppend(TmList* list, Object v);

/* public functions */
#define listDel(list, key) _listDel(GET_LIST(list), key)
#define listIndex(list, val) _listIndex(GET_LIST(list), val)
#define listAdd(a, b) _listAdd(GET_LIST(a), GET_LIST(b))

/* macros */
#define APPEND(list, v) _listAppend(GET_LIST(list), v)
#define LIST_GET(obj, i) list_get(GET_LIST(obj), i)
#define LIST_NODES(obj) (GET_LIST(obj))->nodes
#define LIST_LEN(obj) GET_LIST(obj)->len

#endif
