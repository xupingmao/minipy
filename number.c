#include "include/tm.h"
#ifndef fabs
    #define fabs(v) (v) > 0 ? v : -v
#endif

Object newNumber(double v){
  Object o;
  TM_TYPE(o) = TYPE_NUM;
  GET_NUM(o) = v;
  return o;
}



void numberFormat(char* des, Object num){
	double v = GET_NUM(num);
	if (fabs(v) - fabs((long) v) < 0.000000001) {
		sprintf(des, "%ld", (long) v);
	} else {
		sprintf(des, "%g", v);
	}
}
