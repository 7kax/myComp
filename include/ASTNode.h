#ifndef MYCOMP_AST_H
#define MYCOMP_AST_H

#include <optional>

#include "Type.h"
#include "CodeGenerator.h"
#include "Context.h"

namespace myComp {
// Base class for all AST nodes
class ASTNode_ {
  public:
    virtual ~ASTNode_() = default;

    // Add flags for different types of nodes
    virtual bool is_function_definition() const = 0;
    virtual bool is_statement() const = 0;
    virtual bool is_code_block() const = 0;

    virtual Type *type() const = 0;

    // Do code generation
    // Return the register number of the return value if exists
    virtual std::optional<int>
    generate_code(CodeGenerator *code_generator) const = 0;

    // Print the AST node for debugging
    virtual void print(std::ostream &os, int indent) const = 0;
};

// Base class for all statements
class StatementNode : public ASTNode_ {
  public:
    bool is_function_definition() const override { return false; }
    bool is_statement() const override { return true; }
    bool is_code_block() const override { return false; }

    // Add flags for different types of statements
    virtual bool is_expression() const = 0;
    virtual bool is_variable_declaration() const = 0;
    virtual bool is_if() const = 0;
    virtual bool is_while() const = 0;
    virtual bool is_for() const = 0;
    virtual bool is_return() const = 0;
};

class CodeBlockNode : public ASTNode_ {
  public:
    ~CodeBlockNode() override {
        for (auto &statement : statements_) {
            delete statement;
        }
    }

    explicit CodeBlockNode(std::vector<StatementNode *> statements)
        : statements_(std::move(statements)) {}

    bool is_function_definition() const override { return false; }
    bool is_statement() const override { return false; }
    bool is_code_block() const override { return true; }

    // Code block does not have a type
    Type *type() const override {
        throw std::logic_error("Try to get type of code block.");
    }

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;

    void print(std::ostream &os, int indent) const override;

  private:
    std::vector<StatementNode *> statements_;
};

class FunctionDefinitionNode : public ASTNode_ {
  public:
    ~FunctionDefinitionNode() override { delete code_block_; }

    FunctionDefinitionNode(FunctionPrototype_ *prototype,
                           CodeBlockNode *code_block)
        : prototype_(prototype), code_block_(code_block) {}

    bool is_function_definition() const override { return true; }
    bool is_statement() const override { return false; }
    bool is_code_block() const override { return false; }

    // The return type of the function
    Type *type() const override { return prototype_->return_type_; }

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;

    void print(std::ostream &os, int indent) const override;

  private:
    FunctionPrototype_ *prototype_ = nullptr;
    CodeBlockNode *code_block_ = nullptr;
};

// Base class for all expressions
class ExpressionNode : public StatementNode {
  public:
    bool is_expression() const override { return true; }
    bool is_variable_declaration() const override { return false; }
    bool is_if() const override { return false; }
    bool is_while() const override { return false; }
    bool is_for() const override { return false; }
    bool is_return() const override { return false; }

    // Add flags for different types of expressions
    virtual bool is_binary() const = 0;
    virtual bool is_unary() const = 0;
    virtual bool is_leaf() const = 0;

    // Lvalue is a value that can be assigned to
    bool is_lvalue() const { return is_lvalue_; }
    void set_lvalue() { is_lvalue_ = true; }
    void unset_lvalue() { is_lvalue_ = false; }

    void set_type(Type *type) { type_ = type; }
    Type *type() const override { return type_; }

  private:
    Type *type_ = nullptr;
    bool is_lvalue_ = false;
};

class VariableDeclarationNode : public StatementNode {
  public:
    ~VariableDeclarationNode() override { delete initializer_; }

    VariableDeclarationNode(Variable *variable, ExpressionNode *initializer)
        : variable_(variable), initializer_(initializer) {}

    bool is_expression() const override { return false; }
    bool is_variable_declaration() const override { return true; }
    bool is_if() const override { return false; }
    bool is_while() const override { return false; }
    bool is_for() const override { return false; }
    bool is_return() const override { return false; }

