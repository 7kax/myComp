#include "defs.h"
#include "objects.h"
#include "data.h"

#include "x_86.h"

namespace myComp {
  const std::vector<std::string> x_86::registers = {
    "%r10", "%r11", "%r12", "%r13", "%r9",
    "%r8", "%rcx", "%rdx", "%rsi", "%rdi"
  };
  const std::vector<std::string> x_86::b_registers = {
    "%r10b", "%r11b", "%r12b", "%r13b", "%r9b",
    "%r8b", "%cl", "%dl", "%sil", "%dil"
  };
  const std::vector<std::string> x_86::d_registers = {
    "%r10d", "%r11d", "%r12d", "%r13d", "%r9d",
    "%r8d", "%ecx", "%edx", "%esi", "%edi"
  };

  // Compare 2 registers
  int x_86::compare_registers(const int r1, const int r2,
                              const std::string_view op) {
    x_86::output << "\tcmpq\t" << registers[r2] << ", " << registers[r1] << "\n"
        << "\t" << op << "\t" << b_registers[r1] << "\n"
        << "\tmovzbq\t" << b_registers[r1] << ", " << registers[r1]
        << "\n";
    x_86::free_register(r2);
    return r1;
  }

  // Set the output file
  void x_86::set_output(const std::string_view filename) {
    x_86::output.open(filename.data());

    if (!x_86::output.is_open()) {
      Errors::fatal_error("cannot open output file " + std::string(filename));
    }
  }

  // prelude and postlude
  void x_86::prelude() {
    x_86::free_all_registers();
    x_86::output << "\t.text\n";
  }

  void x_86::postlude() {
  }

  void x_86::function_prelude(const std::string_view name) {
    x_86::output << "\t.text\n"
        << "\t.globl\t" << name << "\n"
        << "\t.type\t" << name << ", @function\n"
        << name << ":\n"
        << "\tpushq\t%rbp\n"
        << "\tmovq\t%rsp, %rbp\n";
  }

  void x_86::function_postlude(std::string_view name, const int offset) {
    // Generate end label
    x_86::label(context.get_end_label());
    // Restore stack pointer
    x_86::output << "\taddq\t$" << -offset << ", %rsp\n"
        << "\tpopq\t%rbp\n"
        << "\tret\n";
  }

  // Register management
  void x_86::free_all_registers() {
    std::fill(x_86::free_registers.begin(), x_86::free_registers.end(), true);
  }

  int x_86::allocate_register() {
    // const auto it = std::ranges::find(x_86::free_registers, true);
    //
    // if (it == x_86::free_registers.end()) {
    //     Errors::fatal_error("out of registers");
    // }
    //
    // const int index =
    // static_cast<int>(std::distance(x_86::free_registers.begin(), it));
    // x_86::free_registers[index] = false;

    for (int i = 0; i < num_registers; ++i) {
      if (x_86::free_registers[i]) {
        x_86::free_registers[i] = false;
        return i;
      }
    }

    Errors::fatal_error("out of registers");
    return -1;
  }

  void x_86::free_register(const int reg) {
    if (x_86::free_registers[reg]) {
      Errors::fatal_error("register is already free");
    }
    x_86::free_registers[reg] = true;
  }

  int x_86::widen_integer(const int reg, Type_ *src, Type_ *dest) { return reg; }

  int x_86::scale_integer(const int reg, const int val) {
    x_86::output << "\timulq\t$" << val << ", " << registers[reg] << "\n";
    return reg;
  }

  // Memory management
  int x_86::load_integer(const int value) {
    const int reg = x_86::allocate_register();
    x_86::output << "\tmovq\t$" << value << ", " << registers[reg] << "\n";
    return reg;
  }

  int x_86::load_symbol(const std::string_view name) {
    const int reg = x_86::allocate_register();
    const std::string location = x_86::variable_location(name);
    auto &[type, offset] = find_symbol(name.data());
    if (!type->is_array())
      switch (type->size()) {
        case 1:
          x_86::output << "\tmovsbq\t" << location << ", " << registers[reg]
              << "\n";
          break;
        case 4:
          x_86::output << "\tmovslq\t" << location << ", " << registers[reg]
              << "\n";
          break;
        case 8:
          x_86::output << "\tmovq\t" << location << ", " << registers[reg] << "\n";
          break;
        default:
          Errors::fatal_error("invalid data type");
      }
    else
      x_86::output << "\tleaq\t" << location << ", " << registers[reg] << "\n";
    return reg;
  }

