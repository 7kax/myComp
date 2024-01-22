#include "defs.h"
#include "objects.h"
#include "data.h"

namespace myComp {
  void Generator::generate_preamble() {
    // Generate the preamble
    asm_generator->prelude();
    // Generate the string literals
    for (auto &str: string_literals) {
      str.second = Generator::get_label();
      asm_generator->allocate_string_literal(str.first, str.second);
    }
    // Allocate space for global variables
    for (const auto &[name, symbol]: symbol_tables["global"].get_symbol_table())
      asm_generator->allocate_global_symbol(name);
  }

  void Generator::generate_postamble() { asm_generator->postlude(); }

  void Generator::generate(const ASTNode *node) {
    if (node->type == ASTNodeType::FUNCTION_DECLARATION)
      Generator::function_declaration(node);
    else if (node->type == ASTNodeType::VARIABLE_DECLARATION)
      Generator::variable_declaration(node);
    else
      Errors::fatal_error("got unexpected AST node type " +
                          std::string(ASTNode_str.at(node->type)));
  }

  void Generator::function_declaration(const ASTNode *node) {
    const std::string &function_name = node->string;

    // Set the context to function
    context.push(function_name, Generator::get_label());

    asm_generator->function_prelude(function_name);

    // Move the parameters in registers to the stack
    const std::vector<parameter> &parameters =
        function.get_prototype(function_name).parameters;
    this->offset = asm_generator->move_parameters(parameters);

    // Allocate space for local variables
    for (auto &[name, symbol]: symbol_tables[function_name].get_symbol_table()) {
      // Skip the parameters
      if (symbol.offset != 0)
        continue;
      // int length = Type::data_size.at(symbol.data_type) * symbol.size;
      int length = symbol.type->size();
      length = (length / 16 + 1) * 16;
      this->offset -= length;
      symbol_tables[function_name].set_offset(name, this->offset);
    }
    asm_generator->allocate_stack(-this->offset);

    Generator::code_block(node->left);
    asm_generator->function_postlude(function_name, offset);

    // Pop the context
    context.pop();
  }

  void Generator::code_block(const ASTNode *node) {
    Generator::statement(node->left);

    if (node->right != nullptr)
      Generator::code_block(node->right);
  }

  void Generator::statement(const ASTNode *node) {
    switch (node->type) {
      case ASTNodeType::IF:
        if (node->right->type != ASTNodeType::ELSE)
          Generator::IF(node);
        else
          Generator::if_else(node);
        break;
      case ASTNodeType::WHILE:
        Generator::WHILE(node);
        break;
      case ASTNodeType::FOR:
        Generator::FOR(node);
        break;
      case ASTNodeType::VARIABLE_DECLARATION:
        Generator::variable_declaration(node);
        break;
      case ASTNodeType::RETURN:
        Generator::RETURN(node);
        break;
      default:
        Generator::expression(node);
    }
    asm_generator->free_all_registers();
  }

  void Generator::IF(const ASTNode *node) {
    std::string if_label = get_label();

    int reg = Generator::expression(node->left);
    asm_generator->jump_zero(reg, if_label);
    Generator::code_block(node->right);
    asm_generator->label(if_label);
  }

  void Generator::if_else(const ASTNode *node) {
    std::string if_label = get_label();
    std::string else_label = get_label();

    int reg = Generator::expression(node->left);
    asm_generator->jump_zero(reg, if_label);
    Generator::code_block(node->right->left);
    asm_generator->jump(else_label);
    asm_generator->label(if_label);
    Generator::code_block(node->right->right);
    asm_generator->label(else_label);
  }

  void Generator::WHILE(const ASTNode *node) {
    std::string start_label = get_label();
    std::string end_label = get_label();

    asm_generator->label(start_label);
    int reg = Generator::expression(node->left);
    asm_generator->jump_zero(reg, end_label);
    Generator::code_block(node->right);
    asm_generator->jump(start_label);
    asm_generator->label(end_label);
  }

  void Generator::FOR(const ASTNode *node) {
    std::string start_label = get_label();
    std::string end_label = get_label();

    Generator::expression(node->left->left);
    asm_generator->label(start_label);
    int reg = Generator::expression(node->left->right);
    asm_generator->jump_zero(reg, end_label);
    Generator::code_block(node->right->right);
    Generator::expression(node->right->left);
    asm_generator->jump(start_label);
    asm_generator->label(end_label);
  }

  void Generator::variable_declaration(const ASTNode *node) {
  }

  void Generator::RETURN(const ASTNode *node) {
    if (context.get_type() != ContextType::FUNCTION)
      Errors::fatal_error("return statement not in function");

    int reg = Generator::expression(node->left);

    asm_generator->RETURN(reg);
  }

