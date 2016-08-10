#include "tm.c"
/* DEFINE START */
Object calc_C0;
Object calc_C1;
Object calc_C2;
Object calc_C3;
Object calc_C4;
Object calc_C5;
Object calc_C6;
Object calc_C7;
Object calc_C8;
Object calc_C9;
Object calc_C10;
Object calc_0g;
Object calc_V__name__;
/* DEFINE END */

Object calc_prog() {
// prog
tm_call(3, calc_next, 0 );
tm_call(4, calc_item, 0 );
while((obj_equals(tm_get_global(calc_0g,calc_C2),calc_C3)||obj_equals(tm_get_global(calc_0g,calc_C2),calc_C4))) {
tm_call(6, calc_next, 0 );
tm_call(7, calc_item, 0 );
}

}


Object calc_item() {
// item
tm_call(10, calc_factor, 0 );
while((obj_equals(tm_get_global(calc_0g,calc_C2),calc_C6)||obj_equals(tm_get_global(calc_0g,calc_C2),calc_C7))) {
tm_call(12, calc_next, 0 );
tm_call(13, calc_factor, 0 );
}

}


Object calc_factor() {
// factor
tm_call(16, calc_next, 1 ,calc_C9);
}


void calc_main(){
calc_0g=dict_new();
calc_C0=string_new("__main__");
calc_C1=string_new("__name__");
calc_C2=string_new("lookahead");
calc_C3=string_new("+");
calc_C4=string_new("-");
calc_C5=string_new("prog");
calc_C6=string_new("*");
calc_C7=string_new("/");
calc_C8=string_new("item");
calc_C9=string_new("number");
calc_C10=string_new("factor");
tm_def_mod("calc.py", calc_0g);
calc_V__name__=calc_C0;
obj_set(calc_0g, calc_C1, calc_V__name__);
def_func(calc_0g,calc_C5, calc_prog);
def_func(calc_0g,calc_C8, calc_item);
def_func(calc_0g,calc_C10, calc_factor);
if(obj_equals(calc_V__name__,calc_C0)) {
tm_call(20, calc_prog, 0 );
}

}

int main(int argc, char* argv[]) {
tm_run_func(argc, argv, "calc.py", calc_main);
}
