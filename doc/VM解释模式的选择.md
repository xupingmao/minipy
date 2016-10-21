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


### 选择

一开始就选择了基于栈的虚拟机，后来的开发也是基于栈上面来的，近期尝试修改成基于寄存器的，主要原因是

1. 希望在tm2c中加入GC，基于寄存器的更好处理

比如
```
def convert(a):
    # 插入GC检测
    # 基于栈需要跟踪op_stack和locals
    # 基于寄存器的只需要跟踪locals
    # 比如只需要
    # tm2c_push_locals(locals, localsCnt);
    # // function body
    # // check gc
    # tm2c_pop_locals(locals, localsCnt);
    # return ret_val;
    
    # 基于栈的需要
    # tm2c_push_locals(locals, localsCnt);
    # // function body
    # // check gc state (op_stack is empty?). check gc.
    # tm2c_pop_locals(locals, localsCnt);
    return a * 10;

def innerFunc(a,b):
    print(a,b)

def test():
    innerFunc(convert(a), convert(b));
    # 基于寄存器
    # t1 = convert(a)
    # t2 = convert(b)
    # innerFunc(t1,t2)
    
    # 基于栈
    # innerFunc(convert(a), convert(b));
    
```

考虑使用栈的话可以改为
```
innerFunc(convert(a), convert(b));

==>
t1 = convert(a)
t2 = convert(b)
innerFunc(t1,t2)
```

也就是说，函数调用全部使用中间变量来存储(和基于寄存器的一样),这样只需要修改tm2c.

2. 至于性能

2.1 普通运算
```
global:a = b + c

register-based
r2 = r0 + r2 # locals[1] = locals[0] + locals[1]
setglobal a , r2

stack-based
push b
push c
add # stack[0] = b
    # stack[1] = c
    # stack[0] = stack[0] + stack[1]
storeglobal a
```
register-based win!

2.2 函数调用
```
print(convert(a,b))

register-based
push a             # stack[0] = a
push b             # stack[1] = b
r = call convert   # locals[2] = call convert
push r             # stack[0] = locals[2]
call print         # call print

stack-based
push print         # stack[0] = print
push convert       # stack[1] = convert
push a             # stack[2] = a
push b             # stack[3] = b
call 2             # call convert
call 1             # call print
pop                # stack[0] = empty
```

register-based win!

### 实际测试结果

2016.04.03
```
global 
stack-based (optimized) : 500
reg-based               : 2000

local
stack-based : 400
reg-based   : 10000
```
unbelievable!
