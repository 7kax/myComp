#include <algorithm>

#include "ASTNode.h"

namespace {
using namespace myComp;
int do_arithmetic(const BinaryExpressionNode *node,
                  CodeGenerator *code_generator,
                  int (CodeGenerator::*op)(int, int, Type *)) {
    int left_reg = node->get_left()->generate_code(code_generator).value();
    int right_reg = node->get_right()->generate_code(code_generator).value();
    Type *left_type = node->get_left()->type();
    Type *right_type = node->get_right()->type();
    Type *result_type = node->type();
    code_generator->type_cast(left_reg, left_type, result_type);
    code_generator->type_cast(right_reg, right_type, result_type);
    return (code_generator->*op)(left_reg, right_reg, result_type);
}

int do_arithmetic(const UnaryExpressionNode *node,
                  CodeGenerator *code_generator,
                  int (CodeGenerator::*op)(int, Type *)) {
    int operand_reg =
        node->get_operand()->generate_code(code_generator).value();
    Type *operand_type = node->get_operand()->type();
    Type *result_type = node->type();
    code_generator->type_cast(operand_reg, operand_type, result_type);
    return (code_generator->*op)(operand_reg, result_type);
}

int arithmetic_compare(const BinaryExpressionNode *node,
                       CodeGenerator *code_generator,
                       int (CodeGenerator::*op)(int, int, Type *)) {
    int left_reg = node->get_left()->generate_code(code_generator).value();
    int right_reg = node->get_right()->generate_code(code_generator).value();
    Type *left_type = node->get_left()->type();
    Type *right_type = node->get_right()->type();
    Type *common_type = usual_arithmetic_conversion(left_type, right_type);
    code_generator->type_cast(left_reg, left_type, common_type);
    code_generator->type_cast(right_reg, right_type, common_type);
    return (code_generator->*op)(left_reg, right_reg, common_type);
}

int pointer_compare(const BinaryExpressionNode *node,
                    CodeGenerator *code_generator,
                    int (CodeGenerator::*op)(int, int, Type *)) {
    int left_reg = node->get_left()->generate_code(code_generator).value();
    int right_reg = node->get_right()->generate_code(code_generator).value();
    Type *left_type = node->get_left()->type();
    Type *right_type = node->get_right()->type();
    return (code_generator->*op)(
        left_reg, right_reg, TypeFactory::get_pointer(TypeFactory::get_void()));
}

template <typename... Args>
void oprand_type_check(std::string op, Args... args) {
    if (!(... || args)) {
        throw std::runtime_error("invalid operands to " + op);
    }
}

bool is_zero_constant(ExpressionNode *node) {
    if (auto *literal = dynamic_cast<LiteralNode *>(node)) {
        return literal->get_int_value() == 0;
    }
    return false;
}

bool is_variable(ExpressionNode *node) {
    return dynamic_cast<VariableNode *>(node) != nullptr;
}
} // namespace

