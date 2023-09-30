#ifndef MYCOMP_PARSER_H
#define MYCOMP_PARSER_H

// This is the header file for the parser.
// The parser is responsible for parsing the input

#include "defs.h"

namespace myComp {
    class Parser {
    private:
        static void variable_declaration(DataType data_type, std::string &name);

        static ASTNode *variable_declaration();

        static ASTNode *function_declaration(DataType return_type, const std::string &name);

        static ASTNode *code_block();

        static ASTNode *statement();

        static ASTNode *if_statement();

        static ASTNode *while_statement();

        static ASTNode *for_statement();

        static ASTNode *return_statement();

    public:
        static ASTNode *build_tree();
    };

} // myComp

#endif //MYCOMP_PARSER_H
