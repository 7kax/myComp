#include "Parser.h"
#include "Context.h"
#include "data.h"

namespace myComp {
ASTNode_ *Parser::build_tree() {
    Type *data_type = token_processor_->next_data_type();
    std::string identifier = token_processor_->next_identifier();

    if (token_processor_->peek_type() != TokenType::LPAREN) {
        return variable_declaration(data_type, identifier);
    } else
        return function_declaration(data_type, identifier);
}

// todo: optimize this function
VariableDeclarationNode *Parser::variable_declaration(Type *data_type,
                                                      std::string &name) {
    if (token_processor_->peek_type() == TokenType::LBRACKET) {

        token_processor_->lbracket();
        int size = static_cast<int>(token_processor_->next_integer());
        token_processor_->rbracket();

        Type *array_type = TypeFactory::get_array(data_type, size);
        VariableManager::insert(array_type, name, Context::get_name());
    } else {
        VariableManager::insert(data_type, name, Context::get_name());
    }
    while (token_processor_->peek_type() == TokenType::COMMA) {
        token_processor_->comma();

        name = token_processor_->next_identifier();

        if (token_processor_->peek_type() == TokenType::LBRACKET) {

            token_processor_->lbracket();
            int size = static_cast<int>(token_processor_->next_integer());
            token_processor_->rbracket();

            Type *array_type = TypeFactory::get_array(data_type, size);
            VariableManager::insert(array_type, name, Context::get_name());
        } else {
            VariableManager::insert(data_type, name, Context::get_name());
        }
    }
    token_processor_->semi();

    // Do not support initialization on declaration

    return new VariableDeclarationNode(nullptr, nullptr);
}

VariableDeclarationNode *Parser::variable_declaration() {
    Type *data_type = token_processor_->next_data_type();

    // Build the tree
    while (token_processor_->peek_type() != TokenType::SEMI) {
        std::string name = token_processor_->next_identifier();
        if (token_processor_->peek_type() == TokenType::LBRACKET) {
            token_processor_->lbracket();
            int size = static_cast<int>(token_processor_->next_integer());
            token_processor_->rbracket();

            Type *array_type = TypeFactory::get_array(data_type, size);
            VariableManager::insert(array_type, name, Context::get_name());
        } else {
            VariableManager::insert(data_type, name, Context::get_name());
        }
        if (token_processor_->peek_type() != TokenType::SEMI)
            token_processor_->comma();
    }

    // Do not support initialization on declaration

    return new VariableDeclarationNode(nullptr, nullptr);
}

FunctionDefinitionNode *Parser::function_declaration(Type *return_type,
                                                     const std::string &name) {
    // Set the context
    Context::push(name);

    // Tell if the function is already declared
    bool declared = FunctionManager::exists(name);
    FunctionPrototype *prototype = FunctionManager::find(name);

    // If the function is already declared, check if the return type matches
    if (declared && return_type != prototype->return_type_) {
        throw std::runtime_error("return type mismatch in function " + name);
    }

    // Fetch the parameter list
    token_processor_->lparen();

    // If the function is already declared, check if the parameter list matches
    // Fill in the name of the parameters
    if (declared) {
        // Update the parameter list
        for (auto &parameters = prototype->parameters_;
             auto &parameter : parameters) {
            Type *type = parameter->type;

            // Check if the parameter type matches
            if (token_processor_->next_data_type() != type) {
                throw std::runtime_error(
                    "parameter type mismatch in function " + name);
            }

            // If the parameter has a name, fill it in
            if (token_processor_->peek_type() == TokenType::IDENTIFIER) {
                parameter->name = token_processor_->next_identifier();
            }

            if (token_processor_->peek_type() != TokenType::RPAREN)
                token_processor_->comma();
        }
    } else {

        // If the function is not declared, build the parameter list
        std::vector<Variable *> parameters;
        while (token_processor_->peek_type() != TokenType::RPAREN) {

            // Get data type and identifier
            Type *data_type = token_processor_->next_data_type();
            std::string identifier;
            if (token_processor_->peek_type() == TokenType::IDENTIFIER)
                identifier = token_processor_->next_identifier();

            // Insert the parameter into the parameter list
            VariableManager::insert(data_type, identifier, Context::get_name());
            Variable *parameter = VariableManager::find(identifier);
            parameters.push_back(parameter);

            if (token_processor_->peek_type() != TokenType::RPAREN)
                token_processor_->comma();
        }

        // Insert the function prototype into the symbol table
        FunctionManager::insert(return_type, name, parameters);

        // Update the prototype
        prototype = FunctionManager::find(name);
    }
    token_processor_->rparen();

    // If next token is a semicolon, it's a declaration
    if (token_processor_->peek_type() == TokenType::SEMI) {
        token_processor_->semi();
        Context::pop();
        return nullptr;
    }

    // Otherwise, it's a definition
    // Make sure the parameters have names
    for (const auto &parameters = prototype->parameters_;
         auto param : parameters) {
        if (param->name.empty())
            throw std::runtime_error("parameter " + param->name +
                                     " has no name");
    }

    // Build the code block
    CodeBlockNode *block = code_block();

    // Ensure non-void functions have a return statement
    if (!return_type->is_void() && !Context::has_return()) {
        throw std::runtime_error("function " + name + " must return a value");
    }

    // Ensure void functions don't have a return statement
    if (return_type->is_void() && Context::has_return()) {
        throw std::runtime_error("void function " + name +
                                 " cannot return a "
                                 "value");
    }

    // Pop the context
    Context::pop();

    // Build the function definition node
    return new FunctionDefinitionNode(prototype, block);
}

CodeBlockNode *Parser::code_block() {
    token_processor_->lbrace();

    std::vector<StatementNode *> statements;
    while (token_processor_->peek_type() != TokenType::RBRACE) {
        statements.push_back(statement());
    }
    token_processor_->rbrace();

    return new CodeBlockNode(statements);
}

StatementNode *Parser::statement() {
    switch (token_processor_->peek_type()) {
    case TokenType::IF:
        return Parser::if_statement();
    case TokenType::WHILE:
        return Parser::while_statement();
    case TokenType::FOR:
        return Parser::for_statement();
    case TokenType::RETURN:
        return Parser::return_statement();
    default:
        StatementNode *node = nullptr;
        if (data_types.contains(token_processor_->peek_type()))
            node = Parser::variable_declaration();
        else
            node = expression_.build_tree(Expression::MAX_PRECEDENCE);
        token_processor_->semi();
        return node;
    }
}

IfNode *Parser::if_statement() {
    token_processor_->match(TokenType::IF);

    // Parse the condition
    token_processor_->lparen();
    ExpressionNode *condition =
        expression_.build_tree(Expression::MAX_PRECEDENCE);
    token_processor_->rparen();

    // Parse the code block
    CodeBlockNode *block_if = code_block();

    // If there is an else statement, parse it
    CodeBlockNode *block_else = nullptr;
    if (token_processor_->peek_type() == TokenType::ELSE) {
        token_processor_->match(TokenType::ELSE);
        block_else = Parser::code_block();
    }

    return new IfNode(condition, block_if, block_else);
}

WhileNode *Parser::while_statement() {
    token_processor_->match(TokenType::WHILE);

    token_processor_->lparen();

    ExpressionNode *condition =
        expression_.build_tree(Expression::MAX_PRECEDENCE);

    token_processor_->rparen();

    CodeBlockNode *block = code_block();

    return new WhileNode(condition, block);
}

ForNode *Parser::for_statement() {
    token_processor_->match(TokenType::FOR);

    token_processor_->lparen();

    ExpressionNode *init = expression_.build_tree(Expression::MAX_PRECEDENCE);
    token_processor_->semi();
    ExpressionNode *condition =
        expression_.build_tree(Expression::MAX_PRECEDENCE);
    token_processor_->semi();
    ExpressionNode *increment =
        expression_.build_tree(Expression::MAX_PRECEDENCE);

    token_processor_->rparen();

    CodeBlockNode *block = code_block();

    return new ForNode(init, condition, increment, block);
}

ReturnNode *Parser::return_statement() {
    token_processor_->match(TokenType::RETURN);

    ExpressionNode *node = expression_.build_tree(Expression::MAX_PRECEDENCE);

    token_processor_->semi();

    // Check if the return type matches the function prototype
    if (!convertable_to(
            node->type(),
            FunctionManager::find(Context::get_name())->return_type_))
        throw std::runtime_error("return type mismatch in function " +
                                 Context::get_name());

    // Set the return flag
    Context::set_return_flag();

    return new ReturnNode(node);
}
} // namespace myComp
