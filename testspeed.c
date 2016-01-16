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
Object a_c_7;
Object a_c_8;
Object a_c_9;
Object a_c_10;
Object a_c_11;
Object a_c_12;
Object a_c_13;
Object a_g;
Object a_v_i;
Object a_v_j;
Object a_v_t1;
Object a_v_t2;
/* DEFINE END */
Object a_f_0() {
  a_v_i=a_c_0;
  a_v_j=a_c_0;
  a_v_t1=tm_call(tm_getglobal(a_g,a_c_1),0);
  while(tm_cmp(a_v_i,a_c_2)<0) {
    a_v_j=tm_mul(a_v_i,a_c_3);
    a_v_i=tm_add(a_v_i,a_c_4);
  }

  a_v_t2=tm_call(tm_getglobal(a_g,a_c_1),0);
  tm_call(tm_getglobal(a_g,a_c_5),1,tm_add(a_c_8,tm_call(tm_getglobal(a_g,a_c_7),1,tm_sub(a_v_t2,a_v_t1))));
}

Object a_f_1() {
  Object a_v_n;
  a_v_n=tm_getarg();
  if(tm_equals(a_v_n,a_c_0)||tm_equals(a_v_n,a_c_4)) {
    return a_c_4;
  }

  return tm_add(tm_call(tm_getglobal(a_g,a_c_10),1,tm_sub(a_v_n,a_c_4)),tm_call(tm_getglobal(a_g,a_c_10),1,tm_sub(a_v_n,a_c_11)));
}

void a_main(){
  a_g=dict_new();
  a_c_0=tm_number(0);
  a_c_1=tm_string("clock");
  a_c_2=tm_number(1000000);
  a_c_3=tm_number(34);
  a_c_4=tm_number(1);
  a_c_5=tm_string("print");
  a_c_6=tm_string("global scope: used time = ");
  a_c_7=tm_string("str");
  a_c_8=tm_string("local scope: used time = ");
  a_c_9=tm_string("test");
  a_c_10=tm_string("fib");
  a_c_11=tm_number(2);
  a_c_12=tm_number(30);
  a_c_13=tm_string("fib(30)");
  a_v_i=a_c_0;
  a_v_j=a_c_0;
  a_v_t1=tm_call(tm_getglobal(a_g,a_c_1),0);
  while(tm_cmp(a_v_i,a_c_2)<0) {
    a_v_j=tm_mul(a_v_i,a_c_3);
    a_v_i=tm_add(a_v_i,a_c_4);
  }

  a_v_t2=tm_call(tm_getglobal(a_g,a_c_1),0);
  tm_call(tm_getglobal(a_g,a_c_5),1,tm_add(a_c_6,tm_call(tm_getglobal(a_g,a_c_7),1,tm_sub(a_v_t2,a_v_t1))));
  tm_define(a_g, a_c_9, a_f_0);
  tm_call(tm_getglobal(a_g,a_c_9),0);
  tm_define(a_g, a_c_10, a_f_1);
  a_v_t1=tm_call(tm_getglobal(a_g,a_c_1),0);
  tm_call(tm_getglobal(a_g,a_c_5),1,tm_call(tm_getglobal(a_g,a_c_10),1,a_c_12));
  a_v_t2=tm_call(tm_getglobal(a_g,a_c_1),0);
  tm_call(tm_getglobal(a_g,a_c_5),2,a_c_13,tm_sub(a_v_t2,a_v_t1));
}

int main(int argc, char* argv[]) {
  run_py_func(argc, argv, a_main);
}

