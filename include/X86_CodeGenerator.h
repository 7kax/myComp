#ifndef MYCOMP_X86_CODEGENERATOR_H
#define MYCOMP_X86_CODEGENERATOR_H

#include <array>
#include <fstream>
#include <string_view>

#include "CodeGenerator.h"

namespace myComp {
class X86_CodeGenerator final : public CodeGenerator {
  public:
    // Set the output file
    void set_output(std::string_view filename) override;

    // Override the virtual functions
    void prelude() override;
    void postlude() override;
    void free_register(int reg) override;
    void free_all_registers() override;
    void function_prelude(std::string_view name) override;
    void function_postlude() override;
    void load_parameters(const std::vector<Variable *> &params) override;
    void allocate_string_literal(std::string_view str,
                                 std::string_view label) override;
    void allocate_global_variables(Variable *var) override;
    void
    allocate_local_variables(const std::vector<Variable *> &variables) override;
    std::string allocate_label() override;
    void add_label(std::string_view label) override;
    void jump_on_zero(int reg, std::string_view label) override;
    void jump_on_non_zero(int reg, std::string_view label) override;
    void jump(std::string_view label) override;
    void return_from_function(int reg) override;
    void type_cast(int reg, Type *src, Type *dest) override;
    int negate(int reg, Type *type) override;
    int add(int reg1, int reg2, Type *type) override;
    int subtract(int reg1, int reg2, Type *type) override;
    int multiply(int reg1, int reg2, Type *type) override;
    int divide(int reg1, int reg2, Type *type) override;
    int modulo(int reg1, int reg2, Type *type) override;
    int bitwise_not(int reg, Type *type) override;
    int bitwise_or(int reg1, int reg2, Type *type) override;
    int bitwise_and(int reg1, int reg2, Type *type) override;
    int bitwise_xor(int reg1, int reg2, Type *type) override;
    int left_shift(int reg1, int reg2, Type *type) override;
    int right_shift(int reg1, int reg2, Type *type) override;
    int compare_equal(int reg1, int reg2, Type *type) override;
    int compare_not_equal(int reg1, int reg2, Type *type) override;
    int compare_less(int reg1, int reg2, Type *type) override;
    int compare_less_equal(int reg1, int reg2, Type *type) override;
    int compare_greater(int reg1, int reg2, Type *type) override;
    int compare_greater_equal(int reg1, int reg2, Type *type) override;
    int logical_not(int reg, Type *type) override;
    void immediate_multiply(int reg, int val) override;
    void immediate_divide(int reg, int val) override;
    void immediate_add(int reg, int val) override;
    int post_increment(Variable *var) override;
    int post_decrement(Variable *var) override;
    int pre_increment(Variable *var) override;
    int pre_decrement(Variable *var) override;
    int load_immediate(int val) override;
    int load_string_literal(std::string_view str) override;
    int load_variable(Variable *var) override;
    int load_variable_address(Variable *var) override;
    void move_immediate(int reg, int val) override;
    void move_register(int reg, Variable *var) override;
    void move_register(int reg, int address_reg, Type *data_type) override;
    int load_from_memory(int address_reg, Type *data_type) override;
    int duplicate_register(int reg) override;
    void move_to_argument(int reg, int n) override;
    int call_function(std::string_view name) override;

  private:
    static constexpr int NUM_REGISTERS = 10;
    static constexpr int RDX_INDEX = 7;
    static constexpr int RCX_INDEX = 6;
    static constexpr std::array<const char *, NUM_REGISTERS> registers{
        "%r10", "%r11", "%r12", "%r13", "%r9",
        "%r8",  "%rcx", "%rdx", "%rsi", "%rdi"};
    static constexpr std::array<const char *, NUM_REGISTERS> b_registers{
        "%r10b", "%r11b", "%r12b", "%r13b", "%r9b",
        "%r8b",  "%cl",   "%dl",   "%sil",  "%dil"};
    static constexpr std::array<const char *, NUM_REGISTERS> d_registers{
        "%r10d", "%r11d", "%r12d", "%r13d", "%r9d",
        "%r8d",  "%ecx",  "%edx",  "%esi",  "%edi"};

    // Output file
    std::ofstream output_file_;

    // Register allocation
    std::array<bool, NUM_REGISTERS> free_registers_{
        true, true, true, true, true, true, true, true, true, true};

    // Tell if we are in a function
    bool in_function_ = false;

    // Function name of current function
    std::string function_name_;

    // Stack size of current function
    int stack_size_ = 0;

    // End label of current function
    std::string end_label_;

    // Variable offsets of current function
    std::unordered_map<Variable *, int> variable_offsets_;

    // Label counter for generating unique labels
    int label_count_ = 0;

    // Get reg by size
    std::string get_reg_by_size(int reg, int size);

    // Allocate a register
    int allocate_register();

    // Get suffix by size
    char get_suffix_by_size(int size);

    // Base of the comparison
    int compare_base(int reg1, int reg2, Type *type, std::string_view inst);

    // Get the location of a variable
    std::string variable_location(Variable *var);
};
} // namespace myComp

#endif