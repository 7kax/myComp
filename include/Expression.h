#ifndef MYCOMP_EXPRESSION_H
#define MYCOMP_EXPRESSION_H

#include "defs.h"

#include "TokenProcessor.h"

namespace myComp {
    class Expression {
    private:
        TokenProcessor *token_processor_ = nullptr;

        ASTNode *primary();

        ASTNode *prefix();

        ASTNode *identifier();

        ASTNode *postfix(const std::string &identifier);

        ASTNode *parameters();

        // Widen the integer type
        static void integer_widen(ASTNode *&left, ASTNode *&right);

        static void integer_scale(ASTNode *&pointer, ASTNode *&integer);

        static bool is_operator(TokenType type);

        static bool is_prefix_operator(TokenType type);

        static bool is_postfix_operator(TokenType type);

    public:
        void set_processor(TokenProcessor *token_processor) { this->token_processor_ = token_processor; }

        // Build the AST
        ASTNode *build_tree(int pre_precedence);

        static constexpr int MAX_PRECEDENCE = 20;
    };
} // namespace myComp

#endif // MYCOMP_EXPRESSION_H
