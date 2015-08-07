# subpy

## a subset of python

## compiler
1. `tokenize.py` tokenizer modified from tinypy/tokenize
2. `parse.py` handwrite LL parser
3. `encode.py`, `asm.py` code generator

## vm features
1. stack based
2. bytecode defined in `python/libs/tmcode.py`
3. exception handling implemented with setjmp
4. C function and python function.

## vm hierarchy
`vm.c` entry
`interp.c` interpreter

## base objects
1. string, strings are immutable
2. number, double number
3. list
4. dict, this is not hashtable, for the optimization of global access, because the offset of a variable in `globals` dict never change.Besides, object are implemented in dict.
5. function: C functions, normal functions and methods.
6. None
