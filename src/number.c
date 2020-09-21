/**
 * description here
 * @author xupingmao
 * @since 2016
 * @modified 2020/09/22 00:04:10
 */

#include "include/mp.h"

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
        sprintf(des, "%lf", v);
    }
}

double number_value(Object num) {
    return GET_NUM(num);
}

long long long_value(Object num) {
    return (long long) GET_NUM(num);
}