    Type *type() const override { return variable_->type; }

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;

    void print(std::ostream &os, int indent) const override;

  private:
    Variable *variable_ = nullptr;
    ExpressionNode *initializer_ = nullptr;
};

class IfNode : public StatementNode {
  public:
    ~IfNode() override {
        delete condition_;
        delete if_block_;
        delete else_block_;
    }

    IfNode(ExpressionNode *condition, CodeBlockNode *if_block,
           CodeBlockNode *else_block)
        : condition_(condition), if_block_(if_block), else_block_(else_block) {}

    bool is_expression() const override { return false; }
    bool is_variable_declaration() const override { return false; }
    bool is_if() const override { return true; }
    bool is_while() const override { return false; }
    bool is_for() const override { return false; }
    bool is_return() const override { return false; }

    Type *type() const override {
        throw std::logic_error("Try to get type of if statement.");
    }

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;

    void print(std::ostream &os, int indent) const override;

  private:
    ExpressionNode *condition_ = nullptr;
    CodeBlockNode *if_block_ = nullptr;
    CodeBlockNode *else_block_ = nullptr;
};

class WhileNode : public StatementNode {
  public:
    ~WhileNode() override {
        delete condition_;
        delete code_block_;
    }

    WhileNode(ExpressionNode *condition, CodeBlockNode *code_block)
        : condition_(condition), code_block_(code_block) {}

    bool is_expression() const override { return false; }
    bool is_variable_declaration() const override { return false; }
    bool is_if() const override { return false; }
    bool is_while() const override { return true; }
    bool is_for() const override { return false; }
    bool is_return() const override { return false; }

    Type *type() const override {
        throw std::logic_error("Try to get type of while statement.");
    }

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;

    void print(std::ostream &os, int indent) const override;

  private:
    ExpressionNode *condition_ = nullptr;
    CodeBlockNode *code_block_ = nullptr;
};

class ForNode : public StatementNode {
  public:
    ~ForNode() override {
        delete initializer_;
        delete condition_;
        delete increment_;
        delete code_block_;
    }

    ForNode(ExpressionNode *initializer, ExpressionNode *condition,
            ExpressionNode *increment, CodeBlockNode *code_block)
        : initializer_(initializer), condition_(condition),
          increment_(increment), code_block_(code_block) {}

    bool is_expression() const override { return false; }
    bool is_variable_declaration() const override { return false; }
    bool is_if() const override { return false; }
    bool is_while() const override { return false; }
    bool is_for() const override { return true; }
    bool is_return() const override { return false; }

    Type *type() const override {
        throw std::logic_error("Try to get type of for statement.");
    }

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;

    void print(std::ostream &os, int indent) const override;

  private:
    ExpressionNode *initializer_ = nullptr;
    ExpressionNode *condition_ = nullptr;
    ExpressionNode *increment_ = nullptr;
    CodeBlockNode *code_block_ = nullptr;
};

class ReturnNode : public StatementNode {
  public:
    ~ReturnNode() override { delete expression_; }

    explicit ReturnNode(ExpressionNode *expression) : expression_(expression) {}

    bool is_expression() const override { return false; }
    bool is_variable_declaration() const override { return false; }
    bool is_if() const override { return false; }
    bool is_while() const override { return false; }
    bool is_for() const override { return false; }
    bool is_return() const override { return true; }

    Type *type() const override { return expression_->type(); }

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;

    void print(std::ostream &os, int indent) const override;

  private:
    ExpressionNode *expression_ = nullptr;
};

// Base class for all binary expressions
class BinaryExpressionNode : public ExpressionNode {
  public:
    ~BinaryExpressionNode() override {
        delete left_;
        delete right_;
    }

    BinaryExpressionNode(std::string op, ExpressionNode *left,
                         ExpressionNode *right)
        : op_(std::move(op)), left_(left), right_(right) {}

    bool is_binary() const override { return true; }
    bool is_unary() const override { return false; }
    bool is_leaf() const override { return false; }

