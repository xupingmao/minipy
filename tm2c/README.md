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
Object helloC0;
Object helloC1;
Object helloC2;
Object helloC3;
Object hello0g;
Object helloV__name__;
/* DEFINE END */
void hello_main(){
hello0g=dictNew();
helloC0=stringNew("__main__");
helloC1=stringNew("__name__");
helloC2=stringNew("print");
helloC3=stringNew("hello,world");
tmDefMod("hello.py", hello0g);
helloV__name__=helloC0;
objSet(hello0g, helloC1, helloV__name__);
tmCall(2, tmGetGlobal(hello0g,helloC2), 1 ,helloC3);
}

int main(int argc, char* argv[]) {
tmRunFunc(argc, argv, "hello.py", hello_main);
}
```

