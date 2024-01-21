# TodoList

## Small change

- 加入编译标志`-D`来输出编译细节(已实现)
    - 抽象语法树将打印在`tree.txt`中, 符号表将打印在`symbol_table.txt`中
- 逐渐使用`std::string_view`替换`const std::string&`
- 函数调用时参数个数允许不匹配

## Big change

- 重构`ASTNode`的继承结构