namespace myComp {
std::optional<int>
CodeBlockNode::generate_code(CodeGenerator *code_generator) const {
    for (auto &statement : statements_) {
        statement->generate_code(code_generator);
        code_generator->free_all_registers();
    }
    return std::nullopt;
}

void CodeBlockNode::print(std::ostream &os, int indent) const {
    os << std::string(indent, ' ') << "CodeBlock:\n";
    for (auto &statement : statements_) {
        statement->print(os, indent + 2);
    }
}

std::optional<int>
FunctionDefinitionNode::generate_code(CodeGenerator *code_generator) const {
    std::string function_name = prototype_->name_;
    code_generator->function_prelude(function_name);
    std::vector<Variable *> params = prototype_->parameters_;
    std::vector<Variable *> variables =
        VariableManager::get_variables_in_scope(function_name);
    // Remove parameters from variables
    for (auto &param : params) {
        variables.erase(std::remove(variables.begin(), variables.end(), param),
                        variables.end());
    }
    code_generator->load_parameters(params);
    code_generator->allocate_local_variables(variables);
    code_block_->generate_code(code_generator);
    code_generator->function_postlude();
    return std::nullopt;
}

void FunctionDefinitionNode::print(std::ostream &os, int indent) const {
    os << std::string(indent, ' ')
       << "FunctionDefinition: " << prototype_->str() << "\n";
    code_block_->print(os, indent);
}

std::optional<int>
VariableDeclarationNode::generate_code(CodeGenerator *code_generator) const {
    // We do nothing here since initialization on declaration is not supported
    // yet
    return std::nullopt;
}

void VariableDeclarationNode::print(std::ostream &os, int indent) const {
    // os << std::string(indent, ' ')
    //           << "VariableDeclaration: " << variable_->str() << "\n";
    // if (initializer_ != nullptr) {
    //     initializer_->print(os,indent + 2);
    // }

    os << std::string(indent, ' ') << "VariableDeclaration\n";
}

std::optional<int> IfNode::generate_code(CodeGenerator *code_generator) const {
    std::string if_label = code_generator->allocate_label();
    int reg = condition_->generate_code(code_generator).value();
    code_generator->jump_on_zero(reg, if_label);
    if_block_->generate_code(code_generator);
    if (else_block_ == nullptr) {
        code_generator->add_label(if_label);
    } else {
        std::string else_label = code_generator->allocate_label();
        code_generator->jump(else_label);
        code_generator->add_label(if_label);
        else_block_->generate_code(code_generator);
        code_generator->add_label(else_label);
    }
    return std::nullopt;
}

void IfNode::print(std::ostream &os, int indent) const {
    os << std::string(indent, ' ') << "If:\n";
    condition_->print(os, indent + 2);
    if_block_->print(os, indent + 2);
    if (else_block_ != nullptr) {
        os << std::string(indent, ' ') << "Else:\n";
        else_block_->print(os, indent + 2);
    }
}

std::optional<int>
WhileNode::generate_code(CodeGenerator *code_generator) const {
    std::string start_label = code_generator->allocate_label();
    std::string end_label = code_generator->allocate_label();
    code_generator->add_label(start_label);
    int reg = condition_->generate_code(code_generator).value();
    code_generator->jump_on_zero(reg, end_label);
    code_block_->generate_code(code_generator);
    code_generator->jump(start_label);
    code_generator->add_label(end_label);
    return std::nullopt;
}

void WhileNode::print(std::ostream &os, int indent) const {
    os << std::string(indent, ' ') << "While:\n";
    condition_->print(os, indent + 2);
    code_block_->print(os, indent + 2);
}

std::optional<int> ForNode::generate_code(CodeGenerator *code_generator) const {
    std::string start_label = code_generator->allocate_label();
    std::string end_label = code_generator->allocate_label();
    initializer_->generate_code(code_generator);
    code_generator->add_label(start_label);
    int reg = condition_->generate_code(code_generator).value();
    code_generator->jump_on_zero(reg, end_label);
    code_block_->generate_code(code_generator);
    increment_->generate_code(code_generator);
    code_generator->jump(start_label);
    code_generator->add_label(end_label);
    return std::nullopt;
}

void ForNode::print(std::ostream &os, int indent) const {
    os << std::string(indent, ' ') << "For:\n";
    initializer_->print(os, indent + 2);
    condition_->print(os, indent + 2);
    increment_->print(os, indent + 2);
    code_block_->print(os, indent + 2);
}

std::optional<int>
ReturnNode::generate_code(CodeGenerator *code_generator) const {
    int reg = expression_->generate_code(code_generator).value();
    code_generator->return_from_function(reg);
    return std::nullopt;
}

void ReturnNode::print(std::ostream &os, int indent) const {
    os << std::string(indent, ' ') << "Return:\n";
    expression_->print(os, indent + 2);
}

void BinaryExpressionNode::print(std::ostream &os, int indent) const {
    os << std::string(indent, ' ') << "BinaryExpression: " << op_ << "\n";
    left_->print(os, indent + 2);
    right_->print(os, indent + 2);
}

void UnaryExpressionNode::print(std::ostream &os, int indent) const {
    os << std::string(indent, ' ') << "UnaryExpression: " << op_ << "\n";
    operand_->print(os, indent + 2);
}

AddNode::AddNode(ExpressionNode *left, ExpressionNode *right)
    : BinaryExpressionNode("+", left, right) {
    _arithmetic_flag =
        left->type()->is_arithmetic() && right->type()->is_arithmetic();
    _left_pointer_flag =
        left->type()->is_pointer() && right->type()->is_integer();
    _right_pointer_flag =
        left->type()->is_integer() && right->type()->is_pointer();

    oprand_type_check("binary +", _arithmetic_flag, _left_pointer_flag,
                      _right_pointer_flag);

    if (_arithmetic_flag) {
        set_type(usual_arithmetic_conversion(left->type(), right->type()));
    } else if (_left_pointer_flag) {
        set_type(left->type());
    } else if (_right_pointer_flag) {
        swap_operands();
        set_type(left->type());
    }

    unset_lvalue();
}

std::optional<int> AddNode::generate_code(CodeGenerator *code_generator) const {
    if (_arithmetic_flag)
        return do_arithmetic(this, code_generator, &CodeGenerator::add);
    int left_reg = get_left()->generate_code(code_generator).value();
    int right_reg = get_right()->generate_code(code_generator).value();
    PointerType *pointer_type = dynamic_cast<PointerType *>(get_left()->type());
    int pointee_size = pointer_type->pointee()->size();
    code_generator->type_cast(right_reg, get_right()->type(),
                              get_left()->type());
    if (pointee_size != 1)
        code_generator->immediate_multiply(right_reg, pointee_size);
    return code_generator->add(left_reg, right_reg, type());
}

SubtractNode::SubtractNode(ExpressionNode *left, ExpressionNode *right)
    : BinaryExpressionNode("-", left, right) {
    _arithmetic_flag =
        left->type()->is_arithmetic() && right->type()->is_arithmetic();
    _pointer_flag = left->type()->is_pointer() && right->type()->is_pointer() &&
                    is_compatible(left->type(), right->type());
    _pointer_integer_flag =
        left->type()->is_pointer() && right->type()->is_integer();

    oprand_type_check("binary -", _arithmetic_flag, _pointer_flag,
                      _pointer_integer_flag);

    if (_arithmetic_flag) {
        set_type(usual_arithmetic_conversion(left->type(), right->type()));
    } else if (_pointer_flag) {
        set_type(TypeFactory::get_signed(8));
    } else if (_pointer_integer_flag) {
        set_type(left->type());
    }

    unset_lvalue();
}

std::optional<int>
SubtractNode::generate_code(CodeGenerator *code_generator) const {
    if (_arithmetic_flag)
        return do_arithmetic(this, code_generator, &CodeGenerator::subtract);
    if (_pointer_flag) {
        int left_reg = get_left()->generate_code(code_generator).value();
        int right_reg = get_right()->generate_code(code_generator).value();
        PointerType *pointer_type =
            dynamic_cast<PointerType *>(get_left()->type());
        int pointee_size = pointer_type->pointee()->size();
        int reg = code_generator->subtract(left_reg, right_reg, type());
        code_generator->immediate_divide(reg, pointee_size);
        return reg;
    }
    int left_reg = get_left()->generate_code(code_generator).value();
    int right_reg = get_right()->generate_code(code_generator).value();
    PointerType *pointer_type = dynamic_cast<PointerType *>(get_left()->type());
    int pointee_size = pointer_type->pointee()->size();
    code_generator->type_cast(right_reg, get_right()->type(),
                              get_left()->type());
    if (pointee_size != 1)
        code_generator->immediate_multiply(right_reg, pointee_size);
    return code_generator->subtract(left_reg, right_reg, type());
}

MultiplyNode::MultiplyNode(ExpressionNode *left, ExpressionNode *right)
    : BinaryExpressionNode("*", left, right) {
    oprand_type_check("binary *", left->type()->is_arithmetic() &&
                                      right->type()->is_arithmetic());

    set_type(usual_arithmetic_conversion(left->type(), right->type()));

    unset_lvalue();
}

std::optional<int>
MultiplyNode::generate_code(CodeGenerator *code_generator) const {
    return do_arithmetic(this, code_generator, &CodeGenerator::multiply);
}

DivideNode::DivideNode(ExpressionNode *left, ExpressionNode *right)
    : BinaryExpressionNode("/", left, right) {
    oprand_type_check("binary /", left->type()->is_arithmetic() &&
                                      right->type()->is_arithmetic());

    set_type(usual_arithmetic_conversion(left->type(), right->type()));

    unset_lvalue();
}

std::optional<int>
DivideNode::generate_code(CodeGenerator *code_generator) const {
    return do_arithmetic(this, code_generator, &CodeGenerator::divide);
}

ModuloNode::ModuloNode(ExpressionNode *left, ExpressionNode *right)
    : BinaryExpressionNode("%", left, right) {
    oprand_type_check("binary %", left->type()->is_arithmetic() &&
                                      right->type()->is_arithmetic());

    set_type(usual_arithmetic_conversion(left->type(), right->type()));

    unset_lvalue();
}

std::optional<int>
ModuloNode::generate_code(CodeGenerator *code_generator) const {
    return do_arithmetic(this, code_generator, &CodeGenerator::modulo);
}

InvertNode::InvertNode(ExpressionNode *operand)
    : UnaryExpressionNode("~", operand) {
    oprand_type_check("unary ~", operand->type()->is_integer());

    set_type(integer_promotion(operand->type()));

    unset_lvalue();
}

std::optional<int>
InvertNode::generate_code(CodeGenerator *code_generator) const {
    return do_arithmetic(this, code_generator, &CodeGenerator::bitwise_not);
}

OrNode::OrNode(ExpressionNode *left, ExpressionNode *right)
    : BinaryExpressionNode("|", left, right) {
    oprand_type_check("binary |", left->type()->is_integer() &&
                                      right->type()->is_integer());

    set_type(usual_arithmetic_conversion(left->type(), right->type()));

    unset_lvalue();
}

std::optional<int> OrNode::generate_code(CodeGenerator *code_generator) const {
    return do_arithmetic(this, code_generator, &CodeGenerator::bitwise_or);
}

AndNode::AndNode(ExpressionNode *left, ExpressionNode *right)
    : BinaryExpressionNode("&", left, right) {
    oprand_type_check("binary &", left->type()->is_integer() &&
                                      right->type()->is_integer());

    set_type(usual_arithmetic_conversion(left->type(), right->type()));

    unset_lvalue();
}

std::optional<int> AndNode::generate_code(CodeGenerator *code_generator) const {
    return do_arithmetic(this, code_generator, &CodeGenerator::bitwise_and);
}

XorNode::XorNode(ExpressionNode *left, ExpressionNode *right)
    : BinaryExpressionNode("^", left, right) {
    oprand_type_check("binary ^", left->type()->is_integer() &&
                                      right->type()->is_integer());

    set_type(usual_arithmetic_conversion(left->type(), right->type()));

    unset_lvalue();
}

std::optional<int> XorNode::generate_code(CodeGenerator *code_generator) const {
    return do_arithmetic(this, code_generator, &CodeGenerator::bitwise_xor);
}

LeftShiftNode::LeftShiftNode(ExpressionNode *left, ExpressionNode *right)
    : BinaryExpressionNode("<<", left, right) {
    oprand_type_check("binary <<", left->type()->is_integer() &&
                                       right->type()->is_integer());

    set_type(integer_promotion(left->type()));

    unset_lvalue();
}

std::optional<int>
LeftShiftNode::generate_code(CodeGenerator *code_generator) const {
    int left_reg = get_left()->generate_code(code_generator).value();
    int right_reg = get_right()->generate_code(code_generator).value();
    code_generator->type_cast(right_reg, get_right()->type(),
                              integer_promotion(get_right()->type()));
    return code_generator->left_shift(left_reg, right_reg, type());
}

RightShiftNode::RightShiftNode(ExpressionNode *left, ExpressionNode *right)
    : BinaryExpressionNode(">>", left, right) {
    oprand_type_check("binary >>", left->type()->is_integer() &&
                                       right->type()->is_integer());

    set_type(integer_promotion(left->type()));

    unset_lvalue();
}

std::optional<int>
RightShiftNode::generate_code(CodeGenerator *code_generator) const {
    int left_reg = get_left()->generate_code(code_generator).value();
    int right_reg = get_right()->generate_code(code_generator).value();
    code_generator->type_cast(right_reg, get_right()->type(),
                              integer_promotion(get_right()->type()));
    return code_generator->right_shift(left_reg, right_reg, type());
}

LessNode::LessNode(ExpressionNode *left, ExpressionNode *right)
    : BinaryExpressionNode("<", left, right) {
    _arithmetic_flag =
        left->type()->is_arithmetic() && right->type()->is_arithmetic();
    _pointer_flag = left->type()->is_pointer() && right->type()->is_pointer() &&
                    is_compatible(left->type(), right->type());

    oprand_type_check("binary <", _arithmetic_flag, _pointer_flag);

    set_type(TypeFactory::get_signed(4));

    unset_lvalue();
}

std::optional<int>
LessNode::generate_code(CodeGenerator *code_generator) const {
    if (_arithmetic_flag) {
        return arithmetic_compare(this, code_generator,
                                  &CodeGenerator::compare_less);
    } else {
        return pointer_compare(this, code_generator,
                               &CodeGenerator::compare_less);
    }
}

LessEqualsNode::LessEqualsNode(ExpressionNode *left, ExpressionNode *right)
    : BinaryExpressionNode("<=", left, right) {
    _arithmetic_flag =
        left->type()->is_arithmetic() && right->type()->is_arithmetic();
    _pointer_flag = left->type()->is_pointer() && right->type()->is_pointer() &&
                    is_compatible(left->type(), right->type());

    oprand_type_check("binary <=", _arithmetic_flag, _pointer_flag);

    set_type(TypeFactory::get_signed(4));

    unset_lvalue();
}

std::optional<int>
LessEqualsNode::generate_code(CodeGenerator *code_generator) const {
    if (_arithmetic_flag) {
        return arithmetic_compare(this, code_generator,
                                  &CodeGenerator::compare_less_equal);
    } else {
        return pointer_compare(this, code_generator,
                               &CodeGenerator::compare_less_equal);
    }
}

GreaterNode::GreaterNode(ExpressionNode *left, ExpressionNode *right)
    : BinaryExpressionNode(">", left, right) {
    _arithmetic_flag =
        left->type()->is_arithmetic() && right->type()->is_arithmetic();
    _pointer_flag = left->type()->is_pointer() && right->type()->is_pointer() &&
                    is_compatible(left->type(), right->type());

    oprand_type_check("binary >", _arithmetic_flag, _pointer_flag);

    set_type(TypeFactory::get_signed(4));

    unset_lvalue();
}

std::optional<int>
GreaterNode::generate_code(CodeGenerator *code_generator) const {
    if (_arithmetic_flag) {
        return arithmetic_compare(this, code_generator,
                                  &CodeGenerator::compare_greater);
    } else {
        return pointer_compare(this, code_generator,
                               &CodeGenerator::compare_greater);
    }
}

GreaterEqualsNode::GreaterEqualsNode(ExpressionNode *left,
                                     ExpressionNode *right)
    : BinaryExpressionNode(">=", left, right) {
    _arithmetic_flag =
        left->type()->is_arithmetic() && right->type()->is_arithmetic();
    _pointer_flag = left->type()->is_pointer() && right->type()->is_pointer() &&
                    is_compatible(left->type(), right->type());

    oprand_type_check("binary >=", _arithmetic_flag, _pointer_flag);

    set_type(TypeFactory::get_signed(4));

    unset_lvalue();
}

std::optional<int>
GreaterEqualsNode::generate_code(CodeGenerator *code_generator) const {
    if (_arithmetic_flag) {
        return arithmetic_compare(this, code_generator,
                                  &CodeGenerator::compare_greater_equal);
    } else {
        return pointer_compare(this, code_generator,
                               &CodeGenerator::compare_greater_equal);
    }
}
EqualsNode::EqualsNode(ExpressionNode *left, ExpressionNode *right)
    : BinaryExpressionNode("==", left, right) {
    _arithmetic_flag =
        left->type()->is_arithmetic() && right->type()->is_arithmetic();
    _pointer_flag = left->type()->is_pointer() && right->type()->is_pointer() &&
                    is_compatible(left->type(), right->type());
    if (left->type()->is_pointer() && is_zero_constant(right)) {
        _pointer_NULL_flag = true;
    } else if (right->type()->is_pointer() && is_zero_constant(left)) {
        swap_operands();
        _pointer_NULL_flag = true;
    }

    oprand_type_check("binary ==", _arithmetic_flag, _pointer_flag,
                      _pointer_NULL_flag);

    set_type(TypeFactory::get_signed(4));

    unset_lvalue();
}

std::optional<int>
EqualsNode::generate_code(CodeGenerator *code_generator) const {
    if (_arithmetic_flag) {
        return arithmetic_compare(this, code_generator,
                                  &CodeGenerator::compare_equal);
    } else if (_pointer_flag || _pointer_NULL_flag) {
        return pointer_compare(this, code_generator,
                               &CodeGenerator::compare_equal);
    }

    // This should never happen
    throw std::runtime_error("Invalid operands to binary ==");
}

NotEqualsNode::NotEqualsNode(ExpressionNode *left, ExpressionNode *right)
    : BinaryExpressionNode("!=", left, right) {
    _arithmetic_flag =
        left->type()->is_arithmetic() && right->type()->is_arithmetic();
    _pointer_flag = left->type()->is_pointer() && right->type()->is_pointer() &&
                    is_compatible(left->type(), right->type());
    if (left->type()->is_pointer() && is_zero_constant(right)) {
        _pointer_NULL_flag = true;
    } else if (right->type()->is_pointer() && is_zero_constant(left)) {
        swap_operands();
        _pointer_NULL_flag = true;
    }

    oprand_type_check("binary !=", _arithmetic_flag, _pointer_flag,
                      _pointer_NULL_flag);

    set_type(TypeFactory::get_signed(4));

    unset_lvalue();
}

std::optional<int>
NotEqualsNode::generate_code(CodeGenerator *code_generator) const {
    if (_arithmetic_flag) {
        return arithmetic_compare(this, code_generator,
                                  &CodeGenerator::compare_not_equal);
    } else if (_pointer_flag || _pointer_NULL_flag) {
        return pointer_compare(this, code_generator,
                               &CodeGenerator::compare_not_equal);
    }

    // This should never happen
    throw std::runtime_error("Invalid operands to binary !=");
}

NotNode::NotNode(ExpressionNode *oprand) : UnaryExpressionNode("!", oprand) {
    oprand_type_check("unary !", oprand->type()->is_scalar());

    set_type(TypeFactory::get_signed(4));

    unset_lvalue();
}

std::optional<int> NotNode::generate_code(CodeGenerator *code_generator) const {
    int oprand_reg = get_operand()->generate_code(code_generator).value();
    return code_generator->logical_not(oprand_reg, get_operand()->type());
}

LogicalOrNode::LogicalOrNode(ExpressionNode *left, ExpressionNode *right)
    : BinaryExpressionNode("||", left, right) {
    oprand_type_check("binary ||",
                      left->type()->is_scalar() && right->type()->is_scalar());

    set_type(TypeFactory::get_signed(4));

    unset_lvalue();
}

std::optional<int>
LogicalOrNode::generate_code(CodeGenerator *code_generator) const {
    std::string true_label = code_generator->allocate_label();
    std::string false_label = code_generator->allocate_label();
    std::string end_label = code_generator->allocate_label();
    int left_reg = get_left()->generate_code(code_generator).value();
    code_generator->jump_on_non_zero(left_reg, true_label);
    int right_reg = get_right()->generate_code(code_generator).value();
    code_generator->jump_on_zero(right_reg, false_label);
    code_generator->add_label(true_label);
    int reg = code_generator->load_immediate(1);
    code_generator->jump(end_label);
    code_generator->add_label(false_label);
    code_generator->move_immediate(reg, 0);
    code_generator->add_label(end_label);
    return reg;
}

LogicalAndNode::LogicalAndNode(ExpressionNode *left, ExpressionNode *right)
    : BinaryExpressionNode("&&", left, right) {
    oprand_type_check("binary &&",
                      left->type()->is_scalar() && right->type()->is_scalar());

    set_type(TypeFactory::get_signed(4));

    unset_lvalue();
}

std::optional<int>
LogicalAndNode::generate_code(CodeGenerator *code_generator) const {
    std::string false_label = code_generator->allocate_label();
    std::string end_label = code_generator->allocate_label();
    int left_reg = get_left()->generate_code(code_generator).value();
    code_generator->jump_on_zero(left_reg, false_label);
    int right_reg = get_right()->generate_code(code_generator).value();
    code_generator->jump_on_zero(right_reg, false_label);
    int reg = code_generator->load_immediate(1);
    code_generator->jump(end_label);
    code_generator->add_label(false_label);
    code_generator->move_immediate(reg, 0);
    code_generator->add_label(end_label);
    return reg;
}

AssignNode::AssignNode(ExpressionNode *left, ExpressionNode *right)
    : BinaryExpressionNode("=", left, right) {
    oprand_type_check("binary =", left->is_lvalue(),
                      convertable_to(right->type(), left->type()));

    set_type(left->type());

    unset_lvalue();
}

std::optional<int>
AssignNode::generate_code(CodeGenerator *code_generator) const {
    int right_reg = get_right()->generate_code(code_generator).value();
    int ret_reg = code_generator->duplicate_register(right_reg);
    if (auto *deref_node = dynamic_cast<DereferenceNode *>(get_left());
        deref_node != nullptr) {
        int left_reg =
            deref_node->get_operand()->generate_code(code_generator).value();
        code_generator->move_register(right_reg, left_reg, deref_node->type());
    } else {
        auto *var_node = dynamic_cast<VariableNode *>(get_left());
        code_generator->move_register(right_reg, var_node->get_variable());
    }
    return ret_reg;
}

AddressNode::AddressNode(ExpressionNode *operand)
    : UnaryExpressionNode("&", operand) {
    oprand_type_check("unary &", operand->is_lvalue());

    set_type(TypeFactory::get_pointer(operand->type()));

    unset_lvalue();
}

std::optional<int>
AddressNode::generate_code(CodeGenerator *code_generator) const {
    if (dynamic_cast<VariableNode *>(get_operand()) != nullptr) {
        auto *var = dynamic_cast<VariableNode *>(get_operand());
        return code_generator->load_variable_address(var->get_variable());
    } else {
        auto *deref = dynamic_cast<DereferenceNode *>(get_operand());
        return deref->get_operand()->generate_code(code_generator);
    }
}

DereferenceNode::DereferenceNode(ExpressionNode *operand)
    : UnaryExpressionNode("*", operand) {
    oprand_type_check("unary *", operand->type()->is_pointer());

    set_type(dynamic_cast<PointerType *>(get_operand()->type())->pointee());

    set_lvalue();
}

std::optional<int>
DereferenceNode::generate_code(CodeGenerator *code_generator) const {
    int reg = get_operand()->generate_code(code_generator).value();
    return code_generator->load_from_memory(reg, type());
}

PostIncrementNode::PostIncrementNode(ExpressionNode *operand)
    : UnaryExpressionNode("++", operand) {
    oprand_type_check("unary ++", is_variable(operand));

    set_type(operand->type());

    unset_lvalue();
}

std::optional<int>
PostIncrementNode::generate_code(CodeGenerator *code_generator) const {
    return code_generator->post_increment(
        dynamic_cast<VariableNode *>(get_operand())->get_variable());
}

PostDecrementNode::PostDecrementNode(ExpressionNode *operand)
    : UnaryExpressionNode("--", operand) {
    oprand_type_check("unary --", is_variable(operand));

    set_type(operand->type());

    unset_lvalue();
}

std::optional<int>
PostDecrementNode::generate_code(CodeGenerator *code_generator) const {
    return code_generator->post_decrement(
        dynamic_cast<VariableNode *>(get_operand())->get_variable());
}

PreIncrementNode::PreIncrementNode(ExpressionNode *operand)
    : UnaryExpressionNode("++", operand) {
    oprand_type_check("unary ++", is_variable(operand));

    set_type(operand->type());

    unset_lvalue();
}

std::optional<int>
PreIncrementNode::generate_code(CodeGenerator *code_generator) const {
    return code_generator->pre_increment(
        dynamic_cast<VariableNode *>(get_operand())->get_variable());
}

PreDecrementNode::PreDecrementNode(ExpressionNode *operand)
    : UnaryExpressionNode("--", operand) {
    oprand_type_check("unary --", is_variable(operand));

    set_type(operand->type());

    unset_lvalue();
}

std::optional<int>
PreDecrementNode::generate_code(CodeGenerator *code_generator) const {
    return code_generator->pre_decrement(
        dynamic_cast<VariableNode *>(get_operand())->get_variable());
}

NegativeNode::NegativeNode(ExpressionNode *operand)
    : UnaryExpressionNode("-", operand) {
    oprand_type_check("unary -", operand->type()->is_arithmetic());

    set_type(integer_promotion(operand->type()));

    unset_lvalue();
}

std::optional<int>
NegativeNode::generate_code(CodeGenerator *code_generator) const {
    return do_arithmetic(this, code_generator, &CodeGenerator::negate);
}

PositiveNode::PositiveNode(ExpressionNode *operand)
    : UnaryExpressionNode("+", operand) {
    oprand_type_check("unary +", operand->type()->is_arithmetic());

    set_type(integer_promotion(operand->type()));

    unset_lvalue();
}

std::optional<int>
PositiveNode::generate_code(CodeGenerator *code_generator) const {
    int reg = get_operand()->generate_code(code_generator).value();
    code_generator->type_cast(reg, get_operand()->type(), type());
    return reg;
}

VariableNode::VariableNode(Variable *variable) : variable_(variable) {
    // Convert array to pointer
    if (variable->type->is_array()) {
        set_type(TypeFactory::array_to_pointer(variable->type));
    } else {
        set_type(variable->type);
    }
    set_lvalue();
}

std::optional<int>
VariableNode::generate_code(CodeGenerator *code_generator) const {
    return code_generator->load_variable(variable_);
}

void VariableNode::print(std::ostream &os, int indent) const {
    os << std::string(indent, ' ') << "Variable: " << variable_->str() << "\n";
}

LiteralNode::LiteralNode(Type *type, long long int_value)
    : string_value_(""), int_value_(int_value), is_string_(false) {
    set_type(type);
    unset_lvalue();
}

LiteralNode::LiteralNode(Type *type, const std::string &string_value)
    : string_value_(string_value), int_value_(0), is_string_(true) {
    set_type(type);
    unset_lvalue();
}

std::optional<int>
LiteralNode::generate_code(CodeGenerator *code_generator) const {
    if (is_string_) {
        return code_generator->load_string_literal(string_value_);
    } else {
        return code_generator->load_immediate(int_value_);
    }
}

void LiteralNode::print(std::ostream &os, int indent) const {
    os << std::string(indent, ' ') << "Literal: ";
    if (is_string_) {
        os << string_value_ << "\n";
    } else {
        os << int_value_ << "\n";
    }
}

long long LiteralNode::get_int_value() const {
    if (is_string_) {
        throw std::runtime_error("string literal has no integer value");
    }
    return int_value_;
}

std::string LiteralNode::get_string_value() const {
    if (!is_string_) {
        throw std::runtime_error("integer literal has no string value");
    }
    return string_value_;
}

FunctionCallNode::FunctionCallNode(const std::string &name,
                                   std::vector<ExpressionNode *> arguments)
    : name_(name), arguments_(arguments) {
    Type *return_type = FunctionManager::find(name)->return_type_;
    set_type(return_type);
    unset_lvalue();
}

std::optional<int>
FunctionCallNode::generate_code(CodeGenerator *code_generator) const {
    FunctionPrototype *prototype = FunctionManager::find(name_);

    for (int i = arguments_.size(); i >= 1; i--) {
        // Calculate the i'th argument
        int reg = arguments_[i - 1]->generate_code(code_generator).value();

        // Type cast the i'th argument
        // If exceeds the number of parameters, do nothing
        if (i <= prototype->parameters_.size()) {
            Type *arg_type = arguments_[i - 1]->type();
            Type *param_type = prototype->parameters_[i - 1]->type;
            code_generator->type_cast(reg, arg_type, param_type);
        }

        code_generator->move_to_argument(reg, i);
    }
    return code_generator->call_function(name_);
}

void FunctionCallNode::print(std::ostream &os, int indent) const {
    os << std::string(indent, ' ') << "FunctionCall: " << name_ << "\n";
    for (auto &arg : arguments_) {
        arg->print(os, indent + 2);
    }
}

} // namespace myComp