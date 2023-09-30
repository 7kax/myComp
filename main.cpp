#include "defs.h"
#include "objects.h"
#include "data.h"

using namespace std;

int main(int argc, char **argv) {
    // If we don't have an argument, print a usage
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <filename>" << endl;
        exit(1);
    }

    // Initialize
    myComp::Init::init();
    myComp::scanner.set_input(argv[1]);
    myComp::asm_generator->set_output("out.s");

    // Scan the first token
    myComp::scanner.next();

    // Build trees
    // ofstream out("tree.txt");
    std::vector<myComp::ASTNode *> nodes;
    while (myComp::scanner.get_token().type != myComp::TokenType::T_EOF) {
        myComp::ASTNode *tree = myComp::Parser::build_tree();
        if (tree != nullptr) {
            // myComp::Tree::print(out, parser->get_tree(), 0);
            nodes.push_back(tree);
        }
    }
    // out.close();

    // Generate the assembly code
    myComp::generator.generate_preamble();
    for (auto &node: nodes) {
        myComp::generator.generate(node);
        delete node;
    }
    myComp::Generator::generate_postamble();

    // End
    myComp::Init::end();
    return 0;
}
