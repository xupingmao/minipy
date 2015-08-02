#include "include/tm.h"

Object newObj(int type, void * value) {
	Object o;
	TM_TYPE(o) = type;
	switch (type) {
	case TYPE_NUM:
		o.value.num = *(double*) value;
		break;
	case TYPE_STR:
		o.value.str = value;
		break;
	case TYPE_LIST:
		GET_LIST(o) = value;
		break;
	case TYPE_DICT:
		GET_DICT(o) = value;
		break;
	case TYPE_MODULE:
		GET_MODULE(o) = value;
		break;
	case TYPE_FUNCTION:
		GET_FUNCTION(o) = value;
		break;
	case TYPE_NONE:
		break;
	default:
		tmRaise("obj_new: not supported type %d", type);
	}
	return o;
}


void objectFree(Object o) {
	switch (TM_TYPE(o)) {
	case TYPE_STR:
		StringFree(GET_STR_OBJ(o));
		break;
	case TYPE_LIST:
		freeList(GET_LIST(o));
		break;
	case TYPE_DICT:
		freeDict(GET_DICT(o));
		break;
	case TYPE_FUNCTION:
		functionFree(GET_FUNCTION(o));
		break;
	case TYPE_MODULE:
		moduleFree(o.value.mod);
		break;
	case TYPE_DATA:
		GET_DATA_PROTO(o)->free(GET_DATA(o));
		break;
	}
}

Object* dataNext(DataObject* data) {
    tmRaise("next is not defined!");
    return NULL;
}

DataProto* getDefaultDataProto() {
	if(!defaultDataProto.init) {
		defaultDataProto.init = 1;
		defaultDataProto.mark = dataMark;
		defaultDataProto.free = dataFree;
		defaultDataProto.next = dataNext;
		defaultDataProto.get = dataGet;
		defaultDataProto.set = dataSet;
		defaultDataProto.str = dataStr;
		defaultDataProto.dataSize = sizeof(DataProto);
	}
	return &defaultDataProto;
}

void initDataProto(DataProto* proto) {
	proto->mark = dataMark;
	proto->free = dataFree;
	proto->get = dataGet;
	proto->set = dataSet;
	proto->next = dataNext;
	proto->str = dataStr;
	proto->init = 1;
	proto->dataSize = sizeof(TmData);
}

Object dataNew(size_t dataSize) {
	Object data;
	data.type = TYPE_DATA;
	GET_DATA(data) = tmMalloc(dataSize);
/*	GET_DATA_PROTO(data)->next = dataNext;
	GET_DATA_PROTO(data)->mark = dataMark;
	GET_DATA_PROTO(data)->free = dataFree;
	GET_DATA_PROTO(data)->get = dataGet;
	GET_DATA_PROTO(data)->set = dataSet;*/
	GET_DATA_PROTO(data) = getDefaultDataProto();
	return gcTrack(data);
}

void dataMark(DataObject* data) {
    /* */
}

void dataFree(TmData* data) {
	tmFree(data, data->proto->dataSize);
}

Object dataGet(Object self, Object key) {
	return NONE_OBJECT;
}

void dataSet(Object self, Object key, Object value) {

}

Object dataStr(Object self) {
	return newString0("data", -1);
}
