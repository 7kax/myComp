#ifndef MYCOMP_PARSER_H
#define MYCOMP_PARSER_H

// This is the header file for the parser.
// The parser is responsible for parsing the input

#include "defs.h"

#include "TokenProcessor.h"
#include "Expression.h"

namespace myComp {
    class Parser {
    private:
        TokenProcessor *token_processor_ = nullptr;

        Expression *expression_ = nullptr;

        // Global variables
        void variable_declaration(Type_ *data_type, std::string &name);

        // Local variables
        ASTNode *variable_declaration();

        ASTNode *function_declaration(Type_ *return_type,
                                      const std::string &name);

        ASTNode *code_block();

        ASTNode *statement();

        ASTNode *if_statement();

        ASTNode *while_statement();

        ASTNode *for_statement();

        ASTNode *return_statement();

    public:
        void set_processor(TokenProcessor *token_processor) { this->token_processor_ = token_processor; }

        void set_expression(Expression *expression) { this->expression_ = expression; }

        ASTNode *build_tree();
    };
} // namespace myComp

#endif // MYCOMP_PARSER_H
