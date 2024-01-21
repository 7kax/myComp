#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "defs.h"

namespace myComp {
    class Debugger {
    public:
        // Print out the abstract syntax tree
        static void print_tree(std::ostream &out, const ASTNode *node, int indent = 0);

        // Print out the symbol table(global + local)
        static void print_symbol_table(std::ostream &out);
    };
}

#endif //DEBUGGER_H