    void print(std::ostream &os, int indent) const override;

    ExpressionNode *get_left() const { return left_; }
    ExpressionNode *get_right() const { return right_; }
    void swap_operands() { std::swap(left_, right_); }

  private:
    std::string op_;
    ExpressionNode *left_ = nullptr;
    ExpressionNode *right_ = nullptr;
};

// Base class for all unary expressions
class UnaryExpressionNode : public ExpressionNode {
  public:
    ~UnaryExpressionNode() override { delete operand_; }

    UnaryExpressionNode(std::string op, ExpressionNode *operand)
        : op_(std::move(op)), operand_(operand) {}

    bool is_binary() const override { return false; }
    bool is_unary() const override { return true; }
    bool is_leaf() const override { return false; }

    void print(std::ostream &os, int indent) const override;

    ExpressionNode *get_operand() const { return operand_; }

  private:
    std::string op_;
    ExpressionNode *operand_ = nullptr;
};

// Base class for all leaf expressions
class LeafExpressionNode : public ExpressionNode {
  public:
    bool is_binary() const override { return false; }
    bool is_unary() const override { return false; }
    bool is_leaf() const override { return true; }

    virtual bool is_variable() const = 0;
    virtual bool is_literal() const = 0;
    virtual bool is_function_call() const = 0;
};

class AddNode : public BinaryExpressionNode {
  public:
    AddNode(ExpressionNode *left, ExpressionNode *right);

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;

  private:
    bool _arithmetic_flag = false;
    bool _left_pointer_flag = false;
    bool _right_pointer_flag = false;
};

class SubtractNode : public BinaryExpressionNode {
  public:
    SubtractNode(ExpressionNode *left, ExpressionNode *right);

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;

  private:
    bool _arithmetic_flag = false;
    bool _pointer_integer_flag = false;
    bool _pointer_flag = false;
};

class MultiplyNode : public BinaryExpressionNode {
  public:
    MultiplyNode(ExpressionNode *left, ExpressionNode *right);

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;
};

class DivideNode : public BinaryExpressionNode {
  public:
    DivideNode(ExpressionNode *left, ExpressionNode *right);

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;
};

class ModuloNode : public BinaryExpressionNode {
  public:
    ModuloNode(ExpressionNode *left, ExpressionNode *right);

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;
};

class InvertNode : public UnaryExpressionNode {
  public:
    explicit InvertNode(ExpressionNode *operand);

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;
};

class OrNode : public BinaryExpressionNode {
  public:
    OrNode(ExpressionNode *left, ExpressionNode *right);

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;
};

class AndNode : public BinaryExpressionNode {
  public:
    AndNode(ExpressionNode *left, ExpressionNode *right);

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;
};

class XorNode : public BinaryExpressionNode {
  public:
    XorNode(ExpressionNode *left, ExpressionNode *right);

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;
};

class LeftShiftNode : public BinaryExpressionNode {
  public:
    LeftShiftNode(ExpressionNode *left, ExpressionNode *right);

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;
};

class RightShiftNode : public BinaryExpressionNode {
  public:
    RightShiftNode(ExpressionNode *left, ExpressionNode *right);

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;
};

class LessNode : public BinaryExpressionNode {
  public:
    LessNode(ExpressionNode *left, ExpressionNode *right);

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;

  private:
    bool _arithmetic_flag = false;
    bool _pointer_flag = false;
};

class LessEqualsNode : public BinaryExpressionNode {
  public:
    LessEqualsNode(ExpressionNode *left, ExpressionNode *right);

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;

  private:
    bool _arithmetic_flag = false;
    bool _pointer_flag = false;
};

class GreaterNode : public BinaryExpressionNode {
  public:
    GreaterNode(ExpressionNode *left, ExpressionNode *right);

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;

  private:
    bool _arithmetic_flag = false;
    bool _pointer_flag = false;
};

class GreaterEqualsNode : public BinaryExpressionNode {
  public:
    GreaterEqualsNode(ExpressionNode *left, ExpressionNode *right);

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;

