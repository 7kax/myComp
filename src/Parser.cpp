#include "Parser.h"

#include "SymbolTable.h"
#include "Context.h"
#include "Tree.h"
#include "Errors.h"

#include "data.h"

namespace myComp {
    ASTNode *Parser::build_tree() {
        Type_ *data_type = this->token_processor_->next_data_type();
        std::string identifier = this->token_processor_->next_identifier();

        if (this->token_processor_->peek_token().first->type() != TokenType::LPAREN) {
            variable_declaration(data_type, identifier);
            // scanner.semi();
            // this->token_processor_->semi();
            return nullptr;
        } else
            return function_declaration(data_type, identifier);
    }

    // todo: optimize this function
    void Parser::variable_declaration(Type_ *data_type, std::string &name) {
        // int size = 1;
        // Type_ *array_type = nullptr;
        SymbolTable &table = symbol_tables[context.get_name()];

        // auto [token, line] = this->token_processor_->peek_token();
        if (this->token_processor_->peek_token().first->type() == TokenType::LBRACKET) {
            this->token_processor_->lbracket();
            int size = static_cast<int>(this->token_processor_->next_integer());
            this->token_processor_->rbracket();
            Type_ *array_type = TypeFactory::getArrayType(data_type, size);
            table.insert(name, array_type);
        } else {
            table.insert(name, data_type);
        }

        // std::tie(token, line) = this->token_processor_->peek_token();
        while (this->token_processor_->peek_token().first->type() == TokenType::COMMA) {
            this->token_processor_->comma();
            name = this->token_processor_->next_identifier();
            // std::tie(token, line) = this->token_processor_->peek_token();
            if (this->token_processor_->peek_token().first->type() == TokenType::LBRACKET) {
                this->token_processor_->lbracket();
                int size = static_cast<int>(this->token_processor_->next_integer());
                this->token_processor_->rbracket();
                Type_ *array_type = TypeFactory::getArrayType(data_type, size);
                table.insert(name, array_type);
            } else {
                table.insert(name, data_type);
            }
        }
        this->token_processor_->semi();
    }

    ASTNode *Parser::variable_declaration() {
        Type_ *data_type = this->token_processor_->next_data_type();
        SymbolTable &table = symbol_tables[context.get_name()];
        // Build the tree
        ASTNode *root = nullptr, *current = nullptr;
        // auto [token, line] = this->token_processor_->peek_token();
        while (this->token_processor_->peek_token().first->type() != TokenType::SEMI) {
            std::string name = this->token_processor_->next_identifier();
            // std::tie(token, line) = this->token_processor_->peek_token();
            if (this->token_processor_->peek_token().first->type() == TokenType::LBRACKET) {
                this->token_processor_->lbracket();
                int size = static_cast<int>(this->token_processor_->next_integer());
                this->token_processor_->rbracket();
                Type_ *array_type = TypeFactory::getArrayType(data_type, size);
                table.insert(name, array_type);
            } else {
                // symbol_tables[function_name].insert(name, data_type, size);
                table.insert(name, data_type);
            }
            if (root == nullptr)
                root = current = Tree::variable_declaration(name, data_type);
            else {
                current->left = Tree::variable_declaration(name, data_type);
                current = current->left;
            }
            // std::tie(token, line) = this->token_processor_->peek_token();
            if (this->token_processor_->peek_token().first->type() != TokenType::SEMI)
                this->token_processor_->comma();
        }

        return root;
    }

    ASTNode *Parser::function_declaration(Type_ *return_type,
                                          const std::string &name) {
        // Tell if the function is already declared
        bool declared = function.is_declared(name);
        if (declared)
            assert(return_type == function.get_prototype(name).return_type);
        // Fetch the parameter list
        this->token_processor_->lparen();
        if (declared) {
            // Update the parameter list
            for (auto &parameters = function.get_prototype(name).parameters;
                    auto &[type, param_name]: parameters) {
                // assert(scanner.next_data_type() == parameter.type);
                if (this->token_processor_->next_data_type() != type) {
                    Errors::fatal_error("type doesn't match prototype for parameter: " +
                                        param_name + " in function " + name);
                }
                // auto [token, line] = this->token_processor_->peek_token();
                if (this->token_processor_->peek_token().first->type() == TokenType::IDENTIFIER) {
                    param_name = this->token_processor_->next_identifier();
                    // std::tie(token, line) = this->token_processor_->peek_token();
                }
                if (this->token_processor_->peek_token().first->type() != TokenType::RPAREN)
                    this->token_processor_->comma();
            }
        } else {
            // Build the parameter list
            std::vector<parameter> parameters;
            // auto [token, line] = this->token_processor_->peek_token();
            while (this->token_processor_->peek_token().first->type() != TokenType::RPAREN) {
                Type_ *data_type = this->token_processor_->next_data_type();
                std::string identifier;
                // std::tie(token, line) = this->token_processor_->peek_token();
                if (this->token_processor_->peek_token().first->type() == TokenType::IDENTIFIER)
                    identifier = this->token_processor_->next_identifier();
                parameters.emplace_back(data_type, identifier);
                // std::tie(token, line) = this->token_processor_->peek_token();
                if (this->token_processor_->peek_token().first->type() != TokenType::RPAREN)
                    this->token_processor_->comma();
            }
            function.add_prototype(name, return_type, parameters);
        }
        this->token_processor_->rparen();
        // If next token is a semicolon, it's a declaration
        if (this->token_processor_->peek_token().first->type() == TokenType::SEMI) {
            this->token_processor_->semi();
            return nullptr;
        }

        // Otherwise, it's a definition
        // Import the parameter list to the symbol table
        for (const auto &parameters = function.get_prototype(name).parameters;
                const auto &[type, param_name]: parameters) {
            assert(!param_name.empty());
            symbol_tables[name].insert(param_name, type);
        }

        // Build the tree
        context.push(name);
        ASTNode *tree = code_block();

        // Ensure non-void functions have a return statement
        if (!return_type->is_void() && !context.has_return()) {
            Errors::fatal_error("function " + name + " has no return statement");
        }

        // Ensure void functions don't have a return statement
        if (return_type->is_void() && context.has_return()) {
            Errors::fatal_error("can't return a value from a void function " + name);
        }

        context.pop();

        return Tree::function_declaration(name, tree);
    }

