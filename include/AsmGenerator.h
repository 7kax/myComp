#ifndef MYCOMP_ASMGENERATOR_H
#define MYCOMP_ASMGENERATOR_H

#include "defs.h"

namespace myComp {
// This an interface for asm generators
class AsmGenerator {
public:
  // Necessary virtual destructor
  virtual ~AsmGenerator() = default;

  // Set the output file
  virtual void set_output(std::string_view filename) = 0;

  // prelude and postlude
  // prelude of the program
  virtual void prelude() = 0;

  // postlude of the program
  virtual void postlude() = 0;

  // prelude of a function
  virtual void function_prelude(std::string_view name) = 0;

  // postlude of a function
  virtual void function_postlude(std::string_view name, int offset) = 0;

  // Register management
  // free all registers
  virtual void free_all_registers() = 0;

  // allocate a register and return its number
  virtual int allocate_register() = 0;

  // free a given register
  virtual void free_register(int reg) = 0;

  // widen a register and return its number
  virtual int widen_integer(int reg, Type_ *src, Type_ *dest) = 0;

  // scale a register and return its number
  virtual int scale_integer(int reg, int val) = 0;

  // Memory management
  // load an integer literal into a register and return its number
  virtual int load_integer(int value) = 0;

  // load a string literal into a register and return its number
  virtual int load_string(std::string_view label) = 0;

  // Allocate space for parameters on stack and return the offset
  virtual int move_parameters(const std::vector<parameter> &params) = 0;

  // Move the stack pointer by a given amount to allocate space for local
  // variables
  virtual void allocate_stack(int amount) = 0;

  // allocate a symbol
  virtual void allocate_global_symbol(std::string_view name) = 0;

  // allocate a string literal
  virtual void allocate_string_literal(std::string_view str,
                                       std::string_view label) = 0;

  // load a symbol's value into a register and return its number
  virtual int load_symbol(std::string_view name) = 0;

  // Store a register's value into a symbol
  virtual int store_symbol(int reg, std::string_view name) = 0;

  // Store a register's value into an address
  // virtual int store_address(int address_reg, int val_reg,
  //                           DataType data_type) = 0;

  // load a symbol's address into a register and return its number
  virtual int load_address(std::string_view name) = 0;

  // dereference a pointer and return its value
  virtual int dereference(int reg, Type_ *data_type) = 0;

  // dereference a pointer with an offset and return its value
  virtual int dereference(int address_reg, int offset_reg,
                          Type_ *data_type) = 0;

  // Copy a register's value into where an argument is expected
  virtual void copy_argument(int src, int) = 0;

  // Arithmetic operations
  virtual int add(int r1, int r2) = 0;

  virtual int sub(int r1, int r2) = 0;

  virtual int mul(int r1, int r2) = 0;

  virtual int div(int r1, int r2) = 0;

  virtual int equal(int r1, int r2) = 0;

  virtual int not_equal(int r1, int r2) = 0;

  virtual int less(int r1, int r2) = 0;

  virtual int less_equal(int r1, int r2) = 0;

  virtual int greater(int r1, int r2) = 0;

  virtual int greater_equal(int r1, int r2) = 0;

  virtual int assign(std::string_view name, int reg) = 0;

  virtual int assign(int address_reg, int val_reg, Type_ *data_type) = 0;

  virtual int assign(int address_reg, int offset_reg, int val_reg,
                     Type_ *data_type) = 0;

  virtual int negative(int reg) = 0;

  virtual int invert(int reg) = 0;

  virtual int NOT(int reg) = 0;

  virtual int pre_increment(std::string_view name) = 0;

  virtual int pre_decrement(std::string_view name) = 0;

  virtual int post_increment(std::string_view name) = 0;

  virtual int post_decrement(std::string_view name) = 0;

  virtual int OR(int r1, int r2) = 0;

  virtual int AND(int r1, int r2) = 0;

  virtual int XOR(int r1, int r2) = 0;

  virtual int logical_or(int r1, int r2) = 0;

  virtual int logical_and(int r1, int r2) = 0;

  virtual int left_shift(int r1, int r2) = 0;

  virtual int right_shift(int r1, int r2) = 0;

  // Control flow operations
  virtual void jump(std::string_view label) = 0;

  virtual void jump_zero(int reg, std::string_view label) = 0;

  virtual void label(std::string_view label) = 0;

  virtual int call(std::string_view name) = 0;

  virtual void RETURN(int reg) = 0;
};
} // namespace myComp

#endif // MYCOMP_ASMGENERATOR_H
