<pre>

 __    __     __     __   __     __     ______   __  __    
/\ "-./  \   /\ \   /\ "-.\ \   /\ \   /\  == \ /\ \_\ \   
\ \ \-./\ \  \ \ \  \ \ \-.  \  \ \ \  \ \  _-/ \ \____ \  
 \ \_\ \ \_\  \ \_\  \ \_\\"\_\  \ \_\  \ \_\    \/\_____\ 
  \/_/  \/_/   \/_/   \/_/ \/_/   \/_/   \/_/     \/_____/ 
                                                           
</pre>

# minipy

[![Build Status](https://travis-ci.org/xupingmao/minipy.svg?branch=master)](https://travis-ci.org/xupingmao/minipy)

A mini python interpreter, vm implemented with ANSI C and compiler implemented with Python 3.

## How to start

```
cd minipy
make && make test

# and enjoy yourself ^_^
```

## Features

### Compiler

In `/src/python`

1. `tmcode.py` bytecode definition
2. `lex.py` lex tokenizer
3. `parse.py` handwrite LL parser
4. `encode.py` code generator, just run `python encode.py {script.py}` and you will get generated code.

### VM features
1. stack based
2. bytecode defined in `src/tmcode.py`
3. exception handling implemented with setjmp/longjmp
4. C function and python function.
5. exception traceback
6. mark-sweep garbage collection

### Advanced features
1. tail call optimization


### VM hierarchy
1. `vm.c` entry
2. `interp.c` interpreter
3. `builtins.c` built-in functions
4. `ops.c` operation implementation
5. `tmarg.c` handle arguments
6. `exception.c` handle exception
7. `gc.c` garbage collector and memory management.
8. `string.c` string functions
9. `number.c` number functions
10. `list.c` list functions
11. `dict.c` dictionary functions
12. `function.c` function functions

### Objects
1. string, strings are immutable.
2. number, use `double` as default number.
3. list, array list increase with simple strategy.
4. dict, this is not hashtable, for the optimization of global access, because the offset of a variable in `globals` dict never change.Besides, object are implemented in dict.
5. function, C functions, python functions and methods.
6. None, NoneType object.
7. data, data are other data structure such as iterator.

### Licence

MIT
