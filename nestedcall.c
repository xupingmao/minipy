#include "include/tm.h"
#include "tm.c"
/* DEFINE START */
Object a_c_0;
Object a_c_1;
Object a_c_2;
Object a_c_3;
Object a_c_4;
Object a_c_5;
Object a_c_6;
Object a_g;
Object a_v_k;
/* DEFINE END */
Object a_f_0() {
  Object a_v_a;
  Object a_v_b;
  a_v_a=tm_getarg();
  a_v_b=tm_getarg();
  return tm_add(a_v_a,a_v_b);
}

Object a_f_1() {
  Object a_v_n;
  a_v_n=tm_getarg();
  return tm_add(a_v_n,a_c_1);
}

void a_main(){
  a_g=dict_new();
  a_c_0=tm_string("add");
  a_c_1=tm_number(1);
  a_c_2=tm_string("inc");
  a_c_3=tm_number(2);
  a_c_4=tm_number(3);
  a_c_5=tm_string("print");
  a_c_6=tm_number(12);
  tm_define(a_g, a_c_0, a_f_0);
  tm_define(a_g, a_c_2, a_f_1);
  a_v_k=tm_call(tm_getglobal(a_g,a_c_0),2,tm_call(tm_getglobal(a_g,a_c_2),1,tm_call(tm_getglobal(a_g,a_c_0),2,a_c_1,a_c_3)),a_c_4);
  tm_call(tm_getglobal(a_g,a_c_5),1,a_v_k);
  a_v_k=(a_c_6&&a_c_4);
  tm_call(tm_getglobal(a_g,a_c_5),1,a_v_k);
}

int main(int argc, char* argv[]) {
  run_py_func(argc, argv, a_main);
}

