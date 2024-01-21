#include "defs.h"
#include "objects.h"
#include "data.h"

#include "x_86.h"

namespace myComp {
    const std::vector<std::string> x_86::registers = {
        "%r10", "%r11", "%r12", "%r13",
        "%r9", "%r8", "%rcx", "%rdx", "%rsi", "%rdi"
    };
    const std::vector<std::string> x_86::b_registers = {
        "%r10b", "%r11b", "%r12b", "%r13b",
        "%r9b", "%r8b", "%cl", "%dl", "%sil", "%dil"
    };
    const std::vector<std::string> x_86::d_registers = {
        "%r10d", "%r11d", "%r12d", "%r13d",
        "%r9d", "%r8d", "%ecx", "%edx", "%esi", "%edi"
    };

    // Compare 2 registers
    int x_86::compare_registers(int r1, int r2, const std::string &op) {
        x_86::output << "\tcmpq\t" << registers[r2] << ", " << registers[r1] << "\n"
                << "\t" << op << "\t" << b_registers[r1] << "\n"
                << "\tmovzbq\t" << b_registers[r1] << ", " << registers[r1] << "\n";
        x_86::free_register(r2);
        return r1;
    }

    // Set the output file
    void x_86::set_output(const std::string &filename) {
        x_86::output.open(filename);
        if (!x_86::output.is_open()) {
            Errors::fatal_error("cannot open output file");
        }
    }

    // prelude and postlude
    void x_86::prelude() {
        x_86::free_all_registers();
        x_86::output << "\t.text\n";
    }

    void x_86::postlude() {
    }

    void x_86::function_prelude(const std::string &name) {
        x_86::output << "\t.text\n"
                << "\t.globl\t" << name << "\n"
                << "\t.type\t" << name << ", @function\n"
                << name << ":\n"
                << "\tpushq\t%rbp\n"
                << "\tmovq\t%rsp, %rbp\n";
    }

    void x_86::function_postlude(const std::string &name, int offset) {
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
        auto it = std::find(x_86::free_registers.begin(), x_86::free_registers.end(), true);
        if (it == x_86::free_registers.end()) {
            Errors::fatal_error("out of registers");
        }
        int index = static_cast<int>(std::distance(x_86::free_registers.begin(), it));
        x_86::free_registers[index] = false;
        return index;
    }

    void x_86::free_register(int reg) {
        if (x_86::free_registers[reg]) {
            Errors::fatal_error("register is already free");
        }
        x_86::free_registers[reg] = true;
    }

    int x_86::widen_integer(int reg, DataType src, DataType dest) {
        return reg;
    }

    int x_86::scale_integer(int reg, int val) {
        x_86::output << "\timulq\t$" << val << ", " << registers[reg] << "\n";
        return reg;
    }

    // Memory management
    int x_86::load_integer(int value) {
        int reg = x_86::allocate_register();
        x_86::output << "\tmovq\t$" << value << ", " << registers[reg] << "\n";
        return reg;
    }

    int x_86::load_symbol(const std::string &name) {
        int reg = x_86::allocate_register();
        std::string location = x_86::variable_location(name);
        if ((symbol_tables[context.get_name()].contains(name)
                 ? symbol_tables[context.get_name()].get_size(name)
                 : symbol_tables["global"].get_size(name)) == 1)
            switch (Type::get_data_type(name)) {
                case DataType::CHAR:
                    x_86::output << "\tmovsbq\t" << location << ", " << registers[reg] << "\n";
                    break;
                case DataType::INT:
                    x_86::output << "\tmovslq\t" << location << ", " << registers[reg] << "\n";
                    break;
                case DataType::LONG:
                case DataType::CHAR_PTR:
                case DataType::INT_PTR:
                case DataType::LONG_PTR:
                case DataType::VOID_PTR:
                    x_86::output << "\tmovq\t" << location << ", " << registers[reg] << "\n";
                    break;
                default:
                    Errors::fatal_error("invalid data type");
            }
        else
            x_86::output << "\tleaq\t" << location << ", " << registers[reg] << "\n";
        return reg;
    }

