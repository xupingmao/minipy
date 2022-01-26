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

## 如何开始

```
cd minipy
make && make test

# and enjoy yourself ^_^
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
3. `list`, 列表
4. `dict`, dict对象目前不是使用hashtable实现的，
5. `function`, 包括native的C函数和自定义的Python函数
6. `class`, 类
7. `None`, None类型
8. `data`, data类型可以使用C语言自由扩展

### 协议

- MIT