  void x_86::allocate_global_symbol(std::string_view name) {
    // const int data_size =
    // Type::data_size.at(symbol_tables["global"].get_data_type(std::string(name)));
    // int data_size =
    //     symbol_tables["global"].get_data_type(std::string(name))->size();
    //  int array_size = symbol_tables["global"].get_size(std::string(name));
    Symbol &symbol = find_symbol(name.data());
    int array_size = 1;
    int data_size = 0;
    if (symbol.type->is_array()) {
      array_size = dynamic_cast<ArrayType *>(symbol.type)->size_of_array();
      data_size = dynamic_cast<ArrayType *>(symbol.type)->size_of_element();
    } else {
      data_size = symbol.type->size();
    }
    x_86::output << "\t.data\n"
        << "\t.globl\t" << name << "\n";
    x_86::output << name << ":\n";
    for (int i = 0; i < array_size; i++) {
      switch (data_size) {
        case 1:
          x_86::output << "\t.byte\t0\n";
          break;
        case 4:
          x_86::output << "\t.long\t0\n";
          break;
        case 8:
          x_86::output << "\t.quad\t0\n";
          break;
        default:
          Errors::fatal_error("invalid data size");
      }
    }
  }

  int x_86::store_symbol(int reg, std::string_view name) {
    // const DataType data_type = Type::get_data_type(std::string(name));
    const std::string location = x_86::variable_location(name);
    Symbol &symbol = find_symbol(name.data());
    switch (symbol.type->size()) {
      case 1:
        x_86::output << "\tmovb\t" << b_registers[reg] << ", " << location << "\n";
        break;
      case 4:
        x_86::output << "\tmovl\t" << d_registers[reg] << ", " << location << "\n";
        break;
      case 8:
        x_86::output << "\tmovq\t" << registers[reg] << ", " << location << "\n";
        break;
      default:
        Errors::fatal_error("invalid data type");
    }
    return reg;
  }

  int x_86::store_address(int address_reg, int val_reg, Type_ *data_type) {
    switch (data_type->size()) {
      case 1:
        output << "\tmovb\t" << b_registers[val_reg] << ", ("
            << registers[address_reg] << ")\n";
        break;
      case 4:
        output << "\tmovl\t" << d_registers[val_reg] << ", ("
            << registers[address_reg] << ")\n";
        break;
      case 8:
        output << "\tmovq\t" << registers[val_reg] << ", ("
            << registers[address_reg] << ")\n";
        break;
      default:
        Errors::fatal_error("invalid data type");
    }
    free_register(address_reg);
    return val_reg;
  }

  int x_86::load_address(std::string_view name) {
    int reg = x_86::allocate_register();

    x_86::output << "\tleaq\t" << x_86::variable_location(name) << ", "
        << registers[reg] << "\n";
    return reg;
  }

  int x_86::dereference(int reg, Type_ *data_type) {
    switch (data_type->size()) {
      case 1:
        x_86::output << "\tmovsbq\t"
            << "(" << registers[reg] << "), " << registers[reg] << "\n";
        break;
      case 4:
        x_86::output << "\tmovslq\t"
            << "(" << registers[reg] << "), " << registers[reg] << "\n";
        break;
      case 8:
        x_86::output << "\tmovq\t"
            << "(" << registers[reg] << "), " << registers[reg] << "\n";
        break;
      default:
        Errors::fatal_error("invalid data type");
    }
    return reg;
  }

  int x_86::dereference(int address_reg, int offset_reg, Type_ *data_type) {
    int reg = x_86::allocate_register();
    // int data_size = Type::data_size.at(data_type);
    switch (data_type->size()) {
      case 1:
        x_86::output << "\tmovsbq\t"
            << "(" << registers[address_reg] << ", "
            << registers[offset_reg] << ", 1), " << registers[reg] << "\n";
        break;
      case 4:
        x_86::output << "\tmovslq\t"
            << "(" << registers[address_reg] << ", "
            << registers[offset_reg] << ", 4), " << registers[reg] << "\n";
        break;
      case 8:
        x_86::output << "\tmovq\t"
            << "(" << registers[address_reg] << ", "
            << registers[offset_reg] << ", 8), " << registers[reg] << "\n";
        break;
      default:
        Errors::fatal_error("invalid data size");
    }
    x_86::free_register(address_reg);
    x_86::free_register(offset_reg);
    return reg;
  }

  // Arithmetic operators
  int x_86::add(int r1, int r2) {
    x_86::output << "\taddq\t" << registers[r2] << ", " << registers[r1] << "\n";
    x_86::free_register(r2);
    return r1;
  }

  int x_86::sub(int r1, int r2) {
    x_86::output << "\tsubq\t" << registers[r2] << ", " << registers[r1] << "\n";
    x_86::free_register(r2);
    return r1;
  }

  int x_86::mul(int r1, int r2) {
    x_86::output << "\timulq\t" << registers[r2] << ", " << registers[r1] << "\n";
    x_86::free_register(r2);
    return r1;
  }

