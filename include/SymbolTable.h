#ifndef MYCOMP_SYMBOLTABLE_H
#define MYCOMP_SYMBOLTABLE_H

#include "defs.h"

namespace myComp {
class SymbolTable {
private:
  // Initially contains item "global" with empty symbol table
  std::map<std::string, Symbol> symbol_table;

public:
  [[nodiscard]] std::map<std::string, Symbol> &get_symbol_table();

  [[nodiscard]] const std::map<std::string, Symbol> &get_symbol_table() const;

  void insert(const std::string &name, Type_ *data_type);

  [[nodiscard]] Type_ *get_data_type(const std::string &name) const;

  [[nodiscard]] bool contains(const std::string &name) const;

  // [[nodiscard]] int get_size(const std::string &name) const;

  void set_offset(const std::string &name, int offset);

  [[nodiscard]] int get_offset(const std::string &name) const;
};

// using SymbolTable = std::map<std::string, Symbol>;
} // namespace myComp

#endif // MYCOMP_SYMBOLTABLE_H
