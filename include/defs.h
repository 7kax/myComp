#ifndef MYCOMP_DEFS_H
#define MYCOMP_DEFS_H

// This is the header file for the definitions

#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <memory>
#include <utility>
#include <vector>
#include <algorithm>
#include <stack>
#include <string>
#include <list>
#include <deque>
#include <cassert>

namespace myComp {
    // Token types
    enum class TokenType {
        // EOF
        T_EOF,
        // Operators
        PLUS,
        MINUS,
        STAR,
        SLASH,
        EQUALS,
        NEQ,
        LESS,
        GREATER,
        LESS_EQ,
        GREATER_EQ,
        ASSIGN,
        AND,
        LOGICAL_AND,
        OR,
        LOGICAL_OR,
        XOR,
        L_SHIFT,
        R_SHIFT,
        INVERT,
        NOT,
        INC,
        DEC,
        // Punctuation
        SEMI,
        LBRACE,
        RBRACE,
        LPAREN,
        RPAREN,
        COMMA,
        LBRACKET,
        RBRACKET,
        // Keywords: control flow
        WHILE,
        FOR,
        IF,
        ELSE,
        RETURN,
        // Keywords: data types
        INT_LITERAL,
        INT,
        CHAR,
        VOID,
        LONG,
        STRING_LITERAL,
        // Identifier
        IDENTIFIER,
    };

    // AST node types
    enum class ASTNodeType {
        // Operators
        ADD,
        SUBTRACT,
        MULTIPLY,
        DIVIDE,
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

    // Data types
    enum class DataType {
        NONE,
        INT,
        CHAR,
        VOID,
        LONG,
        INT_PTR,
        CHAR_PTR,
        VOID_PTR,
        LONG_PTR
    };

    // Context types
    enum class ContextType {
        GLOBAL,
        FUNCTION,
    };

    // Token struct
    struct Token {
        // Token type
        TokenType type;

        // Can be an integer literal or an identifier
        int val;
        std::string string;
    };

    // AST struct
    struct ASTNode {
        // AST node type, deciding the operation
        ASTNodeType type;

        // contains the value of the integer literal or the identifier
        int val = 0;
        std::string string = {};

        // Left and right children
        ASTNode *left = nullptr;
        ASTNode *right = nullptr;

        // DataType: only valid for expression nodes
        DataType data_type = DataType::NONE;

        // Destructor
        ~ASTNode() {
            delete left;
            delete right;
        }
    };

    // Function prototype parameter struct
    struct parameter {
        DataType type;
        std::string name;
    };

    // Function prototype struct
    struct FunctionPrototype {
        DataType return_type;
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
        DataType data_type = DataType::NONE;
        int size = 0;
        int offset = 0;
    };
}

#endif //MYCOMP_DEFS_H
