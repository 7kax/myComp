#include "data.h"

namespace myComp {
// Debug flag
bool debug = false;

const std::set<TokenType> data_types = {
    TokenType::INT,
    TokenType::CHAR,
    TokenType::VOID,
    TokenType::LONG,
};

// Convert an AST node type to a string
const std::map<ASTNodeType, const char *> ASTNode_str = {
    {ASTNodeType::ADD, "add"},
    {ASTNodeType::SUBTRACT, "subtract"},
    {ASTNodeType::MULTIPLY, "multiply"},
    {ASTNodeType::DIVIDE, "divide"},
    {ASTNodeType::MODULO, "modulo"},
    {ASTNodeType::EQUALS, "equals"},
    {ASTNodeType::NEQ, "neq"},
    {ASTNodeType::LESS, "less"},
    {ASTNodeType::GREATER, "greater"},
    {ASTNodeType::LESS_EQ, "less_eq"},
    {ASTNodeType::GREATER_EQ, "greater_eq"},
    {ASTNodeType::ADDRESS, "address"},
    {ASTNodeType::DEREFERENCE, "dereference"},
    {ASTNodeType::OR, "or"},
    {ASTNodeType::LOGICAL_OR, "logical_or"},
    {ASTNodeType::AND, "and"},
    {ASTNodeType::LOGICAL_AND, "logical_and"},
    {ASTNodeType::XOR, "xor"},
    {ASTNodeType::L_SHIFT, "l_shift"},
    {ASTNodeType::R_SHIFT, "r_shift"},
    {ASTNodeType::INVERT, "invert"},
    {ASTNodeType::NOT, "not"},
    {ASTNodeType::POST_INC, "post_inc"},
    {ASTNodeType::POST_DEC, "post_dec"},
    {ASTNodeType::PRE_INC, "pre_inc"},
    {ASTNodeType::PRE_DEC, "pre_dec"},
    {ASTNodeType::NEGATIVE, "negative"},
    {ASTNodeType::ASSIGN, "assign"},
    {ASTNodeType::VARIABLE_DECLARATION, "variable_declaration"},
    {ASTNodeType::COMPOUND, "compound"},
    {ASTNodeType::IF, "if"},
    {ASTNodeType::ELSE, "else"},
    {ASTNodeType::WHILE, "while"},
    {ASTNodeType::FOR, "for"},
    {ASTNodeType::GLUE, "glue"},
    {ASTNodeType::RETURN, "return"},
    {ASTNodeType::FUNCTION_DECLARATION, "function_declaration"},
    {ASTNodeType::VARIABLE, "variable"},
    {ASTNodeType::WIDEN, "widen"},
    {ASTNodeType::INT_LITERAL, "int_literal"},
    {ASTNodeType::FUNCTION_CALL, "function_call"},
    {ASTNodeType::SCALE, "scale"},
    {ASTNodeType::STRING_LITERAL, "string_literal"},
};

// String literals
std::map<std::string, std::string> string_literals;

// Assembly code generator
CodeGenerator *code_generator;
} // namespace myComp
