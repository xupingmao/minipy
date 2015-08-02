#ifndef _STRING_BUILDER_H
#define _STRING_BUILDER_H

#include "../include/object.h"

typedef struct StringBuilder{
	DATA_HEAD
    char* value;
    int len;
    int cap;
}StringBuilder;


StringBuilder* StringBuilderNew();
Object StringBuilderObjNew();
void StringBuilderAppend(StringBuilder* sb, char c);
Object StringBuilderGet(StringBuilder *sb, Object key);
Object StringBuilderToStr(StringBuilder*sb);
void StringBuilderDel(StringBuilder* sb);
DataProto stringBuilderProto;

#endif
