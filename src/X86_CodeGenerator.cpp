#include "X86_CodeGenerator.h"
#include "data.h"

namespace myComp {
void X86_CodeGenerator::set_output(std::string_view filename) {
    output_file_.open(filename.data());
    if (!output_file_.is_open()) {
        throw std::runtime_error("Unable to open file " +
                                 std::string(filename));
    }
}

void X86_CodeGenerator::free_register(int reg) {
    if (reg < 0 || reg >= NUM_REGISTERS) {
        throw std::runtime_error("Invalid register number");
    }
    if (free_registers_[reg]) {
        throw std::runtime_error("Register already free");
    }
    free_registers_[reg] = true;
}

void X86_CodeGenerator::function_prelude(std::string_view name) {
    // Prelude assembly code
    output_file_ << "\t.text\n"
                 << "\t.globl\t" << name << "\n"
                 << "\t.type\t" << name << ", @function\n"
                 << name << ":\n"
                 << "\tpushq\t%rbp\n"
                 << "\tmovq\t%rsp, %rbp\n";

    // Mark the start of a function
    in_function_ = true;

    // Record the function name
    function_name_ = name;

    // Initialize stack size = 0
    stack_size_ = 0;

    // Set the end label
    end_label_ = allocate_label();
}

void X86_CodeGenerator::function_postlude() {
    // Generate end label
    add_label(end_label_);

    // Mark the end of a function
    in_function_ = false;

    // Clear the offset map
    variable_offsets_.clear();

    // Restore stack pointer
    output_file_ << "\taddq\t$" << stack_size_ << ", %rsp\n"
                 << "\tpopq\t%rbp\n"
                 << "\tret\n";
}

void X86_CodeGenerator::load_parameters(const std::vector<Variable *> &params) {
    // Offset of parameters passed through stack
    int up_offset = 16;

    for (int i = 0; i < params.size(); ++i) {
        // Get size of the parameter
        int size = std::max(params[i]->type->size(), static_cast<size_t>(4));

        // First 6 parameters are passed through registers
        if (i < 6) {
            // Allocate space for the parameter and align it to size
            stack_size_ += size;
            stack_size_ += stack_size_ % size;

            // Record the offset
            variable_offsets_[params[i]] = -stack_size_;

            // Generate assembly code
            if (size == 4) {
                output_file_ << "\tmovl\t" << d_registers[NUM_REGISTERS - i - 1]
                             << ", " << -stack_size_ << "(%rbp)\n";
            } else {
                output_file_ << "\tmovq\t" << registers[NUM_REGISTERS - i - 1]
                             << ", " << -stack_size_ << "(%rbp)\n";
            }
        } else {
            // Record the offset
            variable_offsets_[params[i]] = up_offset;

            // Allocate space for the parameter
            up_offset += 8;
        }
    }
}

void X86_CodeGenerator::allocate_local_variables(
    const std::vector<Variable *> &variables) {
    for (auto var : variables) {
        // Skip parameters
        if (variable_offsets_.contains(var)) {
            continue;
        }

        // Get size of the variable
        int size = var->type->size();

        // Allocate space for the variable and align it to size
        stack_size_ += size;
        stack_size_ += stack_size_ % size;

        // Record the offset
        variable_offsets_[var] = -stack_size_;
    }

    // Align the stack size to 16 bytes before allocating space
    stack_size_ = (stack_size_ + 15) / 16 * 16;

    // Allocate space on the stack for the local variables
    output_file_ << "\tsubq\t$" << stack_size_ << ", %rsp\n";
}

std::string X86_CodeGenerator::allocate_label() {
    return ".L" + std::to_string(label_count_++);
}

void X86_CodeGenerator::add_label(std::string_view label) {
    output_file_ << label << ":\n";
}

void X86_CodeGenerator::jump_on_zero(int reg, std::string_view label) {
    output_file_ << "\tcmpq\t$0, " << registers[reg] << "\n"
                 << "\tje\t" << label << "\n";

    free_register(reg);
}

void X86_CodeGenerator::jump_on_non_zero(int reg, std::string_view label) {
    output_file_ << "\tcmpq\t$0, " << registers[reg] << "\n"
                 << "\tjne\t" << label << "\n";

    free_register(reg);
}

void X86_CodeGenerator::jump(std::string_view label) {
    output_file_ << "\tjmp\t" << label << "\n";
}

void X86_CodeGenerator::return_from_function(int reg) {
    int size = FunctionManager::find(function_name_)->return_type_->size();
    if (size == 4) {
        output_file_ << "\tmovl\t" << d_registers[reg] << ", %eax\n";
    } else {
        output_file_ << "\tmovq\t" << registers[reg] << ", %rax\n";
    }

    free_register(reg);
    jump(end_label_);
}

void X86_CodeGenerator::type_cast(int reg, Type *src, Type *dest) {
    if (src->size() >= dest->size()) {
        return;
    }

    int src_size = src->size();
    std::string src_reg = get_reg_by_size(reg, src_size);
    int dest_size = dest->size();
    std::string dest_reg = get_reg_by_size(reg, dest_size);

    char sign_suffix = src->is_signed() ? 's' : 'z';
    char src_suffix = get_suffix_by_size(src_size);
    char dest_suffix = get_suffix_by_size(dest_size);
    std::string mov_inst =
        std::string("mov") + sign_suffix + src_suffix + dest_suffix;

    output_file_ << "\t" << mov_inst << "\t" << src_reg << ", " << dest_reg
                 << "\n";
}

int X86_CodeGenerator::negate(int reg, Type *type) {
    if (type->size() <= 4) {
        output_file_ << "\tnegl\t" << d_registers[reg] << "\n";
    } else {
        output_file_ << "\tnegq\t" << registers[reg] << "\n";
    }

    return reg;
}

int X86_CodeGenerator::add(int reg1, int reg2, Type *type) {
    if (type->size() <= 4) {
        output_file_ << "\taddl\t" << d_registers[reg2] << ", "
                     << d_registers[reg1] << "\n";
    } else {
        output_file_ << "\taddq\t" << registers[reg2] << ", " << registers[reg1]
                     << "\n";
    }

    free_register(reg2);
    return reg1;
}

int X86_CodeGenerator::subtract(int reg1, int reg2, Type *type) {
    if (type->size() <= 4) {
        output_file_ << "\tsubl\t" << d_registers[reg2] << ", "
                     << d_registers[reg1] << "\n";
    } else {
        output_file_ << "\tsubq\t" << registers[reg2] << ", " << registers[reg1]
                     << "\n";
    }

    free_register(reg2);
    return reg1;
}

int X86_CodeGenerator::multiply(int reg1, int reg2, Type *type) {
    if (type->size() <= 4) {
        output_file_ << "\timull\t" << d_registers[reg2] << ", "
                     << d_registers[reg1] << "\n";
    } else {
        output_file_ << "\timulq\t" << registers[reg2] << ", "
                     << registers[reg1] << "\n";
    }

    free_register(reg2);
    return reg1;
}

int X86_CodeGenerator::divide(int reg1, int reg2, Type *type) {
    // Make sure the rdx register is free
    bool should_pop = false;
    if (free_registers_[RDX_INDEX]) {
        free_registers_[RDX_INDEX] = false;
    } else {
        // Push the rdx register onto the stack
        output_file_ << "\tpushq\t%rdx\n";
        should_pop = true;
    }

    if (type->is_signed()) {
        if (type->size() <= 4) {
            output_file_ << "\tmovl\t" << d_registers[reg1] << ", %eax\n"
                         << "\tcltd\n"
                         << "\tidivl\t" << d_registers[reg2] << "\n"
                         << "\tmovl\t%eax, " << d_registers[reg1] << "\n";
        } else {
            output_file_ << "\tmovq\t" << registers[reg1] << ", %rax\n"
                         << "\tcqto\n"
                         << "\tidivq\t" << registers[reg2] << "\n"
                         << "\tmovq\t%rax, " << registers[reg1] << "\n";
        }
    } else {
        if (type->size() <= 4) {
            output_file_ << "\tmovl\t" << d_registers[reg1] << ", %eax\n"
                         << "\tmovl\t$0, %edx\n"
                         << "\tdivl\t" << d_registers[reg2] << "\n"
                         << "\tmovl\t%eax, " << d_registers[reg1] << "\n";
        } else {
            output_file_ << "\tmovq\t" << registers[reg1] << ", %rax\n"
                         << "\tmovl\t$0, %edx\n"
                         << "\tdivq\t" << registers[reg2] << "\n"
                         << "\tmovq\t%rax, " << registers[reg1] << "\n";
        }
    }

    // Restore the rdx register
    if (should_pop) {
        output_file_ << "\tpopq\t%rdx\n";
    } else {
        free_registers_[RDX_INDEX] = true;
    }

    free_register(reg2);
    return reg1;
}

int X86_CodeGenerator::modulo(int reg1, int reg2, Type *type) {
    // Make sure the rdx register is free
    bool should_pop = false;
    if (free_registers_[RDX_INDEX]) {
        free_registers_[RDX_INDEX] = false;
    } else {
        // Push the rdx register onto the stack
        output_file_ << "\tpushq\t%rdx\n";
        should_pop = true;
    }

    if (type->is_signed()) {
        if (type->size() <= 4) {
            output_file_ << "\tmovl\t" << d_registers[reg1] << ", %eax\n"
                         << "\tcltd\n"
                         << "\tidivl\t" << d_registers[reg2] << "\n"
                         << "\tmovl\t%edx, " << d_registers[reg1] << "\n";
        } else {
            output_file_ << "\tmovq\t" << registers[reg1] << ", %rax\n"
                         << "\tcqto\n"
                         << "\tidivq\t" << registers[reg2] << "\n"
                         << "\tmovq\t%rdx, " << registers[reg1] << "\n";
        }
    } else {
        if (type->size() <= 4) {
            output_file_ << "\tmovl\t" << d_registers[reg1] << ", %eax\n"
                         << "\tmovl\t$0, %edx\n"
                         << "\tdivl\t" << d_registers[reg2] << "\n"
                         << "\tmovl\t%edx, " << d_registers[reg1] << "\n";
        } else {
            output_file_ << "\tmovq\t" << registers[reg1] << ", %rax\n"
                         << "\tmovl\t$0, %edx\n"
                         << "\tdivq\t" << registers[reg2] << "\n"
                         << "\tmovq\t%rdx, " << registers[reg1] << "\n";
        }
    }

    // Restore the rdx register
    if (should_pop) {
        output_file_ << "\tpopq\t%rdx\n";
    } else {
        free_registers_[RDX_INDEX] = true;
    }

    free_register(reg2);
    return reg1;
}

int X86_CodeGenerator::bitwise_not(int reg, Type *type) {
    if (type->size() <= 4) {
        output_file_ << "\tnotl\t" << d_registers[reg] << "\n";
    } else {
        output_file_ << "\tnotq\t" << registers[reg] << "\n";
    }

    return reg;
}

int X86_CodeGenerator::bitwise_or(int reg1, int reg2, Type *type) {
    if (type->size() <= 4) {
        output_file_ << "\torl\t" << d_registers[reg2] << ", "
                     << d_registers[reg1] << "\n";
    } else {
        output_file_ << "\torq\t" << registers[reg2] << ", " << registers[reg1]
                     << "\n";
    }

    free_register(reg2);
    return reg1;
}

int X86_CodeGenerator::bitwise_and(int reg1, int reg2, Type *type) {
    if (type->size() <= 4) {
        output_file_ << "\tandl\t" << d_registers[reg2] << ", "
                     << d_registers[reg1] << "\n";
    } else {
        output_file_ << "\tandq\t" << registers[reg2] << ", " << registers[reg1]
                     << "\n";
    }

    free_register(reg2);
    return reg1;
}

int X86_CodeGenerator::bitwise_xor(int reg1, int reg2, Type *type) {
    if (type->size() <= 4) {
        output_file_ << "\txorl\t" << d_registers[reg2] << ", "
                     << d_registers[reg1] << "\n";
    } else {
        output_file_ << "\txorq\t" << registers[reg2] << ", " << registers[reg1]
                     << "\n";
    }

    free_register(reg2);
    return reg1;
}

int X86_CodeGenerator::left_shift(int reg1, int reg2, Type *type) {
    // Make sure the rcx register is free
    bool should_pop = false;
    if (free_registers_[RCX_INDEX]) {
        free_registers_[RCX_INDEX] = false;
    } else {
        // Push the rcx register onto the stack
        output_file_ << "\tpushq\t%rcx\n";
        should_pop = true;
    }

    // Move the shift amount into the cl register
    output_file_ << "\tmovb\t" << b_registers[reg2] << ", %cl\n";

    // Perform the shift
    if (type->size() <= 4) {
        output_file_ << "\tsall\t%cl, " << d_registers[reg1] << "\n";
    } else {
        output_file_ << "\tsalq\t%cl, " << registers[reg1] << "\n";
    }

    // Restore the rcx register
    if (should_pop) {
        output_file_ << "\tpopq\t%rcx\n";
    } else {
        free_registers_[RCX_INDEX] = true;
    }

    free_register(reg2);
    return reg1;
}

int X86_CodeGenerator::right_shift(int reg1, int reg2, Type *type) {
    // Make sure the rcx register is free
    bool should_pop = false;
    if (free_registers_[RCX_INDEX]) {
        free_registers_[RCX_INDEX] = false;
    } else {
        // Push the rcx register onto the stack
        output_file_ << "\tpushq\t%rcx\n";
        should_pop = true;
    }

    // Move the shift amount into the cl register
    output_file_ << "\tmovb\t" << b_registers[reg2] << ", %cl\n";

    // Perform the shift
    if (type->is_signed()) {
        if (type->size() <= 4) {
            output_file_ << "\tsarl\t%cl, " << d_registers[reg1] << "\n";
        } else {
            output_file_ << "\tsarq\t%cl, " << registers[reg1] << "\n";
        }
    } else {
        if (type->size() <= 4) {
            output_file_ << "\tshrl\t%cl, " << d_registers[reg1] << "\n";
        } else {
            output_file_ << "\tshrq\t%cl, " << registers[reg1] << "\n";
        }
    }

    // Restore the rcx register
    if (should_pop) {
        output_file_ << "\tpopq\t%rcx\n";
    } else {
        free_registers_[RCX_INDEX] = true;
    }

    free_register(reg2);
    return reg1;
}

int X86_CodeGenerator::compare_base(int reg1, int reg2, Type *type,
                                    std::string_view inst) {
    switch (type->size()) {
    case 1:
        output_file_ << "\tcmpb\t" << b_registers[reg2] << ", "
                     << b_registers[reg1] << "\n";
        break;
    case 4:
        output_file_ << "\tcmpl\t" << d_registers[reg2] << ", "
                     << d_registers[reg1] << "\n";
        break;
    case 8:
        output_file_ << "\tcmpq\t" << registers[reg2] << ", " << registers[reg1]
                     << "\n";
        break;
    }

    // Set the flag
    output_file_ << "\t" << inst << "\t" << b_registers[reg1] << "\n"
                 << "\tmovzbl\t" << b_registers[reg1] << ", "
                 << d_registers[reg1] << "\n";

    free_register(reg2);
    return reg1;
}

int X86_CodeGenerator::compare_equal(int reg1, int reg2, Type *type) {
    return compare_base(reg1, reg2, type, "sete");
}

int X86_CodeGenerator::compare_not_equal(int reg1, int reg2, Type *type) {
    return compare_base(reg1, reg2, type, "setne");
}

int X86_CodeGenerator::compare_less(int reg1, int reg2, Type *type) {
    return compare_base(reg1, reg2, type, "setl");
}

int X86_CodeGenerator::compare_less_equal(int reg1, int reg2, Type *type) {
    return compare_base(reg1, reg2, type, "setle");
}

int X86_CodeGenerator::compare_greater(int reg1, int reg2, Type *type) {
    return compare_base(reg1, reg2, type, "setg");
}

int X86_CodeGenerator::compare_greater_equal(int reg1, int reg2, Type *type) {
    return compare_base(reg1, reg2, type, "setge");
}

int X86_CodeGenerator::logical_not(int reg, Type *type) {
    switch (type->size()) {
    case 1:
        output_file_ << "\tcmpb\t$0, " << b_registers[reg] << "\n";
        break;
    case 4:
        output_file_ << "\tcmpl\t$0, " << d_registers[reg] << "\n";
        break;
    case 8:
        output_file_ << "\tcmpq\t$0, " << registers[reg] << "\n";
        break;
    }

    // Set the flag
    output_file_ << "\tsete\t" << b_registers[reg] << "\n"
                 << "\tmovzbl\t" << b_registers[reg] << ", " << d_registers[reg]
                 << "\n";

    return reg;
}

void X86_CodeGenerator::immediate_multiply(int reg, int val) {
    // If not power of 2, throw an error
    if (val % 2 != 0) {
        throw std::runtime_error("Immediate value must be a power of 2");
    }

    int shift = 0;
    while (val % 2 == 0) {
        val /= 2;
        shift++;
    }
    if (shift > 0) {
        output_file_ << "\tsalq\t$" << shift << ", " << registers[reg] << "\n";
    }
}

void X86_CodeGenerator::immediate_divide(int reg, int val) {
    // If not power of 2, throw an error
    if (val % 2 != 0) {
        throw std::runtime_error("Immediate value must be a power of 2");
    }
    int shift = 0;
    while (val % 2 == 0) {
        val /= 2;
        shift++;
    }
    if (shift > 0) {
        output_file_ << "\tshrq\t$" << shift << ", " << registers[reg] << "\n";
    }
}

void X86_CodeGenerator::immediate_add(int reg, int val) {
    output_file_ << "\taddq\t$" << val << ", " << registers[reg] << "\n";
}

std::string X86_CodeGenerator::variable_location(Variable *var) {
    if (variable_offsets_.contains(var)) {
        // Local variable : offset(%rbp)
        return std::to_string(variable_offsets_[var]) + "(%rbp)";
    } else {
        // Global variable : name(%rip)
        return var->name + "(%rip)";
    }
}

int X86_CodeGenerator::post_increment(Variable *var) {
    // Load the variable's value into a register
    int reg = load_variable(var);

    // Get the location of the variable
    std::string loc = variable_location(var);

    // Increment the variable
    switch (var->type->size()) {
    case 1:
        output_file_ << "\tincb\t" << loc << "\n";
        break;
    case 4:
        output_file_ << "\tincl\t" << loc << "\n";
        break;
    case 8:
        output_file_ << "\tincq\t" << loc << "\n";
        break;
    }

    return reg;
}

int X86_CodeGenerator::post_decrement(Variable *var) {
    // Load the variable's value into a register
    int reg = load_variable(var);

    // Get the location of the variable
    std::string loc = variable_location(var);

    // Decrement the variable
    switch (var->type->size()) {
    case 1:
        output_file_ << "\tdecb\t" << loc << "\n";
        break;
    case 4:
        output_file_ << "\tdecl\t" << loc << "\n";
        break;
    case 8:
        output_file_ << "\tdecq\t" << loc << "\n";
        break;
    }

    return reg;
}

int X86_CodeGenerator::pre_increment(Variable *var) {
    // Get the location of the variable
    std::string loc = variable_location(var);

    // Increment the variable
    switch (var->type->size()) {
    case 1:
        output_file_ << "\tincb\t" << loc << "\n";
        break;
    case 4:
        output_file_ << "\tincl\t" << loc << "\n";
        break;
    case 8:
        output_file_ << "\tincq\t" << loc << "\n";
        break;
    }

    // Load the variable's value into a register
    return load_variable(var);
}

int X86_CodeGenerator::pre_decrement(Variable *var) {
    // Get the location of the variable
    std::string loc = variable_location(var);

    // Decrement the variable
    switch (var->type->size()) {
    case 1:
        output_file_ << "\tdecb\t" << loc << "\n";
        break;
    case 4:
        output_file_ << "\tdecl\t" << loc << "\n";
        break;
    case 8:
        output_file_ << "\tdecq\t" << loc << "\n";
        break;
    }

    // Load the variable's value into a register
    return load_variable(var);
}

int X86_CodeGenerator::load_immediate(int val) {
    int reg = allocate_register();
    output_file_ << "\tmovq\t$" << val << ", " << registers[reg] << "\n";
    return reg;
}

int X86_CodeGenerator::load_string_literal(std::string_view str) {
    // Get the label for the string literal
    std::string label = string_literals[str.data()];

    // Load the address of the string literal into a register
    int reg = allocate_register();
    output_file_ << "\tleaq\t" << label << "(%rip), " << registers[reg] << "\n";

    return reg;
}

int X86_CodeGenerator::load_variable(Variable *var) {
    // Allocate a register
    int reg = allocate_register();

    // Get the location of the variable
    std::string loc = variable_location(var);

    // Load the variable's value into the register
    if (var->type->is_array()) {
        output_file_ << "\tleaq\t" << loc << ", " << registers[reg] << "\n";
    } else {
        switch (var->type->size()) {
        case 1:
            output_file_ << "\tmovzbl\t" << loc << ", " << d_registers[reg]
                         << "\n";
            break;
        case 4:
            output_file_ << "\tmovl\t" << loc << ", " << d_registers[reg]
                         << "\n";
            break;
        case 8:
            output_file_ << "\tmovq\t" << loc << ", " << registers[reg] << "\n";
            break;
        }
    }

    return reg;
}

int X86_CodeGenerator::load_variable_address(Variable *var) {
    // Allocate a register
    int reg = allocate_register();

    // Get the location of the variable
    std::string loc = variable_location(var);

    // Load the address of the variable into the register
    output_file_ << "\tleaq\t" << loc << ", " << registers[reg] << "\n";

    return reg;
}

void X86_CodeGenerator::move_immediate(int reg, int val) {
    output_file_ << "\tmovq\t$" << val << ", " << registers[reg] << "\n";
}

void X86_CodeGenerator::move_register(int reg, Variable *var) {
    // Get the location of the variable
    std::string loc = variable_location(var);

    // Move the register's value into the variable
    if (var->type->is_array()) {
        output_file_ << "\tmovq\t" << registers[reg] << ", " << loc << "\n";
    } else {
        switch (var->type->size()) {
        case 1:
            output_file_ << "\tmovb\t" << b_registers[reg] << ", " << loc
                         << "\n";
            break;
        case 4:
            output_file_ << "\tmovl\t" << d_registers[reg] << ", " << loc
                         << "\n";
            break;
        case 8:
            output_file_ << "\tmovq\t" << registers[reg] << ", " << loc << "\n";
            break;
        }
    }

    free_register(reg);
}

void X86_CodeGenerator::move_register(int reg, int address_reg,
                                      Type *data_type) {
    // Move the register's value into the address
    switch (data_type->size()) {
    case 1:
        output_file_ << "\tmovb\t" << b_registers[reg] << ", ("
                     << registers[address_reg] << ")\n";
        break;
    case 4:
        output_file_ << "\tmovl\t" << d_registers[reg] << ", ("
                     << registers[address_reg] << ")\n";
        break;
    case 8:
        output_file_ << "\tmovq\t" << registers[reg] << ", ("
                     << registers[address_reg] << ")\n";
        break;
    }

    free_register(reg);
    free_register(address_reg);
}

int X86_CodeGenerator::load_from_memory(int address_reg, Type *data_type) {
    // Allocate a register
    int reg = allocate_register();

    // Load the value from the address into the register
    switch (data_type->size()) {
    case 1:
        output_file_ << "\tmovzbl\t(" << registers[address_reg] << "), "
                     << d_registers[reg] << "\n";
        break;
    case 4:
        output_file_ << "\tmovl\t(" << registers[address_reg] << "), "
                     << d_registers[reg] << "\n";
        break;
    case 8:
        output_file_ << "\tmovq\t(" << registers[address_reg] << "), "
                     << registers[reg] << "\n";
        break;
    }

    free_register(address_reg);
    return reg;
}

int X86_CodeGenerator::duplicate_register(int reg) {
    // Allocate a register
    int new_reg = allocate_register();

    // Duplicate the register's value
    output_file_ << "\tmovq\t" << registers[reg] << ", " << registers[new_reg]
                 << "\n";

    return new_reg;
}

void X86_CodeGenerator::move_to_argument(int reg, int n) {
    if (n <= 6) {
        output_file_ << "\tmovq\t" << registers[reg] << ", "
                     << registers[NUM_REGISTERS - n] << "\n";
    } else {
        output_file_ << "\tpushq\t" << registers[reg] << "\n";
    }

    free_register(reg);
}

int X86_CodeGenerator::call_function(std::string_view name) {
    // Call the function
    output_file_ << "\tcall\t" << name << "\n";

    // Remove the arguments in the stack
    FunctionPrototype *func = FunctionManager::find(name.data());
    if (func->parameters_.size() > 6) {
        output_file_ << "\taddq\t$" << 8 * (func->parameters_.size() - 6)
                     << ", %rsp\n";
    }

    // If the function returns void, return -1
    if (func->return_type_->is_void()) {
        return -1;
    }

    // Allocate a register
    int reg = allocate_register();
    output_file_ << "\tmovq\t%rax, " << registers[reg] << "\n";
    return reg;
}

void X86_CodeGenerator::prelude() {
    // Free all the registers
    free_all_registers();

    output_file_ << "\t.text\n";

    // Generate the string literals
    for (auto &[str, label] : string_literals) {
        label = allocate_label();

        // Unescape the string
        std::string str_literal;
        for (size_t i = 0; i < str.size(); ++i) {
            switch (str[i]) {
            case '\a':
                str_literal += R"(\a)";
                break;
            case '\b':
                str_literal += R"(\b)";
                break;
            case '\f':
                str_literal += R"(\f)";
                break;
            case '\n':
                str_literal += R"(\n)";
                break;
            case '\r':
                str_literal += R"(\r)";
                break;
            case '\t':
                str_literal += R"(\t)";
                break;
            case '\v':
                str_literal += R"(\v)";
                break;
            case '\\':
                str_literal += R"(\\)";
                break;
            case '\'':
                str_literal += R"(\')";
                break;
            case '\"':
                str_literal += R"(\")";
                break;
            default:
                str_literal += str[i];
                break;
            }
        }

        allocate_string_literal(str_literal, label);
    }

    // Generate the global variables
    std::vector<Variable *> global_vars =
        VariableManager::get_variables_in_scope("global");
    for (auto var : global_vars) {
        allocate_global_variables(var);
    }
}

void X86_CodeGenerator::postlude() {
    // Close the output file
    output_file_.close();
}

void X86_CodeGenerator::allocate_string_literal(std::string_view str,
                                                std::string_view label) {
    output_file_ << "\t.section\t.rodata\n"
                 << label << ":\n"
                 << "\t.string\t\"" << str << "\"\n";
}

void X86_CodeGenerator::allocate_global_variables(Variable *var) {
    // Get the size
    int num_elements = 1;
    int size_element = 0;

    // Handle array
    if (var->type->is_array()) {
        num_elements = dynamic_cast<ArrayType *>(var->type)->num_elements();
        size_element = dynamic_cast<ArrayType *>(var->type)->element()->size();
    } else {
        size_element = var->type->size();
    }

    // Allocate space for the variable
    output_file_ << "\t.data\n"
                 << "\t.globl\t" << var->name << "\n"
                 << var->name << ":\n";
    for (int i = 0; i < num_elements; ++i) {
        switch (size_element) {
        case 1:
            output_file_ << "\t.byte\t0\n";
            break;
        case 4:
            output_file_ << "\t.long\t0\n";
            break;
        case 8:
            output_file_ << "\t.quad\t0\n";
            break;
        }
    }
}

std::string X86_CodeGenerator::get_reg_by_size(int reg, int size) {
    switch (size) {
    case 1:
        return b_registers[reg];
    case 4:
        return d_registers[reg];
    case 8:
        return registers[reg];
    default:
        throw std::runtime_error("Invalid size");
    }
}

int X86_CodeGenerator::allocate_register() {
    for (int i = 0; i < NUM_REGISTERS; ++i) {
        if (free_registers_[i]) {
            free_registers_[i] = false;
            return i;
        }
    }
    throw std::runtime_error("No free register");
}

void X86_CodeGenerator::free_all_registers() {
    for (int i = 0; i < NUM_REGISTERS; ++i) {
        free_registers_[i] = true;
    }
}

char X86_CodeGenerator::get_suffix_by_size(int size) {
    switch (size) {
    case 1:
        return 'b';
    case 2:
        return 'w';
    case 4:
        return 'l';
    case 8:
        return 'q';
    default:
        throw std::runtime_error("Invalid size");
    }
}

} // namespace myComp