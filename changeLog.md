# ChangeLog

## 2024-01-20

- 编译时通过抛出异常的方式报错

## 2024-01-21

- 修复了从内存加载整数时无法正常加载的问题
- 新增了`-D`编译标志，用于输出编译细节
  - 抽象语法树将打印在`tree.txt`中
  - 符号表将打印在`symbol_table.txt`中
- 调整了部分编译错误的报错信息
- 新增了对于`return`语句的检测
- 将`char`类型的实现改为有符号
- 栈帧按照 16 字节对齐
- 对于未识别的 token 增加了报错信息

## 2024-01-22

- 调用函数时不再检查参数个数
- **彻底重写了类型系统, 抛弃了基于枚举类型的实现, 改为使用类继承**
  - 新的类型系统区分了有无符号整数
  - 新的类型系统支持复杂指针类型
- 新增`Utils`文件, 用于存放一些工具函数

## 2024-01-23

- 调整了 Token 系统, 在枚举类型的基础上做了拓展, 应用了享元模式
- 删除了全局对象`scanner`
- 新增了`TokenProcessor`类, 用于预读取所有 Token
  - 该类内置一个`Scanner`对象, 用于读取 Token
  - 该类将 Token 存储在`vector`中, 并提供了`peek`和`next`方法
  - `Parser`与`Expression`类将使用该类, 而不是直接使用`Scanner`
  - 该类可以将 Token 序列写入文件, 用于调试(`token.txt`)

## 2024-01-26

- 将`Expression`类转移到了`Parser`内部

## 2024-03-09a

- **完全重构了抽象语法树**
  - 新的抽象语法树使用了类继承, 并且有了更加严格的类型检查
  - 采用了访问者模式, 用于遍历抽象语法树以及生成代码
- 类型转换更加系统化了, 完全参照了 C 语言的类型转换规则
- 新增了以下文件:
  - `ASTNode.h/cpp`: 定义了抽象语法树的节点
  - `CodeGenerator.h/cpp`: 定义了代码生成器所需的接口
  - `X86_CodeGenerator.h/cpp`: 实现了代码生成器的接口
  - `Variable.h/cpp`: 接替了`SymbolTable.h/cpp`, 用于存储变量的信息
- 对以下文件进行了较大的改动:
  - `Context.h/cpp`: 现在不仅负责上下文的管理, 还记录函数原型
  - `Parser.h/cpp`: 使用了新的抽象语法树
  - `Expression.h/cpp`: 使用了新的抽象语法树
  - `data.h/cpp`: 移除了下列全局对象, 使用静态类代替
    - `symbol_tables`
    - `function`
    - `context`
- 移除了以下文件:
  - `AsmGenerator.h`: 它的功能已经被`CodeGenerator.h`替代
  - `Debugger.h/cpp`: 目前他的功能太少, 直接并入了`main.cpp`
  - `Errors.h/cpp`: 现在编译器直接抛出异常, 不使用 C 风格的错误处理
  - `Function.h/cpp`: 它的功能并入到了`Context.h`中
  - `Generator.h/cpp`: 代码生成的逻辑已经嵌入到抽象语法树中, 不再需要这个它
  - `objects.h`: 最初是为了偷懒才引入的, 发现它会导致编译时间过长, 所以移除了
  - `SymbolTable.h/cpp`: 它的功能已经被`Variable.h/cpp`替代
  - `Tree.h/cpp`: 它的功能已经被`ASTNode`类的构造函数替代
  - `Utils.h/cpp`: 现在各个模块各自维护自己的工具函数, 不再需要这个文件
  - `x86.h/cpp`: 它的功能已经被`X86_CodeGenerator.h/cpp`替代
- 测试进行了微调

## 2024-03-09b

- 清除了大量旧定义, 删除了`defs.h`文件, 其内容已经分散到了各个文件中
- 重新命名了原本和旧定义冲突的类
- 移除了一处`assert`语句, 改为抛出异常

## 2024-03-10

- 取消了声明函数时允许不写参数名称的特性
  - 取消这个特性后是因为修复它比较麻烦, 这个特性将在后续回归
- 将测试方式从`bash`脚本改为`Python`脚本
  - 测试代码和结果文件分开存储
  - `err`开头的结果文件名字改为`out`开头
  - 更新了一部分测试, 他们现在拥有更确切的测试名称以及更复杂的代码
- 修复了`Scanner`不识别百分号的问题, 使得取模运算进行
- 修复了`+`不会被识别为前缀运算符的问题
