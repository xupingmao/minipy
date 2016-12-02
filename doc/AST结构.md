# AST结构


## 基本值

### 数字
```
class = Token,
type  = number,
val  = numVal
```

### 字符串
```
class=  Token,
type = string,
val  = strVal
```
### 名称

```
calss =  Token,
type  = name,
val   = nameVal
```

### None, True, False
```
class  = Token,
type   = None | True | False,
val    = None | True | False
```


### 列表
```
列表 = [] | None
```

### 字典
```
字典 = [ [键, 值]* ]

```

## 语句
```

语句 = 赋值 | 条件 | 循环 | 基本值 | [语句]

```

** 注意 ** 语句可能是列表，需要先使用`isinstance`或者`istype`来判断，然后取type属性判断

## 赋值

```
class  =AstNode,
type   = "=",
first  = 语句 | [语句]  (多重赋值),
second = 语句 | [语句] ,
```

## 操作符
```
class = AstNode,
type  = 操作符 +,-,*,/...
first = 操作符左值
second = 操作符右值
```

## if语句

```
class  = AstNode,
type   = if,
first  = 条件,
second = true-body,
third  = None
```

## if-else

```
{
    type   = "if",
    first  = 条件
    second = true-body
    third  = else-body
}
```

## if-elif-else

```
{
    first  = 条件1
    second = true-body
    third  = {
                type  = "if"
                first  = 条件2
                second = 条件2-body
                third  = elseif/else-body
             }
}
```

## while 语句
```
{
    first  = 条件
    second = body
}
```

## for语句
```
type   = for,
first  = for条件,in语句
second = for-body
```

## 函数定义(def)

```
type = 'def'
first = name
second = arg_list //  [ arg ]
third  = def-body
```