  int x_86::div(int r1, int r2) {
    x_86::output << "\tmovq\t" << registers[r1] << ", %rax\n"
        << "\tcqo\n"
        << "\tidivq\t" << registers[r2] << "\n"
        << "\tmovq\t%rax, " << registers[r1] << "\n";
    x_86::free_register(r2);
    return r1;
  }

  int x_86::assign(std::string_view name, int reg) {
    return x_86::store_symbol(reg, name);
  }

  int x_86::assign(int address_reg, int val_reg, Type_ *data_type) {
    return x_86::store_address(address_reg, val_reg, data_type);
  }

  int x_86::assign(int address_reg, int offset_reg, int val_reg,
                   Type_ *data_type) {
    // int data_size = Type::data_size.at(data_type);
    // int data_size = data_type_info.at(data_type).size;
    // if it's an array, we need to get the address of the first element
    //  multiply offset by data size
    x_86::output << "\timulq\t$" << data_type->size() << ", "
        << registers[offset_reg] << "\n";
    // add offset to address
    x_86::output << "\taddq\t" << registers[offset_reg] << ", "
        << registers[address_reg] << "\n";
    x_86::free_register(offset_reg);
    // store value at address
    return x_86::store_address(address_reg, val_reg, data_type);
  }

  // Control flow operations
  void x_86::jump(std::string_view label) {
    x_86::output << "\tjmp\t" << label << "\n";
  }

  void x_86::jump_zero(int reg, std::string_view label) {
    x_86::output << "\ttestq\t" << registers[reg] << ", " << registers[reg]
        << "\n"
        << "\tjz\t" << label << "\n";
    x_86::free_register(reg);
  }

  void x_86::label(std::string_view label) { x_86::output << label << ":\n"; }

  int x_86::call(std::string_view name) {
    x_86::output << "\tcall\t" << name << "\n";
    // Remove parameters from stack
    if (int size = static_cast<int>(
        function.get_prototype(std::string(name)).parameters.size());
      size > 6)
      x_86::output << "\taddq\t$" << (size - 6) * 8 << ", %rsp\n";
    if (function.get_prototype(std::string(name)).return_type->is_void()) {
      return -1;
    }
    int return_reg = x_86::allocate_register();
    x_86::output << "\tmovq\t%rax, " << registers[return_reg] << "\n";
    return return_reg;
  }

  void x_86::RETURN(int reg) {
    std::string name = context.get_name();
    if (function.get_prototype(name).return_type->is_void()) {
      if (reg != -1)
        Errors::fatal_error("void function cannot return a value");
      x_86::jump(context.get_end_label());
      return;
    }

    switch (function.get_prototype(name).return_type->size()) {
      case 1:
        x_86::output << "\tmovzbl\t" << b_registers[reg] << ", %eax\n";
        break;
      case 4:
        x_86::output << "\tmovl\t" << d_registers[reg] << ", %eax\n";
        break;
      case 8:
        x_86::output << "\tmovq\t" << registers[reg] << ", %rax\n";
        break;
      default:
        Errors::fatal_error("invalid data type");
    }
    x_86::free_register(reg);
    x_86::jump(context.get_end_label());
  }

  int x_86::load_string(std::string_view label) {
    int reg = x_86::allocate_register();
    x_86::output << "\tleaq\t" << label << "(%rip), " << registers[reg] << "\n";
    return reg;
  }

  void x_86::allocate_string_literal(std::string_view str,
                                     std::string_view label) {
    x_86::label(label);
    for (char c: str) {
      x_86::output << "\t.byte\t" << static_cast<int>(c) << "\n";
    }
    x_86::output << "\t.byte\t0\n";
  }

  int x_86::invert(int reg) {
    x_86::output << "\tnotq\t" << registers[reg] << "\n";
    return reg;
  }

  int x_86::NOT(int reg) {
    x_86::output << "\ttestq\t" << registers[reg] << ", " << registers[reg]
        << "\n"
        << "\tsete\t" << b_registers[reg] << "\n"
        << "\tmovzbq\t" << b_registers[reg] << ", " << registers[reg]
        << "\n";
    return reg;
  }

  int x_86::post_increment(std::string_view name) {
    int reg = x_86::load_symbol(name);
    // DataType data_type = Type::get_data_type(std::string(name));
    Symbol &symbol = find_symbol(name.data());
    std::string location = x_86::variable_location(name);
    switch (symbol.type->size()) {
      case 1:
        x_86::output << "\tincb\t" << location << "\n";
        break;
      case 4:
        x_86::output << "\tincl\t" << location << "\n";
        break;
      case 8:
        x_86::output << "\tincq\t" << location << "\n";
        break;
      default:
        Errors::fatal_error("invalid data type");
    }
    return reg;
  }

