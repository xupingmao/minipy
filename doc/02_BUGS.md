# Fixed: 2020_0924_Mac上编译运行测试偶尔出现异常：

```
./minipy ./test/test-main.py

==================================================
|  test-string
==================================================
INFO : RUN ljust [5]
INFO : PASS, result= [test ]
make: *** [test] Segmentation fault: 11
```

原因: `obj_call`没有处理class的`__init__`方法


# 2020_1006_Linux_StackTrace

code:
```
def do_program(tree, env, indent):
    try:
        return do_block(tree, env, 0)
    except Exception as e:
        compile_error(env.fname, env.src, env.token, e)
```

stack trace (original exception traceback is missing)

```
Traceback (most recent call last):
  File "mp_init": in "boot" , at line 353
  File "mp_init": in "execfile" , at line 269
  File "mp2c/mp2c.py": in "#main" , at line 993
  File "mp2c/mp2c.py": in "mp2c" , at line 931
  File "mp2c/mp2c.py": in "do_block" , at line 913
Exception:
  File "mp2c/mp2c.py": in "do_block" at line 913
  obj_add: can not add "is_true_obj(" and None
```