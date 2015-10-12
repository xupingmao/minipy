#ifndef _TM_DATA_H
#define _TM_DATA_H
#include "tm.h"
#include "object.h"

#define DATA_OBJECT_HEAD     \
    int marked;              \
	size_t dataSize;         \
	int init;                \
	void (*mark)();          \
	void (*free)();          \
	Object (*str)();         \
	Object (*get)();         \
	void (*set)();           \
    Object* (*next)();

typedef struct DataProto {
	DATA_OBJECT_HEAD
}DataProto;

#define DATA_HEAD    int marked;      \
	DataProto* proto;

typedef struct TmData {
	DATA_HEAD
}TmData;

typedef struct DataObject {
  DATA_OBJECT_HEAD
}DataObject;

typedef struct _TmBaseIterator {
	DATA_HEAD
	Object func;
  Object ret;
}TmBaseIterator;
static DataProto baseIterProto;

Object dataNew(size_t size);
void dataMark();
void dataFree();
void data_set(Object, Object, Object);
Object data_get(Object, Object);
Object data_str(Object self);

void obj_free(Object o);
Object obj_new(int type, void* value);
DataProto defaultDataProto;
DataProto* getDefaultDataProto();
void initDataProto(DataProto* proto);

#endif