  int Generator::expression(const ASTNode *node) {
    ASTNodeType node_type = node->type;
    switch (node_type) {
      case ASTNodeType::FUNCTION_CALL:
        return Generator::call(node);
      case ASTNodeType::ASSIGN:
        return Generator::assign(node);
      case ASTNodeType::ADDRESS:
        return asm_generator->load_address(node->string);
      case ASTNodeType::DEREFERENCE:
        if (node->right == nullptr)
          return asm_generator->dereference(Generator::expression(node->left),
                                            node->data_type);
        else
          return asm_generator->dereference(Generator::expression(node->left),
                                            Generator::expression(node->right),
                                            node->data_type);
      case ASTNodeType::NEGATIVE:
        return asm_generator->negative(Generator::expression(node->left));
      case ASTNodeType::INVERT:
        return asm_generator->invert(Generator::expression(node->left));
      case ASTNodeType::NOT:
        return asm_generator->NOT(Generator::expression(node->left));
      case ASTNodeType::PRE_INC:
        return asm_generator->pre_increment(node->left->string);
      case ASTNodeType::PRE_DEC:
        return asm_generator->pre_decrement(node->left->string);
      case ASTNodeType::POST_INC:
        return asm_generator->post_increment(node->left->string);
      case ASTNodeType::POST_DEC:
        return asm_generator->post_decrement(node->left->string);
      default:
        break;
    }

    int left_reg, right_reg;
    if (node->left != nullptr)
      left_reg = Generator::expression(node->left);
    if (node->right != nullptr)
      right_reg = Generator::expression(node->right);

    switch (node_type) {
      case ASTNodeType::ADD:
        return asm_generator->add(left_reg, right_reg);
      case ASTNodeType::SUBTRACT:
        return asm_generator->sub(left_reg, right_reg);
      case ASTNodeType::MULTIPLY:
        return asm_generator->mul(left_reg, right_reg);
      case ASTNodeType::DIVIDE:
        return asm_generator->div(left_reg, right_reg);
      case ASTNodeType::EQUALS:
        return asm_generator->equal(left_reg, right_reg);
      case ASTNodeType::NEQ:
        return asm_generator->not_equal(left_reg, right_reg);
      case ASTNodeType::LESS:
        return asm_generator->less(left_reg, right_reg);
      case ASTNodeType::GREATER:
        return asm_generator->greater(left_reg, right_reg);
      case ASTNodeType::LESS_EQ:
        return asm_generator->less_equal(left_reg, right_reg);
      case ASTNodeType::GREATER_EQ:
        return asm_generator->greater_equal(left_reg, right_reg);
      case ASTNodeType::WIDEN:
        return asm_generator->widen_integer(left_reg, node->left->data_type,
                                            node->data_type);
      case ASTNodeType::SCALE:
        return asm_generator->scale_integer(left_reg, node->val);
      case ASTNodeType::VARIABLE:
        return asm_generator->load_symbol(node->string);
      case ASTNodeType::INT_LITERAL:
        return asm_generator->load_integer(node->val);
      case ASTNodeType::STRING_LITERAL:
        return asm_generator->load_string(string_literals[node->string]);
      case ASTNodeType::OR:
        return asm_generator->OR(left_reg, right_reg);
      case ASTNodeType::LOGICAL_OR:
        return asm_generator->logical_or(left_reg, right_reg);
      case ASTNodeType::AND:
        return asm_generator->AND(left_reg, right_reg);
      case ASTNodeType::LOGICAL_AND:
        return asm_generator->logical_and(left_reg, right_reg);
      case ASTNodeType::XOR:
        return asm_generator->XOR(left_reg, right_reg);
      case ASTNodeType::L_SHIFT:
        return asm_generator->left_shift(left_reg, right_reg);
      case ASTNodeType::R_SHIFT:
        return asm_generator->right_shift(left_reg, right_reg);
      default:
        Errors::fatal_error("got unexpected AST node node_type for expression: " +
                            std::string(ASTNode_str.at(node_type)));
    }
    // This is unreachable
    return -1;
  }

  int Generator::call(const ASTNode *node) {
    ASTNode *param = node->left;

    // Calculate the number of arguments
    std::stack<ASTNode *> params;
    while (param != nullptr) {
      params.push(param->right);
      param = param->left;
    }

    // Push the arguments
    while (!params.empty()) {
      int reg = Generator::expression(params.top());
      asm_generator->copy_argument(reg, static_cast<int>(params.size()));
      params.pop();
    }

    // Call the function
    return asm_generator->call(node->string);
  }

  int Generator::assign(const ASTNode *node) {
    if (node->left->type == ASTNodeType::VARIABLE)
      return asm_generator->assign(node->left->string,
                                   Generator::expression(node->right));
    if (node->left->type == ASTNodeType::DEREFERENCE) {
      if (node->left->right == nullptr)
        return asm_generator->assign(Generator::expression(node->left->left),
                                     Generator::expression(node->right),
                                     node->left->data_type);
      else
        return asm_generator->assign(Generator::expression(node->left->left),
                                     Generator::expression(node->left->right),
                                     Generator::expression(node->right),
                                     node->left->data_type);
    }
    return -1;
  }
} // namespace myComp
