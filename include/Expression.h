#ifndef MYCOMP_EXPRESSION_H
#define MYCOMP_EXPRESSION_H

#include "defs.h"

namespace myComp {
    class Expression {
    private:
        // Primary expression
        static ASTNode *primary();

        // Prefix expression
        static ASTNode *prefix();

        // Build tree for parameters
        static ASTNode *parameters(const std::string &name);

        // deal with identifier
        static ASTNode *identifier();

        // deal with postfix operators
        static ASTNode *postfix(const std::string &identifier);

    public:
        // Build the AST
        static ASTNode *build_tree(int pre_precedence);
    };
} // myComp

#endif //MYCOMP_EXPRESSION_H
