## tm2c.py

tm2c.py is a converter that converts python source code to C language source code.

for example, you want to convert the following source code to C.

```
# at file hello.py
print("Hello,World")
```

1. run `python gen_tm2c.py` to generate `tm2c.py` file

2. just run `tm tm2c.py hello.py > hello.c` or `python tm2c.py hello.py > hello.c`

then you can get the C code.

```
#include "tm.c"
/* DEFINE START */
Object hello_c0;
Object hello_c1;
Object hello_c2;
Object hello_c3;
Object hello0g;
Object hello_v__name__;
/* DEFINE END */
void hello_main(){
hello0g=dict_new();
hello_c0=string_new("__main__");
hello_c1=string_new("__name__");
hello_c2=string_new("print");
hello_c3=string_new("hello,world");
tm_def_mod("hello.py", hello0g);
hello_v__name__=hello_c0;
obj_set(hello0g, hello_c1, hello_v__name__);
tm_call(2, tm_get_global(hello0g,hello_c2), 1 ,hello_c3);
}

int main(int argc, char* argv[]) {
tm_run_func(argc, argv, "hello.py", hello_main);
}
```



