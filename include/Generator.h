#ifndef MYCOMP_GENERATOR_H
#define MYCOMP_GENERATOR_H

// This is the header file for the generator.
// The generator is responsible for generating the assembly code

#include "defs.h"

namespace myComp {

    class Generator {
    private:
        // Label
        int label_counter = 1;

        // Current offset
        int offset = 0;

        // Get a new label
        std::string get_label() { return "L" + std::to_string(label_counter++); }

        // Functions
        void function_declaration(const ASTNode *node);

        // Generate the assembly code for a statement
        void statement(const ASTNode *node);

        // Generate the assembly code for a compound statement
        void code_block(const ASTNode *node);

        // Generate the assembly code for an expression
        int expression(const ASTNode *node);

        // Generate the assembly code for an if statement
        void IF(const ASTNode *node);

        // Generate the assembly code for an if-else statement
        void if_else(const ASTNode *node);

        // Generate the assembly code for a while statement
        void WHILE(const ASTNode *node);

        // Generate the assembly code for a for statement
        void FOR(const ASTNode *node);

        // Generate the assembly code for a declaration statement
        void variable_declaration(const ASTNode *node);

        // Generate the assembly code for a return statement
        void RETURN(const ASTNode *node);

        // Generate the assembly code for a function call
        int call(const ASTNode *node);

        // Generate the assembly code for an assignment
        int assign(const ASTNode *node);

    public:
        // Generate the assembly code according to the AST
        void generate(const ASTNode *node);

        // Generate the preamble
        void generate_preamble();

        // Generate the postamble
        static void generate_postamble();

    };

} // myComp

#endif //MYCOMP_GENERATOR_H
