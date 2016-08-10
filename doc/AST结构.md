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
type   =if,
first  = 条件,
second = if主体,
third = else语句 | None
```

## while 语句
```
{

}
```

## for语句
```
type = for,
first = statement (statement 'in' statement)
second = statementlist
```

## 函数定义(def)

```
type = 'def'
first = name
second = arg_list //  [ arg ]
third = statementlist
```