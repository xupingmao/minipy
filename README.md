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
# subpy
Python子集，虚拟机部分使用C语言实现，编译器使用Python3实现(开始是用Python2写的，应该还能兼容，没有测试)。

## How to start
run `python setup.py gcc` in console(or you may use other c compiler)
## 如何开始
在控制台(终端)运行`python setup.py gcc`(你也可以用其他C编译器，我喜欢TCC，小而且快速)

## features
## 特性

### compiler
1. `tokenize.py` tokenizer modified from tinypy/tokenize
2. `parse.py` handwrite LL parser
3. `encode.py`, `asm.py` code generator

### vm features
1. stack based
2. bytecode defined in `python/libs/tmcode.py`
3. exception handling implemented with setjmp
4. C function and python function.
5. exception can be tracked by lineno and handled.

### vm hierarchy
1. `vm.c` entry
2. `interp.c` interpreter
3. `builtins.c` built-in functions
4. `builtins2.c` built-in functions for developers.

### objects
1. string, strings are immutable.
2. number, use `double` as default number.
3. list, array list increase with simple strategy.
4. dict, this is not hashtable, for the optimization of global access, because the offset of a variable in `globals` dict never change.Besides, object are implemented in dict.
5. function, C functions, python functions and methods.
6. None, NoneType object.

### tools
1. repl.py repl mode
2. dis.py dissamble bytecode.
3. libs/tm2c.py convert Python source code to C source code, not done yet.

### 小工具
1. repl.py 交互式模式，直接运行./tm.exe进入交互模式
2. dis.py 反编译
3. libs/tm2c.py 转换TMPython的源代码到C语言源代码，部分完成。

### tm2c.py

位于`subpy/python/libs/tm2c.py`, 例子在`subpy/python/test/tm2c/`目录下面,目前完成的部分比较少，有了大体思路
使用方法

1. 进入`subpy/python`目录,编译`python setup.py gcc`
2. 运行例子`tm.exe libs/tm2c.py testspeed.py`,结果会输出到控制台，可以重定向到文件`tm libs/tm2c.py testspeed.py > ../testspeed.c`
3. 运行`gcc ../testspeed.c -o testspeed.exe`
4. 运行`testspeed.exe`

### 关于虚拟机基于栈和基于寄存器

#### 基于栈
1. 易于实现，不用分配寄存器
2. 占用的字节码长度小一些
3. 更容易反编译，所以在字节码级别优化的程度可以更大（优点&缺点）

#### 基于寄存器
1. 需要分配寄存器，稍微复杂一点，不过使用虚拟寄存器，难度不大
2. 一条指令是相对完整的语句，不会有操作栈，GC的时候不用标记操作栈
3. 解释执行更快，因为没有大量的栈操作
4. 指令更少

That's all.

