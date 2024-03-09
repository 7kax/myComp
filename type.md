# 类型

## 类型分类

- void
- basic
  - char
  - signed integer
    - signed char(1)
    - short(2)
    - int(4)
    - long(8)
  - unsigned integer
    - unsigned char(1)
    - unsigned short(2)
    - unsigned int(4)
    - unsigned long(8)
  - floating point
    - float(4)
    - double(8)
- enumerated
- derived
  - array
  - struct
  - union
  - pointer

基于上述分类, 抽象基类`Type`提供如下接口:

- `bool is_void()`
- `bool is_char()`
- `bool is_integer()`
- `bool is_scalar()`
- `bool is_aggregate()`
- `bool is_pointer()`
- `bool is_signed()`
- `bool is_unsigned()`
- `bool is_float()`
- `bool is_array()`
- `bool is_struct()`
- `bool is_union()`
- `bool is_enum()`
- `bool is_arithmetic()`
- `bool is_basic()`
- `bool is_derived()`
- `size_t size()`
  - 类型的总大小
- `size_t alignment()`
  - 类型的对齐大小, 对于基本类型, 通常等于大小
- `std::string str()`
  - 类型的字符串表示, 每种类型都有一个唯一的字符串表示

对于派生类型, 有如下接口:

- array
  - `Type* element()`: 返回数组元素类型
  - `size_t num_elements()`: 返回数组元素个数
- pointer
  - `Type* pointee()`: 返回指针指向的类型
- struct
  - `std::vector<std::pair<Type*, std::string>> fields()`: 返回结构体的字段类型
- union
  - `std::vector<std::pair<Type*, std::string>> fields()`: 返回联合体的字段类型
- enum
  - `std::vector<std::string> fields()`: 返回枚举类型的字段名

工厂类`TypeFactory`提供如下接口:

- `VoidType* get_void()`
- `CharType* get_char()`
- `SignedIntegerType* get_signed(size_t size)`
- `UnsignedIntegerType* get_unsigned(size_t size)`
- `FloatType* get_float(size_t size)`
- `ArrayType* get_array(Type* element, int size)`
- `StructType* get_struct(const std::vector<std::pair<Type*, std::string>>& fields)`
- `UnionType* get_union(const std::vector<std::pair<Type*, std::string>>& fields)`
- `EnumType* get_enum(const std::vector<std::string>& fields)`
- `PointerType* get_pointer(Type* pointee)`

## 类型兼容

函数`bool is_compatible(Type* a, Type* b)`判断两个类型是否兼容, 兼容的定义如下:

- 两个类型相等
- 都是指针类型, 且指向的类型兼容
- 都是数组类型, 且
  - 元素类型兼容
  - 元素个数相等, 或者其中一个类型的元素个数为 0
- 都是结构体/联合体/枚举类型, 且
  - 字段个数相等
  - 对于结构体/联合体, 对应字段的类型兼容, 字段名相同, 顺序相同
  - 对于枚举类型, 字段名相同, 顺序相同
- `char`类型与`signed char`/`unsigned char`都不兼容

## 不完整类型

函数`bool is_incomplete(Type* type)`判断类型是否是不完整类型, 不完整类型的定义如下:

- `void`类型
- 元素个数为 0 的数组类型
- 字段个数为 0 的结构体/联合体类型

## 整数提升

整型提升适用于所有等级小于等于`int`的整数类型

函数`Type* integer_promotion(Type* type)`返回整数提升后的类型, 整数提升的规则如下:

- 如果`int`能容纳所有可能的值, 转换为`int`
- 否则, 转换为`unsigned int`

## 隐式转换

下列情况下将发生隐式转换:

### 指针转换

任何指向`void`的指针类型都可以隐式转换为任何其他指针类型, 但反之不成立

### 如同赋值的转换

- 赋值时, 右值类型转换为左值类型
- 标量初始化时, 右值类型转换为左值类型
- 函数调用时, 实参类型转换为形参类型
- 返回值类型转换为函数返回类型

### 通常算术转换

下列算数运算符的实参将会经历隐式转换, 以获得一个公共类型:

接口`Type* usual_arithmetic_conversion(Type* a, Type* b)`返回两个类型的公共类型, 通常算术转换的规则如下:

- 二元算数: `+`, `-`, `*`, `/`, `%`
- 二元关系: `==`, `!=`, `<`, `>`, `<=`, `>=`
- 二元位运算: `&`, `|`, `^`
- 条件运算: `? :`

隐式转换的规则如下:

- 如果其中一个为`double`, 则另一个转换为`double`
- 否则, 如果其中一个为`float`, 则另一个转换为`float`
- 否则两个操作数均为整数, 则先对两个操作数进行**整数提升**, 然后
  - 如果两个类型相同, 则获得公共类型
  - 否则, 如果两者符号性相同, 则将较小的类型转换为较大的类型
  - 否则两者符号性不同, 则
    - 如果无符号数的**转换等级**大于等于有符号数, 则将有符号数转换为无符号数
    - 否则, 两个操作数均转换为有符号数对应的无符号版本

### 值变换

#### 左值变换

非数组类型的左值表达式**不在**以下情况时将发生隐式转换:

- 作为取值运算符的操作数
- 作为前/后自增减运算符的操作数
- 作为成员访问运算符的左操作数
- 作为赋值与复合赋值运算符的左操作数

使其失去左值性

#### 数组到指针的转换

数组类型的左值表达式**不在**以下情况时将发生隐式转换:

- 作为取址运算符的操作数
- 作为用于数组初始化的字符串字面量

使其转换为指向数组首元素的指针类型

## 转型

除了隐式转换外, 以下情况可以使用显式转型:

- 转换为`void`类型, 此时表达式被求值, 返回值将被丢弃
- 任何整数类型能被转换为任何指针类型
- 任何指针类型可以被转换为任何整数类型
- 任何指向对象的指针能被转换为指向任何其他对象类型的指针

转型后始终获得非左值

函数`bool convertable_to(Type* src, Type* dest)`判断`src`类型是否能转换为`dest`类型
