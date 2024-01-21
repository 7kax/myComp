#ifndef MYCOMP_TREE_H
#define MYCOMP_TREE_H

#include "defs.h"

namespace myComp {
    class Tree {
    public:
        static ASTNode *int_literal(int value);

        static ASTNode *join(ASTNodeType type, ASTNode *left, ASTNode *right);

        static ASTNode *unary(ASTNodeType type, ASTNode *left);

        static ASTNode *variable_declaration(std::string identifier, DataType data_type, int size = 0);

        static ASTNode *function_declaration(std::string identifier, ASTNode *code_block);

        static ASTNode *function_call(const std::string &identifier, ASTNode *parameters);

        static ASTNode *variable(const std::string &identifier);

        static ASTNode *parameter(ASTNode *_expression);

        static ASTNode *dereference(ASTNode *address);

        static ASTNode *dereference(ASTNode *address, ASTNode *offset);

        static ASTNode *address(ASTNode *left);

        // static void print(std::ostream &out, const ASTNode *node, int indent = 0);

        static ASTNode *string_literal(const std::string &str);

        static ASTNode *negative(ASTNode *node);

        static ASTNode *NOT(ASTNode *node);

        static ASTNode *invert(ASTNode *node);

        static ASTNode *increment(ASTNode *node, bool is_prefix);

        static ASTNode *decrement(ASTNode *node, bool is_prefix);
    };
} // myComp

#endif //MYCOMP_TREE_H
