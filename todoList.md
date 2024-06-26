# TodoList

## Small change

- 加入编译标志`-D`来输出编译细节(已实现)
  - 抽象语法树将打印在`tree.txt`中, 符号表将打印在`symbol_table.txt`中
- 逐渐使用`std::string_view`替换`const std::string&`(放弃)
- 函数调用时参数个数允许不匹配(已实现)
- 添加一个参数解析器`ArgParser`来解析命令行参数(已实现)
- 逐步替换掉全局变量
- 允许声明变长参数列表`...`
  - 只有当声明变长参数列表时, 调用时不检查变长参数列表的个数
- 更多的赋值运算符

## Big change

- 重构`ASTNode`的继承结构(已实现)
- 使用`PrattParse`算法重构`Expression`类的部分代码(已实现)
- 支持在`AST`上进行常量传播, 优化开关为`-const-propagation`
- `AST -> Asm`变为`AST -> LLVM IR`