  int x_86::post_decrement(std::string_view name) {
    int reg = x_86::load_symbol(name);
    Symbol &symbol = find_symbol(name.data());
    std::string location = x_86::variable_location(name);
    switch (symbol.type->size()) {
      case 1:
        x_86::output << "\tdecb\t" << location << "\n";
        break;
      case 4:
        x_86::output << "\tdecl\t" << location << "\n";
        break;
      case 8:
        x_86::output << "\tdecq\t" << location << "\n";
        break;
      default:
        Errors::fatal_error("invalid data type");
    }
    return reg;
  }

  int x_86::OR(int r1, int r2) {
    x_86::output << "\torq\t" << registers[r1] << ", " << registers[r2] << "\n";
    x_86::free_register(r1);
    return r2;
  }

  int x_86::AND(int r1, int r2) {
    x_86::output << "\tandq\t" << registers[r1] << ", " << registers[r2] << "\n";
    x_86::free_register(r1);
    return r2;
  }

  int x_86::XOR(int r1, int r2) {
    x_86::output << "\txorq\t" << registers[r1] << ", " << registers[r2] << "\n";
    x_86::free_register(r1);
    return r2;
  }

  int x_86::logical_or(int r1, int r2) { return 0; }

  int x_86::logical_and(int r1, int r2) { return 0; }

  int x_86::left_shift(int r1, int r2) {
    x_86::output << "\tmovb\t" << b_registers[r2] << ", %cl\n"
        << "\tshlq\t%cl, " << registers[r1] << "\n";
    x_86::free_register(r2);
    return r1;
  }

  int x_86::right_shift(int r1, int r2) {
    x_86::output << "\tmovb\t" << b_registers[r2] << ", %cl\n"
        << "\tshrq\t%cl, " << registers[r1] << "\n";
    x_86::free_register(r2);
    return r1;
  }

  int x_86::pre_increment(std::string_view name) {
    Symbol &symbol = find_symbol(name.data());
    std::string location = x_86::variable_location(name);
    switch (symbol.type->size()) {
      case 1:
        x_86::output << "\tincb\t" << location << "\n";
        break;
      case 4:
        x_86::output << "\tincl\t" << location << "\n";
        break;
      case 8:
        x_86::output << "\tincq\t" << location << "\n";
        break;
      default:
        Errors::fatal_error("invalid data type");
    }
    return x_86::load_symbol(name);
  }

  int x_86::pre_decrement(std::string_view name) {
    Symbol &symbol = find_symbol(name.data());
    std::string location = x_86::variable_location(name);
    switch (symbol.type->size()) {
      case 1:
        x_86::output << "\tdecb\t" << location << "\n";
        break;
      case 4:
        x_86::output << "\tdecl\t" << location << "\n";
        break;
      case 8:
        x_86::output << "\tdecq\t" << location << "\n";
        break;
      default:
        Errors::fatal_error("invalid data type");
    }
    return x_86::load_symbol(name);
  }

  int x_86::negative(int reg) {
    x_86::output << "\tnegq\t" << registers[reg] << "\n";
    return reg;
  }

  std::string x_86::variable_location(std::string_view identifier) {
    if (symbol_tables[context.get_name()].contains(std::string(identifier)))
      return std::to_string(symbol_tables[context.get_name()].get_offset(
               std::string(identifier))) +
             "(%rbp)";
    else
      return std::string(identifier) + "(%rip)";
  }

  void x_86::allocate_stack(int amount) {
    x_86::output << "\tsubq\t$" << amount << ", %rsp\n";
  }

  int x_86::move_parameters(const std::vector<parameter> &params) {
    int offset = 0;
    int up_offset = 16;
    for (int i = 0; i < params.size(); ++i) {
      // int size = std::max(Type::data_size.at(params[i].type), 4);
      int size = std::max(params[i].type->size(), 4);
      if (i < 6) {
        offset -= size;
        symbol_tables[context.get_name()].set_offset(params[i].name, offset);
        if (size == 4)
          x_86::output << "\tmovl\t" << d_registers[num_registers - i - 1] << ", "
              << offset << "(%rbp)\n";
        else
          x_86::output << "\tmovq\t" << registers[num_registers - i - 1] << ", "
              << offset << "(%rbp)\n";
      } else {
        symbol_tables[context.get_name()].set_offset(params[i].name, up_offset);
        up_offset += 8;
      }
    }

    return offset;
  }

  void x_86::copy_argument(int src, int num_arg) {
    if (num_arg > 6)
      x_86::output << "\tpushq\t" << registers[src] << "\n";
    else
      x_86::output << "\tmovq\t" << registers[src] << ", "
          << registers[num_registers - num_arg] << "\n";

    x_86::free_register(src);
  }
} // namespace myComp
