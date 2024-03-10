# ASTNode 设计

ASTNode 是一个抽象类, 用于表示抽象语法树中的节点, 基本接口如下:

- `bool is_function_definition()`: 返回节点是否是函数定义
- `bool is_code_block()`: 返回节点是否是代码块
- `bool is_statement()`: 返回节点是否是语句
- `Type *type()`: 返回节点的类型, 如果节点没有类型, 抛出异常
- `std::optional<int> generate_code()`: 生成节点的代码, 返回存储返回值的寄存器, 可以没有返回值
- `std::string str()`: 返回节点的字符串表示

## ASTNode 的子类

- `FunctionDefinitionNode`: 表示函数**定义**
  - 内部包含一个`CodeBlockNode`节点, 用于表示函数体
  - 内部包含一个`FunctionPrototype`, 用于表示函数的原型
  - `Type *type()`: 返回函数的返回值类型
- `CodeBlockNode`: 表示代码块
  - 内部包含一个`std::vector<StatementNode *>`节点, 用于表示代码块中的语句
- `StatementNode`: 表示语句, 是一个抽象类
  - `bool is_statement()`: 返回`true`
  - 新增接口`bool is_expression()`
  - 新增接口`bool is_variable_declaration()`
  - 新增接口`bool is_return()`
  - 新增接口`bool is_if()`
  - 新增接口`bool is_while()`
  - 新增接口`bool is_for()`

## StatementNode 的子类

- `ExpressionNode`: 表示表达式, 是一个抽象类
  - 内部包含一个`Type *`, 用于表示表达式的类型
  - 内部包含一个`bool`, 用于表示表达式是否是左值
  - `bool is_expression()`: 返回`true`
  - `Type *type()`: 返回表达式的类型
  - 新增接口`bool is_binary()`
  - 新增接口`bool is_unary()`
  - 新增接口`bool is_leaf()`
  - 新增接口`bool is_lvalue()`
  - 新增接口`void set_lvalue()`
  - 新增接口`void unset_lvalue()`
- `VariableDeclarationNode`: 表示变量声明
  - 内部包含一个`Variable`, 用于表示变量
  - 内部包含一个`ExpressionNode`, 用于表示变量的初始化表达式
  - `bool is_variable_declaration()`: 返回`true`
  - `Type *type()`: 返回变量的类型
- `IfNode`: 表示 if 语句
  - 内部包含一个`ExpressionNode`, 用于表示条件
  - 内部包含一个`CodeBlockNode`, 用于表示 if 语句的执行体
  - 内部包含一个`CodeBlockNode`, 用于表示 else 语句的执行体
  - `bool is_if()`: 返回`true`
- `WhileNode`: 表示 while 语句
  - 内部包含一个`ExpressionNode`, 用于表示条件
  - 内部包含一个`CodeBlockNode`, 用于表示 while 语句的执行体
  - `bool is_while()`: 返回`true`
- `ForNode`: 表示 for 语句
  - 内部包含一个`ExpressionNode`, 用于表示初始化语句
  - 内部包含一个`ExpressionNode`, 用于表示条件
  - 内部包含一个`ExpressionNode`, 用于表示更新语句
  - 内部包含一个`CodeBlockNode`, 用于表示 for 语句的执行体
  - `bool is_for()`: 返回`true`
- `ReturnNode`: 表示返回语句
  - 内部包含一个`ExpressionNode`, 用于表示返回值
  - `bool is_return()`: 返回`true`

## ExpressionNode 的继承体系

- `BinaryExpressionNode`: 表示二元表达式
  - 内部包含一个`ExpressionNode`, 用于表示左操作数
  - 内部包含一个`ExpressionNode`, 用于表示右操作数
  - 内部包含一个`std::string`, 用于表示操作符
- `UnaryExpressionNode`: 表示一元表达式
  - 内部包含一个`ExpressionNode`, 用于表示操作数
  - 内部包含一个`std::string`, 用于表示操作符
- `LeafExpressionNode`: 表示叶子表达式

### BinaryExpressionNode 的子类

包含以下运算符: ADD, SUBTRACT, MULTIPLY, DIVIDE, MODULO, EQUALS, NEQ, LESS, GREATER, LESS_EQ, GREATER_EQ, OR, LOGICAL_OR, AND, LOGICAL_AND, XOR, L_SHIFT, R_SHIFT, ASSIGN

### UnaryExpressionNode 的子类

包含以下运算符: ADDRESS, DEREFERENCE, INVERT, NOT, POST_INC, POST_DEC, PRE_INC, PRE_DEC, NEGATIVE, POSITIVE

### LeafExpressionNode 的子类

- `VariableNode`: 表示变量
  - 内部包含一个`Variable`, 用于表示变量
- `LiteralNode`: 表示字面量
  - 内部包含一个`std::string`, 用于表示字面量的字符串
  - 内部包含一个`long long`, 用于表示字面量的值
- `FunctionCallNode`: 表示函数调用
  - 内部包含一个`std::string`, 用于表示函数名
  - 内部包含一个`std::vector<ExpressionNode *>`, 用于表示实参列表
