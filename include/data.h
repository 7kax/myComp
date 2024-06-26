#ifndef MYCOMP_DATA_H
#define MYCOMP_DATA_H

#include <map>
#include <set>

#include "ASTNode.h"
#include "Token.h"

namespace myComp {
// Debug flag
extern bool debug;

// Data type in tokens
extern const std::set<TokenType> data_types;

// Convert an AST node type to a string
extern const std::map<ASTNodeType, const char *> ASTNode_str;

// String literals
extern std::map<std::string, std::string> string_literals;

// Assembly code generator
extern CodeGenerator *code_generator;
} // namespace myComp

#endif // MYCOMP_DATA_H
