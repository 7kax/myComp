#ifndef MYCOMP_CODEGENERATOR_H
#define MYCOMP_CODEGENERATOR_H

#include <vector>

#include "Variable.h"

namespace myComp {
class CodeGenerator {
  public:
    virtual ~CodeGenerator() = default;

    // Set the output file
    virtual void set_output(std::string_view filename) = 0;

    // Prelude and postlude of the code
    virtual void prelude() = 0;
    virtual void postlude() = 0;

    // Manually release the register
    virtual void free_register(int reg) = 0;
    virtual void free_all_registers() = 0;

    // Generate function prelude
    // Mark the start of a function(record the function name)
    // Initialize stack size = 0
    // Set the end label
    virtual void function_prelude(std::string_view name) = 0;

    // Generate end label
    // Mark the end of a function
    // Restore stack pointer
    // Clear the offset map
    virtual void function_postlude() = 0;

    // Load parameters into stack
    // Increment stack size
    // Record the variables and their offsets
    virtual void load_parameters(const std::vector<Variable *> &params) = 0;

    // Allocate and record a string literal
    virtual void allocate_string_literal(std::string_view str,
                                         std::string_view label) = 0;

    // Allocate space for a global variable
    virtual void allocate_global_variables(Variable *var) = 0;

    // Allocate space for local variables(parameters excluded)
    // Increment stack size
    // Record the variables and their offsets
    // Allocate space on the stack for the local variables
    virtual void
    allocate_local_variables(const std::vector<Variable *> &variables) = 0;

    // Allocate a label and return its name
    virtual std::string allocate_label() = 0;

    // Add a label
    virtual void add_label(std::string_view label) = 0;

    // Jump
    virtual void jump_on_zero(int reg, std::string_view label) = 0;
    virtual void jump_on_non_zero(int reg, std::string_view label) = 0;
    virtual void jump(std::string_view label) = 0;

    // Move value in register
    // Jump to the end label
    virtual void return_from_function(int reg) = 0;

    // Type cast in place
    virtual void type_cast(int reg, Type *src, Type *dest) = 0;

    // Perform an arithmetic operation, parameter `type` indicates the type of
    // the result
    // Return the register number
    virtual int negate(int reg, Type *type) = 0;
    virtual int add(int reg1, int reg2, Type *type) = 0;
    virtual int subtract(int reg1, int reg2, Type *type) = 0;
    virtual int multiply(int reg1, int reg2, Type *type) = 0;
    virtual int divide(int reg1, int reg2, Type *type) = 0;
    virtual int modulo(int reg1, int reg2, Type *type) = 0;
    virtual int bitwise_not(int reg, Type *type) = 0;
    virtual int bitwise_or(int reg1, int reg2, Type *type) = 0;
    virtual int bitwise_and(int reg1, int reg2, Type *type) = 0;
    virtual int bitwise_xor(int reg1, int reg2, Type *type) = 0;
    virtual int left_shift(int reg1, int reg2, Type *type) = 0;
    virtual int right_shift(int reg1, int reg2, Type *type) = 0;

    // Perform a comparison, parameter `type` indicates the type of the operands
    // Return the register number
    virtual int compare_equal(int reg1, int reg2, Type *type) = 0;
    virtual int compare_not_equal(int reg1, int reg2, Type *type) = 0;
    virtual int compare_less(int reg1, int reg2, Type *type) = 0;
    virtual int compare_less_equal(int reg1, int reg2, Type *type) = 0;
    virtual int compare_greater(int reg1, int reg2, Type *type) = 0;
    virtual int compare_greater_equal(int reg1, int reg2, Type *type) = 0;

    virtual int logical_not(int reg, Type *type) = 0;

    // Multiply/divide a register by an immediate value in place
    // Immediate value must be a power of 2
    virtual void immediate_multiply(int reg, int val) = 0;
    virtual void immediate_divide(int reg, int val) = 0;

    // Add an immediate value to a register
    virtual void immediate_add(int reg, int val) = 0;

    virtual int post_increment(Variable *var) = 0;
    virtual int post_decrement(Variable *var) = 0;
    virtual int pre_increment(Variable *var) = 0;
    virtual int pre_decrement(Variable *var) = 0;

    // Load an immediate value into a register
    // Return the register number
    virtual int load_immediate(int val) = 0;

    // Load address of string literal into a register
    // Return the register number
    virtual int load_string_literal(std::string_view str) = 0;

    // Load a variable's value into a register
    // Return the register number
    virtual int load_variable(Variable *var) = 0;

    // Load a variable's address into a register
    // Return the register number
    virtual int load_variable_address(Variable *var) = 0;

    // Move an immediate value into a register
    // The register will not be released
    virtual void move_immediate(int reg, int val) = 0;

    // Move a register's value into a variable
    virtual void move_register(int reg, Variable *var) = 0;

    // Move a register's value into an address
    virtual void move_register(int reg, int address_reg, Type *data_type) = 0;

    // Load a value from an address in reg to another register
    // Return the register number
    virtual int load_from_memory(int address_reg, Type *data_type) = 0;

    // Duplicate a register's value
    // Return the register number
    virtual int duplicate_register(int reg) = 0;

    // Move a register's value as nth argument
    virtual void move_to_argument(int reg, int n) = 0;

    // Call a function
    // Return the register number
    virtual int call_function(std::string_view name) = 0;

  private:
};
} // namespace myComp

#endif // MYCOMP_CODEGENERATOR_H
