#include "defs.h"
#include "objects.h"
#include "data.h"

namespace myComp {
void Debugger::print_tree(std::ostream &out, const ASTNode *node, int indent) {
  for (int i = 0; i < indent; ++i) {
    out << '\t';
  }

  out << ASTNode_str.at(node->type) << ": (left: "
      << (node->left == nullptr ? "null" : ASTNode_str.at(node->left->type))
      << ", right: "
      << (node->right == nullptr ? "null" : ASTNode_str.at(node->right->type))
      << ")\n";

  if (node->left != nullptr)
    Debugger::print_tree(out, node->left, indent + 1);
  if (node->right != nullptr)
    Debugger::print_tree(out, node->right, indent + 1);
}

void Debugger::print_symbol_table(std::ostream &out) {
  for (const auto &[name, symbol_table] : symbol_tables) {
    out << "Symbol table for " << name << ":\n";
    for (const auto &[name, symbol] : symbol_table.get_symbol_table()) {
      out << "\t" << name << ": " << symbol.type->str()
          << ", size: " << symbol.type->size() << ", offset: " << symbol.offset
          << '\n';
    }
  }
}
} // namespace myComp
