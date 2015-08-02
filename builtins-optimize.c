
typedef struct RangeIter {
	DATA_HEAD
	long cur;
    long inc;
    long stop;
    Object cur_obj;
}RangeIter;

Object* rangeNext(RangeIter* data) {
    long cur = data->cur;
    if (data->inc > 0 && cur < data->stop) {
        data->cur += data->inc;
        data->cur_obj = newNumber(cur);
        return &data->cur_obj;
    } else if (data->inc < 0 && cur > data->stop) {
        data->cur -= data->inc;
        data->cur_obj = newNumber(cur);
        return &data->cur_obj;
    }
    return NULL;
}

static DataProto rangeIter;
DataProto* getRangeIterProto() {
	if(!rangeIter.init) {
		initDataProto(&rangeIter);
		rangeIter.next = rangeNext;
		rangeIter.dataSize = sizeof(RangeIter);
	}
	return &rangeIter;
}

Object blt_Range() {
	long start = 0;
	long end = 0;
	int inc;
    static const char* szFunc = "range";
	switch (tm->argumentsCount) {
	case 1:
		start = 0;
		end = getIntArg(szFunc);
		inc = 1;
		break;
	case 2:
		start = getIntArg(szFunc);
		end = getIntArg(szFunc);
		inc = 1;
		break;
	case 3:
		start = getIntArg(szFunc);
		end = getIntArg(szFunc);
		inc = getIntArg(szFunc);
		break;
	default:
		tmRaise("range([n, [ n, [n]]]), but see %d arguments",
				tm->argumentsCount);
	}
	if (inc == 0)
		tmRaise("range(): increment can not be 0!");
	/* eg. (1, 10, -1),  (10, 1, 1) : range can not be the same signal */
	if (inc * (end - start) < 0)
		tmRaise("range(%d, %d, %d): not valid range!", start, end, inc);
	/*Object list = newList((end - start) / inc);
	long i = 0;
	for (i = start; i < end; i += inc) {
		APPEND(list, newNumber(i));
	}
	return list;*/
    Object data = dataNew(sizeof(RangeIter));
    RangeIter *iterator = (RangeIter*) GET_DATA(data);
	iterator->cur = start;
	iterator->stop = end;
    iterator->inc = inc;
	iterator->proto = getRangeIterProto();
    return data;
}
