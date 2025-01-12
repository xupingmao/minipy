<pre>

 __    __     __     __   __     __     ______   __  __    
/\ "-./  \   /\ \   /\ "-.\ \   /\ \   /\  == \ /\ \_\ \   
\ \ \-./\ \  \ \ \  \ \ \-.  \  \ \ \  \ \  _-/ \ \____ \  
 \ \_\ \ \_\  \ \_\  \ \_\\"\_\  \ \_\  \ \_\    \/\_____\ 
  \/_/  \/_/   \/_/   \/_/ \/_/   \/_/   \/_/     \/_____/ 
                                                           
</pre>

# minipy

[![Build Status](https://travis-ci.org/xupingmao/minipy.svg?branch=master)](https://travis-ci.org/xupingmao/minipy)

迷你Python解释器，Python实现的编译器+C语言实现的VM.

注意：本项目主要是用于学习编译器相关原理，如果需要用于生产环境，可以参考以下项目
- [micropython](https://github.com/micropython/micropython)

## 如何开始

### 编译解释器

```
cd minipy
make && make test

# and enjoy yourself ^_^
```

### 打包成可执行文件

```
# 编辑 pack/main.py

# 打包
python3 build.py pack

# 执行打包后的文件
./pack_main
```

## 特性

### 编译器

位于 `/src/python`

- `mp_opcode.py` 字节码定义
- `mp_tokenize.py` 词法分析器，将代码转换成单词(`tokens`)
    - 运行 `python mp_tokenize.py {script.py}` 可以打印出单词
- `mp_parse.py` 语法分析器，将单词(`tokens`)转换成语法树(`Syntax Tree`)
    - 运行 `python mp_parse.py {script.py}` 可以打印出语法树
- `mp_encode.py` 代码生成器，将语法树(`Syntax Tree`)转换成字节码(`opcodes`)
    - 运行 `python mp_encode.py {script.py}` 可以打印出字节码(未处理过的)

### 特性
- [x] 基于栈的计算机模型，字节码定义在 `src/python/mp_opcode.py`
- [x] 支持异常处理，基于`setjmp/longjmp`实现
- [x] 支持Native方法扩展
- [x] 支持常用的Python类型
- [x] 支持函数定义、简单类定义
- [x] Mark-Sweep垃圾回收
- [x] 字符串常量池
- [x] 尾调用优化
- [] DEBUG功能
- [] 用户级线程
- [] 类的继承

### 工具
1. minipy -dis {test.py} 打印字节码(常量处理过)


### 代码结构
01. `main.c` 程序入口
02. `vm.c` 虚拟机入口
03. `execute.c` 解释器
04. `builtins.c` 一些常用的内置方法
05. `obj_ops.c` 对象的操作符实现
06. `argument.c` 函数调用参数API
07. `exception.c` 异常处理
08. `gc.c` 垃圾回收器
09. `string.c` 字符串处理
10. `number.c` 数字处理
11. `list.c`   列表处理
12. `dict.c`   字典处理
13. `function.c` 函数/方法处理

### 类型系统
1. `string`, 是不可变对象
2. `number`, 全部使用double类型
3. `list`, 列表（动态数组）
4. `dict`, 哈希表
5. `function`, 包括native的C函数和自定义的Python函数
6. `class`, 自定义Python类型
7. `None`, None类型
8. `data`, 自定义的C语言类型

### 相关项目

其他Python的实现
- [CPython](https://github.com/python/cpython) Python的官方实现版本
- [micropython](https://github.com/micropython/micropython) 嵌入式版本
- [tinypy](https://github.com/philhassey/tinypy) 64K的迷你版本，支持Python的部分子集
- [cython](https://github.com/cython/cython) Python的超集，可以把Python转换成C语言编译以提升运行速度，同时也可以简化Python的C语言扩展的开发

其他嵌入式脚本语言实现
- Lua
    - [lua](https://github.com/lua/lua) 非常流行的嵌入式脚本引擎
    - [luajit](https://github.com/LuaJIT/LuaJIT) lua的JIT性能优化版本
- JavaScript
    - [quickjs](https://github.com/bellard/quickjs)
    - [duktape](https://github.com/svaarala/duktape)

更多有意思的编译器项目
- [ShivyC](https://github.com/ShivamSarodia/ShivyC) 一个Python编写的C语言编译器
- [NASM](https://github.com/netwide-assembler/nasm) 跨平台的x86汇编和反汇编器
- [bdwgc](https://github.com/ivmai/bdwgc) 一个为C/C++语言打造的GC模块，生产环境广泛应用
- [mkirchner/gc]() 一个更简单的C语言GC库，用于学习

### 协议

- MIT
