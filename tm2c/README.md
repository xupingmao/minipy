
### tm2c
convert python code to c code, still in development.

#### features
1. function/class defintion.
2. function call
3. all operators except `<<`, `>>`, `^`, `&`, `~`, `**`, `//`
4. garbage collection

#### TODO

bug fix and optimization

#### how to use it
1. cd `tm2c`
2. put your python file in `tests`, like `tests/test.py`
3. run `python tm2c.py tests/test.py`
4. you will find a new file in `output/test.c` if there is no error occurs.
5. cd `output`, run `gcc test.c -o test`, you can get a executable file `test`, ^_^