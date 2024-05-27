/*
 * @Author: xupingmao
 * @email: 578749341@qq.com
 * @Date: 2016
 * @LastEditors: xupingmao
 * @LastEditTime: 2024-05-28 01:04:41
 * @FilePath: /minipy/src/number.c
 * @Description: 描述
 */

#include "include/mp.h"

MpObj mp_number(double v){
  MpObj o;
  MP_TYPE(o) = TYPE_NUM;
  GET_NUM(o) = v;
  return o;
}

void mp_format_number(char* des, MpObj num){
    assert(IS_NUM(num));
    
    double v = GET_NUM(num);
    if (fabs(v) - fabs((double)(long) v) < 0.000000001) {
        sprintf(des, "%ld", (long) v);
    } else {
        sprintf(des, "%lf", v);
    }
}

double mp_number_to_double(MpObj num) {
    assert(IS_NUM(num));
    
    return GET_NUM(num);
}

long long mp_number_to_long(MpObj num) {
    assert(IS_NUM(num));

    return (long long) GET_NUM(num);
}