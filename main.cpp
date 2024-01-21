#include "defs.h"
#include "objects.h"
#include "data.h"

using namespace std;

int main(int argc, char **argv) {
    try {
        // If we don't have an argument, print a usage
        if (argc < 2) {
            cerr << "Usage: " << argv[0] << " [opts] <filename>" << endl;
            exit(1);
        }

        // If -D is specified, set debug mode
        if (argc > 2 && string(argv[1]) == "-D") {
            myComp::debug = true;
            argv++;
        }

        // Initialize
        myComp::Init::init();
        myComp::scanner.set_input(argv[1]);
        myComp::asm_generator->set_output("out.s");

        // Scan the first token
        myComp::scanner.next();

        // Build trees
        std::vector<myComp::ASTNode *> nodes;
        while (myComp::scanner.get_token().type != myComp::TokenType::T_EOF) {
            myComp::ASTNode *tree = myComp::Parser::build_tree();
            if (tree != nullptr) {
                nodes.push_back(tree);
            }
        }

        // If debug flag is set, print the trees and symbol tables
        if (myComp::debug) {
            ofstream out("tree.txt");
            for (const auto &node: nodes) {
                out << "Tree:" << endl;
                myComp::Debugger::print_tree(out, node);
                out << std::endl;
            }
            out.close();

            ofstream out2("symbol_table.txt");
            myComp::Debugger::print_symbol_table(out2);
            out2.close();
        }

        // Generate the assembly code
        myComp::generator.generate_preamble();
        for (auto &node: nodes) {
            myComp::generator.generate(node);
            delete node;
        }
        myComp::Generator::generate_postamble();

        // End
        myComp::Init::end();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
