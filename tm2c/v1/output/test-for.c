#include "tm2c.c"
#define TM_NO_BIN 1
/* DEFINE START */
Object test_for_C0;
Object test_for_C1;
Object test_for_C2;
Object test_for_C3;
Object test_for_C4;
Object test_for_C5;
Object test_for_0g;
Object test_for_V__name__;
Object test_for_V0;
Object test_for_V1;
Object test_for_Vi;
/* DEFINE END */

Object test_for_kk() {
// kk
Object test_for_Vj;
test_for_V0 = iter_new(array_to_list(3,test_for_C2,test_for_C3,test_for_C4));
Object* test_for_P1;
test_for_P1 = next_ptr(test_for_V0);
while (test_for_P1 != NULL) {
test_for_Vj = *test_for_P1;
tm_call_native(6, test_for_print, 1 ,test_for_Vj);
test_for_P1 = next_ptr(test_for_V0);
}

}


void test_for_main(){
test_for_0g=dict_new();
test_for_C0=string_new("__main__");
test_for_C1=string_new("__name__");
test_for_C2=tm_number(1.0);
test_for_C3=tm_number(2.0);
test_for_C4=tm_number(3.0);
test_for_C5=string_new("kk");
tm_def_mod("test-for.py", test_for_0g);
test_for_V__name__=test_for_C0;
obj_set(test_for_0g, test_for_C1, test_for_V__name__);
test_for_V0 = iter_new(array_to_list(3,test_for_C2,test_for_C3,test_for_C4));
Object* test_for_P1;
test_for_P1 = next_ptr(test_for_V0);
while (test_for_P1 != NULL) {
test_for_Vi = *test_for_P1;
tm_call_native(2, test_for_print, 1 ,test_for_Vi);
test_for_P1 = next_ptr(test_for_V0);
}

def_func(test_for_0g,test_for_C5, test_for_kk);
tm_call_native(8, test_for_kk, 0 );
}

int main(int argc, char* argv[]) {
tm_run_func(argc, argv, "test-for.py", test_for_main);
}
