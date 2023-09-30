#include "defs.h"
#include "objects.h"
#include "data.h"

namespace myComp {
    // Convert token to data type
    const std::map<TokenType, DataType> token_to_data = {
            {TokenType::INT,  DataType::INT},
            {TokenType::CHAR, DataType::CHAR},
            {TokenType::VOID, DataType::VOID},
            {TokenType::LONG, DataType::LONG},
    };
    // Precedence of operators
    const std::map<ASTNodeType, int> precedence = {
            {ASTNodeType::MULTIPLY,    3},
            {ASTNodeType::DIVIDE,      3},
            {ASTNodeType::ADD,         4},
            {ASTNodeType::SUBTRACT,    4},
            {ASTNodeType::L_SHIFT,     5},
            {ASTNodeType::R_SHIFT,     5},
            {ASTNodeType::LESS,        6},
            {ASTNodeType::GREATER,     6},
            {ASTNodeType::LESS_EQ,     6},
            {ASTNodeType::GREATER_EQ,  6},
            {ASTNodeType::EQUALS,      7},
            {ASTNodeType::NEQ,         7},
            {ASTNodeType::AND,         8},
            {ASTNodeType::XOR,         9},
            {ASTNodeType::OR,          10},
            {ASTNodeType::LOGICAL_AND, 11},
            {ASTNodeType::LOGICAL_OR,  12},
            {ASTNodeType::ASSIGN,      14},
    };
    // Convert a token type to an AST node type
    const std::map<TokenType, ASTNodeType> token_to_op = {
            {TokenType::PLUS,        ASTNodeType::ADD},
            {TokenType::MINUS,       ASTNodeType::SUBTRACT},
            {TokenType::STAR,        ASTNodeType::MULTIPLY},
            {TokenType::SLASH,       ASTNodeType::DIVIDE},
            {TokenType::EQUALS,      ASTNodeType::EQUALS},
            {TokenType::NEQ,         ASTNodeType::NEQ},
            {TokenType::LESS,        ASTNodeType::LESS},
            {TokenType::GREATER,     ASTNodeType::GREATER},
            {TokenType::LESS_EQ,     ASTNodeType::LESS_EQ},
            {TokenType::GREATER_EQ,  ASTNodeType::GREATER_EQ},
            {TokenType::ASSIGN,      ASTNodeType::ASSIGN},
            {TokenType::AND,         ASTNodeType::AND},
            {TokenType::LOGICAL_AND, ASTNodeType::LOGICAL_AND},
            {TokenType::OR,          ASTNodeType::OR},
            {TokenType::LOGICAL_OR,  ASTNodeType::LOGICAL_OR},
            {TokenType::XOR,         ASTNodeType::XOR},
            {TokenType::L_SHIFT,     ASTNodeType::L_SHIFT},
            {TokenType::R_SHIFT,     ASTNodeType::R_SHIFT},
    };
    // Convert a data type to a pointer data type
    const std::map<DataType, DataType> data_to_ptr = {
            {DataType::INT,  DataType::INT_PTR},
            {DataType::CHAR, DataType::CHAR_PTR},
            {DataType::VOID, DataType::VOID_PTR},
            {DataType::LONG, DataType::LONG_PTR},
    };
    // Convert a pointer data type to a data type
    const std::map<DataType, DataType> ptr_to_data = {
            {DataType::INT_PTR,  DataType::INT},
            {DataType::CHAR_PTR, DataType::CHAR},
            {DataType::VOID_PTR, DataType::VOID},
            {DataType::LONG_PTR, DataType::LONG},
    };