    ASTNode *Parser::code_block() {
        this->token_processor_->lbrace();
        ASTNode *root = nullptr, *n = nullptr;
        while (this->token_processor_->peek_token().first->type() != TokenType::RBRACE) {
            if (root == nullptr)
                root = n = Tree::unary(ASTNodeType::COMPOUND, Parser::statement());
            else {
                n->right = Tree::unary(ASTNodeType::COMPOUND, Parser::statement());
                n = n->right;
            }
        }
        this->token_processor_->rbrace();

        return root;
    }

    ASTNode *Parser::statement() {
        switch (this->token_processor_->peek_token().first->type()) {
            case TokenType::IF:
                return Parser::if_statement();
            case TokenType::WHILE:
                return Parser::while_statement();
            case TokenType::FOR:
                return Parser::for_statement();
            case TokenType::RETURN:
                return Parser::return_statement();
            default:
                ASTNode *node;
                if (data_types.contains(this->token_processor_->peek_token().first->type()))
                    node = Parser::variable_declaration();
                else
                    node = this->expression_->build_tree(Expression::MAX_PRECEDENCE);
                this->token_processor_->semi();
                return node;
        }
    }

    ASTNode *Parser::if_statement() {
        this->token_processor_->match(TokenType::IF);

        this->token_processor_->lparen();
        ASTNode *condition = this->expression_->build_tree(Expression::MAX_PRECEDENCE);
        this->token_processor_->rparen();

        ASTNode *block_if = code_block();

        if (this->token_processor_->peek_token().first->type() != TokenType::ELSE)
            return Tree::join(ASTNodeType::IF, condition, block_if);

        this->token_processor_->match(TokenType::ELSE);

        ASTNode *block_else = Parser::code_block();

        return Tree::join(ASTNodeType::IF, condition,
                          Tree::join(ASTNodeType::ELSE, block_if, block_else));
    }

    ASTNode *Parser::while_statement() {
        this->token_processor_->match(TokenType::WHILE);

        this->token_processor_->lparen();
        ASTNode *condition = this->expression_->build_tree(Expression::MAX_PRECEDENCE);
        this->token_processor_->rparen();

        ASTNode *block = code_block();

        return Tree::join(ASTNodeType::WHILE, condition, block);
    }

    ASTNode *Parser::for_statement() {
        this->token_processor_->match(TokenType::FOR);

        this->token_processor_->lparen();
        ASTNode *init = this->expression_->build_tree(Expression::MAX_PRECEDENCE);
        this->token_processor_->semi();
        ASTNode *condition = this->expression_->build_tree(Expression::MAX_PRECEDENCE);
        this->token_processor_->semi();
        ASTNode *increment = this->expression_->build_tree(Expression::MAX_PRECEDENCE);
        this->token_processor_->rparen();

        ASTNode *block = code_block();

        return Tree::join(ASTNodeType::FOR,
                          Tree::join(ASTNodeType::GLUE, init, condition),
                          Tree::join(ASTNodeType::GLUE, increment, block));
    }

    ASTNode *Parser::return_statement() {
        this->token_processor_->match(TokenType::RETURN);

        ASTNode *node = this->expression_->build_tree(Expression::MAX_PRECEDENCE);

        this->token_processor_->semi();

        if (!node->data_type->convertable_to(
                function.get_prototype(context.get_name()).return_type))
            Errors::fatal_error("return type mismatch in function " +
                                context.get_name());

        // Set the return flag
        context.set_return_flag();

        return Tree::unary(ASTNodeType::RETURN, node);
    }
} // namespace myComp
