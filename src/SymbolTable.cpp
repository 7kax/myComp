#include "defs.h"
#include "objects.h"
#include "data.h"

namespace myComp {
  std::map<std::string, Symbol> &SymbolTable::get_symbol_table() {
    return this->symbol_table;
  }

  const std::map<std::string, Symbol> &SymbolTable::get_symbol_table() const {
    return this->symbol_table;
  }

  void SymbolTable::insert(const std::string &name, Type_ *data_type) {
    if (this->contains(name))
      Errors::fatal_error("redeclaration of identifier " + name + " in scope " +
                          context.get_name());
    // If it's an array, convert it to a pointer
    // if (size > 1)
    // data_type = data_to_ptr.at(data_type);
    // data_type = data_type_info.at(data_type).pointer_form;
    // data_type = type_factory.arrayToPointer(data_type);
    this->symbol_table[name] = {data_type};
  }

  Type_ *SymbolTable::get_data_type(const std::string &name) const {
    if (!this->contains(name))
      Errors::fatal_error("undeclared identifier " + name + " in scope " +
                          context.get_name());
    return this->symbol_table.at(name).type;
  }

  bool SymbolTable::contains(const std::string &name) const {
    return this->symbol_table.contains(name);
  }

  // int SymbolTable::get_size(const std::string &name) const {
  //   if (!this->contains(name))
  //     Errors::fatal_error("undeclared identifier " + name + " in scope " +
  //                         context.get_name());
  //   return this->symbol_table.at(name).size;
  // }

  void SymbolTable::set_offset(const std::string &name, int offset) {
    if (!this->contains(name))
      Errors::fatal_error("undeclared identifier " + name + " in scope " +
                          context.get_name());
    this->symbol_table[name].offset = offset;
  }

  int SymbolTable::get_offset(const std::string &name) const {
    if (!this->contains(name))
      Errors::fatal_error("undeclared identifier " + name + " in scope " +
                          context.get_name());
    return this->symbol_table.at(name).offset;
  }
} // namespace myComp
