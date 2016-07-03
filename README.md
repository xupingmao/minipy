<pre>
       _____________ ____________________________.___.
      /   _____/    |   \______   \______   \__  |   |
      \_____  \|    |   /|    |  _/|     ___//   |   |
      /        \    |  / |    |   \|    |    \____   |
     /_______  /______/  |______  /|____|    / ______|
             \/                 \/           \/       

</pre>

# subpy
a subset of python, vm implemented with ANSI C and compiler implemented with Python 3.

## How to start
`cd src` run `python setup.py gcc` in console(or you may use other c compiler)

## features

### compiler
1. `tmcode.py` bytecode definition
2. `tokenize.py` tokenizer modified from tinypy/tokenize
3. `parse.py` handwrite LL parser
4. `encode.py` code generator
5. `setup.py` build bytecodes to c array, saved in `bin.c` by default.

### vm features
1. stack based
2. bytecode defined in `src/tmcode.py`
3. exception handling implemented with setjmp
4. C function and python function.
5. exception can be tracked by lineno and handled.

### vm hierarchy
1. `vm.c` entry
2. `interp.c` interpreter
3. `builtins.c` built-in functions
4. `ops.c` operation implementation
5. `tmarg.c` handle arguments
6. `exception.c` handle exception
7. `gc` garbage collector and memory management.
8. `string.c` string functions
9. `number.c` number functions
10. `list.c` list functions
11. `dict.c` dictionary functions
12. `function.c` function functions

### objects
1. string, strings are immutable.
2. number, use `double` as default number.
3. list, array list increase with simple strategy.
4. dict, this is not hashtable, for the optimization of global access, because the offset of a variable in `globals` dict never change.Besides, object are implemented in dict.
5. function, C functions, python functions and methods.
6. None, NoneType object.
7. data, data are other data structure such as iterator.

### tools
1. repl.py repl mode
2. dis.py dissamble bytecode.
4. ast.py print abstract syntax tree.

### LISENCE

MIT
