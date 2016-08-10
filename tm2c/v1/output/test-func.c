#define TM_NO_BIN 1
#include "tm2c.c"
/* DEFINE START */
Object test_func_C0;
Object test_func_C1;
Object test_func_C2;
Object test_func_C3;
Object test_func_C4;
Object test_func_C5;
Object test_func_C6;
Object test_func_0g;
Object test_func_V__name__;
Object test_func_V_msg;
/* DEFINE END */

Object test_func_this_is_a_func() {
// this_is_a_func
return test_func_C3;
}


Object test_func_test_arg() {
// test_arg
Object test_func_Va;
Object test_func_Vb;
test_func_Va=tm_take_arg();
test_func_Vb=tm_take_arg();
return obj_add(test_func_Va,test_func_Vb);
}


void test_func_main(){
test_func_0g=dict_new();
test_func_C0=string_new("__main__");
test_func_C1=string_new("__name__");
test_func_C2=string_new("hello,world");
test_func_C3=string_new("hello");
test_func_C4=string_new("this_is_a_func");
test_func_C5=string_new("test_arg");
test_func_C6=string_new("_msg");
tm_def_mod("test-func.py", test_func_0g);
test_func_V__name__=test_func_C0;
obj_set(test_func_0g, test_func_C1, test_func_V__name__);
tm_call_native(3, bf_print, 1 ,test_func_C2);
def_func(test_func_0g,test_func_C4, test_func_this_is_a_func);
def_func(test_func_0g,test_func_C5, test_func_test_arg);
test_func_V_msg=tm_call_native(13, test_func_this_is_a_func, 0 );
obj_set(test_func_0g, test_func_C6, test_func_V_msg);
tm_call_native(15, bf_print, 1 ,test_func_V_msg);
}

int main(int argc, char* argv[]) {
tm_run_func(argc, argv, "test-func.py", test_func_main);
}