    void x_86::allocate_global_symbol(const std::string &name) {
        int data_size = Type::data_size.at(symbol_tables["global"].get_data_type(name));
        int array_size = symbol_tables["global"].get_size(name);
        x_86::output << "\t.data\n" << "\t.globl\t" << name << "\n";
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

    int x_86::store_symbol(int reg, const std::string &name) {
        DataType data_type = Type::get_data_type(name);
        std::string location = x_86::variable_location(name);
        switch (data_type) {
            case DataType::CHAR:
                x_86::output << "\tmovb\t" << b_registers[reg] << ", " << location << "\n";
                break;
            case DataType::INT:
                x_86::output << "\tmovl\t" << d_registers[reg] << ", " << location << "\n";
                break;
            case DataType::LONG:
            case DataType::CHAR_PTR:
            case DataType::INT_PTR:
            case DataType::LONG_PTR:
            case DataType::VOID_PTR:
                x_86::output << "\tmovq\t" << registers[reg] << ", " << location << "\n";
                break;
            default:
                Errors::fatal_error("invalid data type");
        }
        return reg;
    }


    int x_86::store_address(int address_reg, int val_reg, DataType data_type) {
        switch (data_type) {
            case DataType::CHAR:
                output << "\tmovb\t" << b_registers[val_reg] << ", (" << registers[address_reg] << ")\n";
                break;
            case DataType::INT:
                output << "\tmovl\t" << d_registers[val_reg] << ", (" << registers[address_reg] << ")\n";
                break;
            case DataType::LONG:
                output << "\tmovq\t" << registers[val_reg] << ", (" << registers[address_reg] << ")\n";
                break;
            default:
                Errors::fatal_error("invalid data type");
        }
        free_register(address_reg);
        return val_reg;
    }

    int x_86::load_address(const std::string &name) {
        int reg = x_86::allocate_register();

        x_86::output << "\tleaq\t" << x_86::variable_location(name) << ", " << registers[reg] << "\n";
        return reg;
    }

    int x_86::dereference(int reg, DataType data_type) {
        switch (data_type) {
            case DataType::CHAR:
                x_86::output << "\tmovsbq\t" << "(" << registers[reg] << "), " << registers[reg] << "\n";
                break;
            case DataType::INT:
                x_86::output << "\tmovslq\t" << "(" << registers[reg] << "), " << registers[reg] << "\n";
                break;
            case DataType::LONG:
                x_86::output << "\tmovq\t" << "(" << registers[reg] << "), " << registers[reg] << "\n";
                break;
            default:
                Errors::fatal_error("invalid data type");
        }
        return reg;
    }

    int x_86::dereference(int address_reg, int offset_reg, DataType data_type) {
        int reg = x_86::allocate_register();
        int data_size = Type::data_size.at(data_type);
        switch (data_size) {
            case 1:
                x_86::output << "\tmovsbq\t" << "(" << registers[address_reg] << ", " << registers[offset_reg]
                        << ", 1), "
                        << registers[reg] << "\n";
                break;
            case 4:
                x_86::output << "\tmovslq\t" << "(" << registers[address_reg] << ", " << registers[offset_reg]
                        << ", 4), "
                        << registers[reg] << "\n";
                break;
            case 8:
                x_86::output << "\tmovq\t" << "(" << registers[address_reg] << ", " << registers[offset_reg] << ", 8), "
                        << registers[reg] << "\n";
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

    int x_86::assign(const std::string &name, int reg) {
        return x_86::store_symbol(reg, name);
    }

    int x_86::assign(int address_reg, int val_reg, DataType data_type) {
        return x_86::store_address(address_reg, val_reg, data_type);
    }

    int x_86::assign(int address_reg, int offset_reg, int val_reg, DataType data_type) {
        int data_size = Type::data_size.at(data_type);
        //if it's an array, we need to get the address of the first element
        // multiply offset by data size
        x_86::output << "\timulq\t$" << data_size << ", " << registers[offset_reg] << "\n";
        // add offset to address
        x_86::output << "\taddq\t" << registers[offset_reg] << ", " << registers[address_reg] << "\n";
        x_86::free_register(offset_reg);
        // store value at address
        return x_86::store_address(address_reg, val_reg, data_type);
    }


    // Control flow operations
    void x_86::jump(const std::string &label) {
        x_86::output << "\tjmp\t" << label << "\n";
    }

    void x_86::jump_zero(int reg, const std::string &label) {
        x_86::output << "\ttestq\t" << registers[reg] << ", " << registers[reg] << "\n"
                << "\tjz\t" << label << "\n";
        x_86::free_register(reg);
    }

    void x_86::label(const std::string &label) {
        x_86::output << label << ":\n";
    }

    int x_86::call(const std::string &name) {
        x_86::output << "\tcall\t" << name << "\n";
        // Remove parameters from stack
        if (int size = static_cast<int>(function.get_prototype(name).parameters.size()); size > 6)
            x_86::output << "\taddq\t$" << (size - 6) * 8 << ", %rsp\n";
        if (function.get_prototype(name).return_type == DataType::VOID) {
            return -1;
        }
        int return_reg = x_86::allocate_register();
        x_86::output << "\tmovq\t%rax, " << registers[return_reg] << "\n";
        return return_reg;
    }

    void x_86::RETURN(int reg) {
        std::string name = context.get_name();
        if (function.get_prototype(name).return_type == DataType::VOID) {
            if (reg != -1)
                Errors::fatal_error("void function cannot return a value");
            x_86::jump(context.get_end_label());
            return;
        }

        switch (function.get_prototype(name).return_type) {
            case DataType::CHAR:
                x_86::output << "\tmovzbl\t" << b_registers[reg] << ", %eax\n";
                break;
            case DataType::INT:
                x_86::output << "\tmovl\t" << d_registers[reg] << ", %eax\n";
                break;
            case DataType::LONG:
                x_86::output << "\tmovq\t" << registers[reg] << ", %rax\n";
                break;
            default:
                Errors::fatal_error("invalid data type");
        }
        x_86::free_register(reg);
        x_86::jump(context.get_end_label());
    }

    int x_86::load_string(const std::string &label) {
        int reg = x_86::allocate_register();
        x_86::output << "\tleaq\t" << label << "(%rip), " << registers[reg] << "\n";
        return reg;
    }

    void x_86::allocate_string_literal(const std::string &str, const std::string &label) {
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
        x_86::output << "\ttestq\t" << registers[reg] << ", " << registers[reg] << "\n"
                << "\tsete\t" << b_registers[reg] << "\n"
                << "\tmovzbq\t" << b_registers[reg] << ", " << registers[reg] << "\n";
        return reg;
    }

    int x_86::post_increment(const std::string &name) {
        int reg = x_86::load_symbol(name);
        DataType data_type = Type::get_data_type(name);
        std::string location = x_86::variable_location(name);
        switch (data_type) {
            case DataType::CHAR:
                x_86::output << "\tincb\t" << location << "\n";
                break;
            case DataType::INT:
                x_86::output << "\tincl\t" << location << "\n";
                break;
            case DataType::LONG:
            case DataType::CHAR_PTR:
            case DataType::INT_PTR:
            case DataType::LONG_PTR:
            case DataType::VOID_PTR:
                x_86::output << "\tincq\t" << location << "\n";
                break;
            default:
                Errors::fatal_error("invalid data type");
        }
        return reg;
    }

    int x_86::post_decrement(const std::string &name) {
        int reg = x_86::load_symbol(name);
        DataType data_type = Type::get_data_type(name);
        std::string location = x_86::variable_location(name);
        switch (data_type) {
            case DataType::CHAR:
                x_86::output << "\tdecb\t" << location << "\n";
                break;
            case DataType::INT:
                x_86::output << "\tdecl\t" << location << "\n";
                break;
            case DataType::LONG:
            case DataType::CHAR_PTR:
            case DataType::INT_PTR:
            case DataType::LONG_PTR:
            case DataType::VOID_PTR:
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

    int x_86::logical_or(int r1, int r2) {
        return 0;
    }

    int x_86::logical_and(int r1, int r2) {
        return 0;
    }

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

    int x_86::pre_increment(const std::string &name) {
        DataType data_type = Type::get_data_type(name);
        std::string location = x_86::variable_location(name);
        switch (data_type) {
            case DataType::CHAR:
                x_86::output << "\tincb\t" << location << "\n";
                break;
            case DataType::INT:
                x_86::output << "\tincl\t" << location << "\n";
                break;
            case DataType::LONG:
            case DataType::CHAR_PTR:
            case DataType::INT_PTR:
            case DataType::LONG_PTR:
            case DataType::VOID_PTR:
                x_86::output << "\tincq\t" << location << "\n";
                break;
            default:
                Errors::fatal_error("invalid data type");
        }
        return x_86::load_symbol(name);
    }

    int x_86::pre_decrement(const std::string &name) {
        DataType data_type = Type::get_data_type(name);
        std::string location = x_86::variable_location(name);
        switch (data_type) {
            case DataType::CHAR:
                x_86::output << "\tdecb\t" << location << "\n";
                break;
            case DataType::INT:
                x_86::output << "\tdecl\t" << location << "\n";
                break;
            case DataType::LONG:
            case DataType::CHAR_PTR:
            case DataType::INT_PTR:
            case DataType::LONG_PTR:
            case DataType::VOID_PTR:
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

    std::string x_86::variable_location(const std::string &identifier) {
        if (symbol_tables[context.get_name()].contains(identifier))
            return std::to_string(symbol_tables[context.get_name()].get_offset(identifier)) + "(%rbp)";
        else
            return identifier + "(%rip)";
    }

    void x_86::allocate_stack(int amount) {
        x_86::output << "\tsubq\t$" << amount << ", %rsp\n";
    }

    int x_86::move_parameters(const std::vector<parameter> &params) {
        int offset = 0;
        int up_offset = 16;
        for (int i = 0; i < params.size(); ++i) {
            int size = std::max(Type::data_size.at(params[i].type), 4);
            if (i < 6) {
                offset -= size;
                symbol_tables[context.get_name()].set_offset(params[i].name, offset);
                if (size == 4)
                    x_86::output << "\tmovl\t" << d_registers[num_registers - i - 1] << ", " << offset << "(%rbp)\n";
                else
                    x_86::output << "\tmovq\t" << registers[num_registers - i - 1] << ", " << offset << "(%rbp)\n";
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
            x_86::output << "\tmovq\t" << registers[src] << ", " << registers[num_registers - num_arg] << "\n";

        x_86::free_register(src);
    }
}
