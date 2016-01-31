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

Object           listNew(int cap);
TmList*         list_alloc_untracked(int cap);

void             list_set(TmList* list, int n, Object v);
Object list_get(TmList* list, int n);
void listFree(TmList* );
void listMethodsInit();


Object listIterNew(TmList* list);
Object* listNext(TmListIterator* iterator);

/* private functions */
Object list_add(TmList*, TmList*);
void list_del(TmList*list, Object key);
int list_index(TmList*, Object val);
void listAppend(TmList* list, Object v);



DataProto listIterProto = { 0 };

/* macros */
#define LIST_GET(obj, i) list_get(GET_LIST(obj), i)
#define LIST_NODES(obj) (GET_LIST(obj))->nodes
#define LIST_LEN(obj) GET_LIST(obj)->len

#endif
