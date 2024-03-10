#define F_DEBUG

#include <iostream>

#include "data.h"
#include "Init.h"
#include "TokenProcessor.h"
#include "Parser.h"

using namespace myComp;

int main(int argc, char **argv) {
    try {
        // If we don't have an argument, print a usage
        if (argc < 2) {
            std::cerr << "Usage: " << argv[0] << " [opts] <filename>"
                      << std::endl;
            exit(1);
        }

        // If -D is specified, set debug mode
        if (argc > 2 && std::string(argv[1]) == "-D") {
            debug = true;
            argv++;
        }

        // Initialize
        Init::init();

        TokenProcessor token_processor;
        token_processor.set_input(argv[1]);
        // token_processor.set_input("input.c");
        token_processor.process();
        if (debug) {
            std::ofstream out("tokens.txt");
            token_processor.print(out);
        }

        Parser parser;
        parser.set_processor(&token_processor);

        code_generator->set_output("out.s");

        // Build trees
        std::vector<ASTNode_ *> nodes;
        while (!token_processor.eof()) {
            ASTNode_ *tree = parser.build_tree();
            if (tree != nullptr) {
                nodes.push_back(tree);
            }
        }

        if (debug) {
            std::ofstream out("tree.txt");
            for (auto node : nodes) {
                node->print(out, 0);
            }
        }

        // If debug flag is set, print the trees and symbol tables
        // if (debug) {
        //     std::ofstream out("tree.txt");
        //     for (const auto &node : nodes) {
        //         out << "Tree:" << std::endl;
        //         Debugger::print_tree(out, node);
        //         out << std::endl;
        //     }
        //     out.close();

        //     ofstream out2("symbol_table.txt");
        //     Debugger::print_symbol_table(out2);
        //     out2.close();
        // }

        // Generate the assembly code
        code_generator->prelude();
        for (auto &node : nodes) {
            node->generate_code(code_generator);
            delete node;
        }
        code_generator->postlude();

        // End
        Init::end();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