    // Convert an AST node type to a string
    const std::map<ASTNodeType, const char *> ASTNode_str = {
            {ASTNodeType::ADD,                  "add"},
            {ASTNodeType::SUBTRACT,             "sub"},
            {ASTNodeType::MULTIPLY,             "mul"},
            {ASTNodeType::DIVIDE,               "div"},
            {ASTNodeType::LESS,                 "less"},
            {ASTNodeType::GREATER,              "greater"},
            {ASTNodeType::LESS_EQ,              "less_eq"},
            {ASTNodeType::GREATER_EQ,           "greater_eq"},
            {ASTNodeType::EQUALS,               "equals"},
            {ASTNodeType::NEQ,                  "neq"},
            {ASTNodeType::ASSIGN,               "assign"},
            {ASTNodeType::VARIABLE_DECLARATION, "variable_declaration"},
            {ASTNodeType::INT_LITERAL,          "int_literal"},
            {ASTNodeType::FUNCTION_DECLARATION, "function_declaration"},
            {ASTNodeType::FUNCTION_CALL,        "function_call"},
            {ASTNodeType::VARIABLE,             "variable"},
            {ASTNodeType::GLUE,                 "glue"},
            {ASTNodeType::ADDRESS,              "address"},
            {ASTNodeType::DEREFERENCE,          "dereference"},
            {ASTNodeType::WIDEN,                "widen"},
            {ASTNodeType::SCALE,                "scale"},
            {ASTNodeType::RETURN,               "return"},
            {ASTNodeType::COMPOUND,             "compound"},
            {ASTNodeType::IF,                   "if"},
            {ASTNodeType::ELSE,                 "else"},
            {ASTNodeType::WHILE,                "while"},
            {ASTNodeType::FOR,                  "for"},
            {ASTNodeType::WIDEN,                "widen"},
            {ASTNodeType::STRING_LITERAL,       "string_literal"},
    };

    // Convert a token type to a string
    const std::map<TokenType, const char *> token_str = {
            {TokenType::INT,            "int"},
            {TokenType::CHAR,           "char"},
            {TokenType::VOID,           "void"},
            {TokenType::LONG,           "long"},
            {TokenType::INT_LITERAL,    "int_literal"},
            {TokenType::STRING_LITERAL, "string_literal"},
            {TokenType::IDENTIFIER,     "identifier"},
            {TokenType::PLUS,           "plus"},
            {TokenType::MINUS,          "minus"},
            {TokenType::STAR,           "star"},
            {TokenType::SLASH,          "slash"},
            {TokenType::EQUALS,         "equals"},
            {TokenType::NEQ,            "neq"},
            {TokenType::LESS,           "less"},
            {TokenType::GREATER,        "greater"},
            {TokenType::LESS_EQ,        "less_eq"},
            {TokenType::GREATER_EQ,     "greater_eq"},
            {TokenType::ASSIGN,         "assign"},
            {TokenType::AND,            "and"},
            {TokenType::LOGICAL_AND,    "logical_and"},
            {TokenType::OR,             "or"},
            {TokenType::LOGICAL_OR,     "logical_or"},
            {TokenType::XOR,            "xor"},
            {TokenType::L_SHIFT,        "l_shift"},
            {TokenType::R_SHIFT,        "r_shift"},
            {TokenType::LPAREN,         "lparen"},
            {TokenType::RPAREN,         "rparen"},
            {TokenType::LBRACE,         "lbrace"},
            {TokenType::RBRACE,         "rbrace"},
            {TokenType::SEMI,           "semi"},
            {TokenType::COMMA,          "comma"},
            {TokenType::IF,             "if"},
            {TokenType::ELSE,           "else"},
            {TokenType::WHILE,          "while"},
            {TokenType::FOR,            "for"},
            {TokenType::RETURN,         "return"},
    };

    // String literals
    std::map<std::string, std::string> string_literals;

    // Scanner
    Scanner scanner;

    // Generate code
    Generator generator;

    // Assembly code generator
    AsmGenerator *asm_generator;

    // Symbol table for functions
    std::map<std::string, SymbolTable> symbol_tables;

    // Function
    Function function;

    // Context
    Context context;
} // myComp