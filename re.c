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
Object a_c_14;
Object a_c_15;
Object a_c_16;
Object a_c_17;
Object a_c_18;
Object a_c_19;
Object a_c_20;
Object a_c_21;
Object a_c_22;
Object a_c_23;
Object a_c_24;
Object a_c_25;
Object a_c_26;
Object a_c_27;
Object a_c_28;
Object a_c_29;
Object a_c_30;
Object a_c_31;
Object a_c_32;
Object a_g;
Object a_v__ANY;
Object a_v__ONE;
Object a_v__STAR;
Object a_v__SET;
Object a_v__PLUS;
Object a_v__CHAR;
Object a_v__GROUP;
Object a_v_RegError;
Object a_v_RegItem;
Object a_v_code;
Object a_v_i;
Object a_v_c;
/* DEFINE END */
Object a_f_0() {
  Object a_v_self;
  Object a_v_pattern;
  Object a_v_i;
  Object a_v_pre;
  Object a_v_c;
  Object a_v_end;
  Object a_v_set;
  a_v_self=tm_getarg();
  a_v_pattern=tm_getarg();
  a_v_i=tm_getarg();
  a_v_pre=tm_getarg();
  a_v_c=tm_get(a_v_pattern,a_v_i);
  tm_set(a_c_7,a_v_pre,a_v_pre);
  tm_set(a_c_7,tm_getglobal(a_g,a_c_8),tm_get(a_v_pattern,a_v_i));
  if(tm_equals(a_v_c,a_c_9)) {
    tm_set(a_c_7,tm_getglobal(a_g,a_c_10),a_v__ANY);
    tm_set(a_c_7,a_v_i,tm_add(a_v_i,a_c_11));
  }
  else   if(tm_equals(a_v_c,a_c_12)) {
    tm_call(tm_getglobal(a_g,a_c_13),1,a_v_pre);
    tm_set(a_c_7,tm_getglobal(a_g,a_c_10),a_v__ONE);
    tm_set(a_c_7,a_v_i,tm_add(a_v_i,a_c_11));
  }
  else   if(tm_equals(a_v_c,a_c_14)) {
    tm_call(tm_getglobal(a_g,a_c_13),1,a_v_pre);
    tm_set(a_c_7,tm_getglobal(a_g,a_c_10),a_v__STAR);
    tm_set(a_c_7,a_v_i,tm_add(a_v_i,a_c_11));
  }
  else   if(tm_equals(a_v_c,a_c_15)) {
    tm_call(tm_getglobal(a_g,a_c_13),1,a_v_pre);
    tm_set(a_c_7,tm_getglobal(a_g,a_c_10),a_v__PLUS);
    tm_set(a_c_7,a_v_i,tm_add(a_v_i,a_c_11));
  }
  else   if((tm_equals(a_v_c,a_c_16)||tm_equals(a_v_c,a_c_17))) {
    if(tm_equals(a_v_c,a_c_16)) {
      a_v_end=a_c_18;
      tm_set(a_c_7,tm_getglobal(a_g,a_c_10),a_v__SET);
    }
  else{
    a_v_end=a_c_19;
    tm_set(a_c_7,tm_getglobal(a_g,a_c_10),a_v__GROUP);
    }

    a_v_i=tm_add(a_v_i,a_c_11);
    a_v_pre=NONE_OBJECT;
    a_v_set=tm_list(0);
    while((tm_cmp(a_v_i,tm_call(tm_getglobal(a_g,a_c_20),1,a_v_pattern))<0&&(!tm_equals(tm_get(a_v_pattern,a_v_i),a_v_end)))) {
      a_v_pre=tm_call(a_v_RegItem,3,a_v_pattern,a_v_i,a_v_pre);
      tm_call(tm_get(a_v_set,a_c_21),1,a_v_pre);
      a_v_i=tm_get(a_v_pre,a_c_22);
    }

    tm_set(a_c_7,a_v_set,a_v_set);
    if(tm_equals(tm_get(a_v_pattern,a_v_i),a_v_end)) {
      a_v_i=tm_add(a_v_i,a_c_11);
    }

    tm_set(a_c_7,a_v_i,a_v_i);
  }
  else   if(tm_equals(a_v_c,a_c_23)) {
    tm_set(a_c_7,tm_getglobal(a_g,a_c_10),a_v__CHAR);
    tm_set(a_c_7,tm_getglobal(a_g,a_c_8),tm_get(a_v_pattern,tm_add(a_v_i,a_c_11)));
    tm_set(a_c_7,a_v_i,tm_add(a_v_i,a_c_1));
  }
  else{
    tm_set(a_c_7,tm_getglobal(a_g,a_c_10),a_v__CHAR);
    tm_set(a_c_7,tm_getglobal(a_g,a_c_8),a_v_c);
    tm_set(a_c_7,a_v_i,tm_add(a_v_i,a_c_11));
  }

}

