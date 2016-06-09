#include "include/tm.h"

Object tm_number(double v){
  Object o;
  TM_TYPE(o) = TYPE_NUM;
  GET_NUM(o) = v;
  return o;
}



void number_format(char* des, Object num){
    double v = GET_NUM(num);
    if (fabs(v) - fabs((double)(long) v) < 0.000000001) {
        sprintf(des, "%ld", (long) v);
    } else {
        sprintf(des, "%g", v);
    }
}