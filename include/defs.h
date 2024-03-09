#ifndef MYCOMP_DEFS_H
#define MYCOMP_DEFS_H

// This is the header file for the definitions

#include <algorithm>
#include <cassert>
#include <deque>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "Type.h"
#include "Token.h"
#include "ASTNode.h"

namespace myComp {
// AST node types
enum class ASTNodeType {
    // Operators
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    MODULO,
    EQUALS,
    NEQ,
    LESS,
    GREATER,
    LESS_EQ,
    GREATER_EQ,
    ADDRESS,
    DEREFERENCE,
    OR,
    LOGICAL_OR,
    AND,
    LOGICAL_AND,
    XOR,
    L_SHIFT,
    R_SHIFT,
    INVERT,
    NOT,
    POST_INC,
    POST_DEC,
    PRE_INC,
    PRE_DEC,
    NEGATIVE,
    POSITIVE,
    // Statements
    ASSIGN,
    VARIABLE_DECLARATION,
    // Control flow
    COMPOUND,
    IF,
    ELSE,
    WHILE,
    FOR,
    GLUE,
    RETURN,
    // Functions
    FUNCTION_DECLARATION,
    VARIABLE,
    WIDEN,
    INT_LITERAL,
    FUNCTION_CALL,
    SCALE,
    STRING_LITERAL,
};

// Context types
enum class ContextType {
    GLOBAL,
    FUNCTION,
};

// AST struct
struct ASTNode {
    // AST node type, deciding the operation
    ASTNodeType type;

    // contains the value of the integer literal or the identifier
    long long val = 0;
    std::string string = {};

    // Left and right children
    ASTNode *left = nullptr;
    ASTNode *right = nullptr;

    // DataType: only valid for expression nodes
    Type *data_type = nullptr;

    // Destructor
    ~ASTNode() {
        delete left;
        delete right;
    }
};

// Function prototype parameter struct
struct parameter {
    Type *type;
    std::string name;
};

// Function prototype struct
struct FunctionPrototype {
    Type *return_type;
    std::vector<parameter> parameters;
};

// Context node struct
struct ContextNode {
    ContextType type;
    std::string name;
    std::string end_label;
    bool has_return = false;
};

// Symbol struct
struct Symbol {
    // DataType data_type = DataType::NONE;
    Type *type = nullptr;
    int offset = 0;
};
} // namespace myComp

#endif // MYCOMP_DEFS_H
