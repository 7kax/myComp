#ifndef MYCOMP_DATA_H
#define MYCOMP_DATA_H

#include "defs.h"

#include "Context.h"

namespace myComp {
// Debug flag
extern bool debug;

// Data type in tokens
extern const std::set<TokenType> data_types;

// Precedence of operators
extern const std::map<ASTNodeType, int> precedence;

// Convert a token type to an AST node type
extern const std::map<TokenType, ASTNodeType> token_to_op;

// Convert an AST node type to a string
extern const std::map<ASTNodeType, const char *> ASTNode_str;

// String literals
extern std::map<std::string, std::string> string_literals;

// Symbol tables
// extern std::map<std::string, SymbolTable> symbol_tables;

// Scanner
// extern Scanner scanner;

// Generate code
// extern Generator generator;

// Assembly code generator
extern CodeGenerator *code_generator;

// Function
// extern Function function;

// Context
// extern Context context;
} // namespace myComp

#endif // MYCOMP_DATA_H