Object a_f_1() {
  Object a_v_pattern;
  Object a_v_i;
  Object a_v_pre;
  Object a_v_set;
  a_v_pattern=tm_getarg();
  a_v_i=a_c_0;
  a_v_pre=NONE_OBJECT;
  a_v_set=tm_list(0);
  while(tm_cmp(a_v_i,tm_call(tm_getglobal(a_g,a_c_20),1,a_v_pattern))<0) {
    a_v_pre=tm_call(a_v_RegItem,3,a_v_pattern,a_v_i,a_v_pre);
    tm_call(tm_get(a_v_set,a_c_21),1,a_v_pre);
    a_v_i=tm_get(a_v_pre,a_c_22);
  }

  return a_v_set;
}

Object a_f_2() {
  Object a_v_text;
  Object a_v_pattern;
  a_v_text=tm_getarg();
  a_v_pattern=tm_getarg();
  tm_call(tm_get(tm_call(tm_getglobal(a_g,a_c_26),1,a_v_pattern),a_c_27),1,a_v_text);
}

Object a_f_3() {
  Object a_v_text;
  Object a_v_pattern;
  a_v_text=tm_getarg();
  a_v_pattern=tm_getarg();
  return tm_call(tm_get(tm_call(tm_getglobal(a_g,a_c_25),1,a_v_pattern),a_c_28),1,a_v_text);
}

void a_main(){
  a_g=dict_new();
  a_c_0=tm_number(0);
  a_c_1=tm_number(2);
  a_c_2=tm_number(3);
  a_c_3=tm_number(4);
  a_c_4=tm_number(5);
  a_c_5=tm_number(6);
  a_c_6=tm_number(7);
  a_c_7=tm_string("self");
  a_c_8=tm_string("value");
  a_c_9=tm_string(".");
  a_c_10=tm_string("state");
  a_c_11=tm_number(1);
  a_c_12=tm_string("?");
  a_c_13=tm_string("assert");
  a_c_14=tm_string("*");
  a_c_15=tm_string("+");
  a_c_16=tm_string("[");
  a_c_17=tm_string("(");
  a_c_18=tm_string("]");
  a_c_19=tm_string(")");
  a_c_20=tm_string("len");
  a_c_21=tm_string("append");
  a_c_22=tm_string("i");
  a_c_23=tm_string("\\");
  a_c_24=tm_string("__init__");
  a_c_25=tm_string("compile");
  a_c_26=tm_string("re_compile");
  a_c_27=tm_string("match");
  a_c_28=tm_string("parse");
  a_c_29=tm_string("findall");
  a_c_30=tm_string("abc?+test[123].*");
  a_c_31=tm_string("print");
  a_c_32=tm_string("pre");
  a_v__ANY=a_c_0;
  a_v__ONE=a_c_1;
  a_v__STAR=a_c_2;
  a_v__SET=a_c_3;
  a_v__PLUS=a_c_4;
  a_v__CHAR=a_c_5;
  a_v__GROUP=a_c_6;
  a_v_RegError=dict_new();

  a_v_RegItem=dict_new();
  tm_method(a_v_RegItem,a_c_24,a_f_0);

  tm_define(a_g, a_c_25, a_f_1);
  tm_define(a_g, a_c_27, a_f_2);
  tm_define(a_g, a_c_29, a_f_3);
  a_v_code=tm_call(tm_getglobal(a_g,a_c_25),1,a_c_30);
  a_v_i=a_c_0;
  while(tm_cmp(a_v_i,tm_call(tm_getglobal(a_g,a_c_20),1,a_v_code))<0) {
    a_v_c=tm_get(a_v_code,a_v_i);
    tm_call(tm_getglobal(a_g,a_c_31),4,tm_get(a_v_c,a_c_10),tm_get(a_v_c,a_c_8),tm_get(a_v_c,a_c_22),tm_get(a_v_c,a_c_32));
    a_v_i=tm_add(a_v_i,a_c_11);
  }

}

int main(int argc, char* argv[]) {
  run_py_func(argc, argv, a_main);
}

