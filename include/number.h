#ifndef _NUMBER_H
#define _NUMBER_H

#include "tm.h"
Object tmNumber(double v);
void numberFormat(char* des, Object num);


#define GET_NUM(obj) (obj).value.dv

#endif
