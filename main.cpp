#define F_DEBUG

#include <iostream>

#include "Init.h"
#include "Parser.h"
#include "TokenProcessor.h"
#include "data.h"
#include "ArgParser.h"

using namespace myComp;

int main(int argc, char **argv) {
    try {
        // Parse the arguments
        ArgParser arg_parser;
        arg_parser.parse(argc, argv);

        // If debug mode, create log dir
        if (arg_parser.debug()) {
            std::filesystem::create_directories("logs");
        }

        // Initialize
        Init::init();

        TokenProcessor token_processor;
        token_processor.set_input(arg_parser.file_name());
        token_processor.process();
        if (arg_parser.debug()) {
            std::ofstream out("logs/tokens.txt");
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

        if (arg_parser.debug()) {
            std::ofstream out("logs/tree.txt");
            for (auto node : nodes) {
                node->print(out, 0);
            }
        }

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
