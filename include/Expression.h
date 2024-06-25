#ifndef MYCOMP_EXPRESSION_H
#define MYCOMP_EXPRESSION_H

#include <unordered_map>

#include "ASTNode.h"
#include "TokenProcessor.h"

namespace myComp {
class Expression {
  private:
    TokenProcessor *token_processor_ = nullptr;

    ExpressionNode *primary();
    ExpressionNode *prefix();
    ExpressionNode *identifier();
    ExpressionNode *postfix(const std::string &identifier);
    std::vector<ExpressionNode *> parse_arguments();

  public:
    void set_processor(TokenProcessor *token_processor) {
        token_processor_ = token_processor;
    }

    // Build the AST
    ExpressionNode *build_tree(int pre_precedence);

    static constexpr int MAX_PRECEDENCE = 0;
};
} // namespace myComp

#endif // MYCOMP_EXPRESSION_H
