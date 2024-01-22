#ifndef MYCOMP_EXPRESSION_H
#define MYCOMP_EXPRESSION_H

#include "defs.h"

namespace myComp {
  class Expression {
  private:
    static ASTNode *primary();

    static ASTNode *prefix();

    static ASTNode *identifier();

    static ASTNode *postfix(const std::string &identifier);

    static ASTNode *parameters(const std::string &name);

    // Widen the integer type
    static void integer_widen(ASTNode *&left, ASTNode *&right);

    static void integer_scale(ASTNode *&pointer, ASTNode *&integer);

  public:
    // Build the AST
    static ASTNode *build_tree(int pre_precedence);
  };
} // namespace myComp

#endif // MYCOMP_EXPRESSION_H