  private:
    bool _arithmetic_flag = false;
    bool _pointer_flag = false;
};

class EqualsNode : public BinaryExpressionNode {
  public:
    EqualsNode(ExpressionNode *left, ExpressionNode *right);

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;

  private:
    bool _arithmetic_flag = false;
    bool _pointer_flag = false;
    bool _pointer_NULL_flag = false;
};

class NotEqualsNode : public BinaryExpressionNode {
  public:
    NotEqualsNode(ExpressionNode *left, ExpressionNode *right);

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;

  private:
    bool _arithmetic_flag = false;
    bool _pointer_flag = false;
    bool _pointer_NULL_flag = false;
};

class NotNode : public UnaryExpressionNode {
  public:
    explicit NotNode(ExpressionNode *operand);

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;
};

class LogicalOrNode : public BinaryExpressionNode {
  public:
    LogicalOrNode(ExpressionNode *left, ExpressionNode *right);

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;
};

class LogicalAndNode : public BinaryExpressionNode {
  public:
    LogicalAndNode(ExpressionNode *left, ExpressionNode *right);

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;
};

class AssignNode : public BinaryExpressionNode {
  public:
    AssignNode(ExpressionNode *left, ExpressionNode *right);

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;
};

class AddressNode : public UnaryExpressionNode {
  public:
    explicit AddressNode(ExpressionNode *operand);

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;
};

class DereferenceNode : public UnaryExpressionNode {
  public:
    explicit DereferenceNode(ExpressionNode *operand);

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;
};

class PostIncrementNode : public UnaryExpressionNode {
  public:
    explicit PostIncrementNode(ExpressionNode *operand);

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;
};

class PostDecrementNode : public UnaryExpressionNode {
  public:
    explicit PostDecrementNode(ExpressionNode *operand);

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;
};

class PreIncrementNode : public UnaryExpressionNode {
  public:
    explicit PreIncrementNode(ExpressionNode *operand);

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;
};

class PreDecrementNode : public UnaryExpressionNode {
  public:
    explicit PreDecrementNode(ExpressionNode *operand);

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;
};

class NegativeNode : public UnaryExpressionNode {
  public:
    explicit NegativeNode(ExpressionNode *operand);

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;
};

class PositiveNode : public UnaryExpressionNode {
  public:
    explicit PositiveNode(ExpressionNode *operand);

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;
};

class VariableNode : public LeafExpressionNode {
  public:
    explicit VariableNode(Variable *variable);

    bool is_variable() const override { return true; }
    bool is_literal() const override { return false; }
    bool is_function_call() const override { return false; }

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;

    void print(std::ostream &os, int indent) const override;

    Variable *get_variable() const { return variable_; }

  private:
    Variable *variable_ = nullptr;
};

class LiteralNode : public LeafExpressionNode {
  public:
    explicit LiteralNode(Type *type, const std::string &string_value);

    explicit LiteralNode(Type *type, long long int_value);

    bool is_variable() const override { return false; }
    bool is_literal() const override { return true; }
    bool is_function_call() const override { return false; }

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;

    void print(std::ostream &os, int indent) const override;

    long long get_int_value() const;
    std::string get_string_value() const;

  private:
    std::string string_value_;
    long long int_value_{};
    bool is_string_{false};
};

class FunctionCallNode : public LeafExpressionNode {
  public:
    ~FunctionCallNode() override {
        for (auto argument : arguments_) {
            delete argument;
        }
    }

    FunctionCallNode(const std::string &name,
                     std::vector<ExpressionNode *> arguments);

    bool is_variable() const override { return false; }
    bool is_literal() const override { return false; }
    bool is_function_call() const override { return true; }

    std::optional<int>
    generate_code(CodeGenerator *code_generator) const override;

    void print(std::ostream &os, int indent) const override;

  private:
    std::string name_;
    std::vector<ExpressionNode *> arguments_;
};
} // namespace myComp

#endif // MYCOMP_AST_H
