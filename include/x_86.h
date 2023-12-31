#ifndef MYCOMP_X_86_H
#define MYCOMP_X_86_H

#include "defs.h"

#include "AsmGenerator.h"

namespace myComp {
    class x_86 : public AsmGenerator {
    private:
        // The output file
        std::ofstream output;

        // List of registers
        static const std::vector<std::string> registers;

        // Number of registers
        static constexpr int num_registers = 10;

        // List of b-register
        static const std::vector<std::string> b_registers;

        // List of d-register
        static const std::vector<std::string> d_registers;

        // List of available registers
        std::vector<bool> free_registers;

        // Compare 2 registers
        int compare_registers(int r1, int r2, const std::string &op);

        // Get the location of a symbol
        static std::string variable_location(const std::string &identifier);

    public:
        int move_parameters(const std::vector<parameter> &params) override;

        // Constructor
        x_86() : free_registers(num_registers, true) {}

        // Set the output file
        void set_output(const std::string &filename) override;

        // prelude and postlude
        void prelude() override;

        void postlude() override;

        void function_prelude(const std::string &name) override;

        void function_postlude(const std::string &name, int offset) override;

        void allocate_stack(int amount) override;

        // Register management
        void free_all_registers() override;

        int allocate_register() override;

        void free_register(int reg) override;

        void copy_argument(int src, int num_arg) override;

        int widen_integer(int reg, DataType src, DataType dest) override;

        int scale_integer(int reg, int val) override;

        // Memory management
        int load_integer(int value) override;

        int load_string(const std::string &label) override;

        int load_symbol(const std::string &name) override;

        void allocate_global_symbol(const std::string &name) override;

        int invert(int reg) override;

        int NOT(int reg) override;

        int post_increment(const std::string &name) override;

        int pre_increment(const std::string &name) override;

        int pre_decrement(const std::string &name) override;

        int negative(int reg) override;

        int post_decrement(const std::string &name) override;

        int OR(int r1, int r2) override;

        int AND(int r1, int r2) override;

        int XOR(int r1, int r2) override;

        int logical_or(int r1, int r2) override;

        int logical_and(int r1, int r2) override;

        int left_shift(int r1, int r2) override;

        int right_shift(int r1, int r2) override;

        void allocate_string_literal(const std::string &str, const std::string &label) override;

        int store_symbol(int reg, const std::string &name) override;

        int store_address(int address_reg, int val_reg, DataType data_type) override;

        int load_address(const std::string &name) override;

        int dereference(int reg, DataType data_type) override;

        int dereference(int address_reg, int offset_reg, DataType data_type) override;

        // Arithmetic operations
        int add(int r1, int r2) override;

        int sub(int r1, int r2) override;

        int mul(int r1, int r2) override;

        int div(int r1, int r2) override;

        int equal(int r1, int r2) override { return this->compare_registers(r1, r2, "sete"); }

        int not_equal(int r1, int r2) override { return this->compare_registers(r1, r2, "setne"); }

        int less(int r1, int r2) override { return this->compare_registers(r1, r2, "setl"); }

        int less_equal(int r1, int r2) override { return this->compare_registers(r1, r2, "setle"); }

        int greater(int r1, int r2) override { return this->compare_registers(r1, r2, "setg"); }

        int greater_equal(int r1, int r2) override { return this->compare_registers(r1, r2, "setge"); }

        int assign(const std::string &name, int reg) override;

        int assign(int address_reg, int val_reg, DataType data_type) override;

        int assign(int address_reg, int offset_reg, int val_reg, DataType data_type) override;

        // Control flow operations
        void jump(const std::string &label) override;

        void jump_zero(int reg, const std::string &label) override;

        void label(const std::string &label) override;

        int call(const std::string &name) override;

        void RETURN(int reg) override;

        // Destructor: close the output file
        ~x_86() override { output.close(); }

    };
}


#endif //MYCOMP_X_86_H
