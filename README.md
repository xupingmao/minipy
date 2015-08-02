# subpy

## a subset of python

## compiler
1. `tokenize.py` tokenizer modified from tinypy/tokenize
2. `parse.py` handwrite LL parser
3. `encode.py`, `asm.py` code generator

## virtual machine features
1. stack based
2. bytecode defined in `python/libs/tmcode.py`
3. exception handling implemented with setjmp
4. C function and python function.

## vm hierarchy
`vm.c` entry
`interp.c` interpreter
