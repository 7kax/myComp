#include "defs.h"
#include "objects.h"
#include "data.h"

namespace myComp {
  Symbol &find_symbol(const std::string &name) {
    if (!symbol_tables[context.get_name()].contains(name)) {
      if (!symbol_tables["global"].contains(name))
        Errors::fatal_error("undeclared identifier " + name + " in scope " +
                            context.get_name());

      return symbol_tables["global"].get_symbol_table().at(name);
    }
    return symbol_tables[context.get_name()].get_symbol_table().at(name);
  }
} // namespace myComp
