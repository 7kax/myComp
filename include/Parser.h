#ifndef MYCOMP_PARSER_H
#define MYCOMP_PARSER_H

// This is the header file for the parser.
// The parser is responsible for parsing the input

#include "ASTNode.h"
#include "Expression.h"
#include "TokenProcessor.h"

namespace myComp {
class Parser {
  private:
    TokenProcessor *token_processor_ = nullptr;

    Expression expression_;

    // Global variables
    VariableDeclarationNode *variable_declaration(Type *data_type,
                                                  std::string &name);

    // Local variables
    VariableDeclarationNode *variable_declaration();

    FunctionDefinitionNode *function_declaration(Type *return_type,
                                                 const std::string &name);

    CodeBlockNode *code_block();

    StatementNode *statement();

    IfNode *if_statement();

    WhileNode *while_statement();

    ForNode *for_statement();

    ReturnNode *return_statement();

  public:
    void set_processor(TokenProcessor *token_processor) {
        this->token_processor_ = token_processor;
        this->expression_.set_processor(token_processor);
    }

    // void set_expression(Expression *expression) { this->expression_ =
    // expression; }

    ASTNode_ *build_tree();
};
} // namespace myComp

#endif // MYCOMP_PARSER_H
