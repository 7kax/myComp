#include "defs.h"
#include "objects.h"
#include "data.h"

namespace myComp {
    ASTNode *Parser::build_tree() {
        DataType data_type = scanner.next_data_type();
        std::string identifier = scanner.next_string();

        if (scanner.get_token().type != TokenType::LPAREN) {
            variable_declaration(data_type, identifier);
            scanner.semi();
            return nullptr;
        } else
            return function_declaration(data_type, identifier);
    }

    void Parser::variable_declaration(DataType data_type, std::string &name) {
        int size = 1;
        if (scanner.get_token().type == TokenType::LBRACKET) {
            scanner.lbracket();
            size = scanner.next_int_literal();
            scanner.rbracket();
        }
        symbol_tables[context.get_name()].insert(name, data_type, size);

        while (scanner.get_token().type != TokenType::SEMI) {
            size = 1;
            name = scanner.next_string();
            if (scanner.get_token().type == TokenType::LBRACKET) {
                scanner.lbracket();
                size = scanner.next_int_literal();
                scanner.rbracket();
            }
            symbol_tables[context.get_name()].insert(name, data_type, size);
            if (scanner.get_token().type != TokenType::SEMI)
                scanner.comma();
        }
    }

    ASTNode *Parser::variable_declaration() {
        // Fetch the data type and name
        DataType data_type = scanner.next_data_type();
        const std::string &function_name = context.get_name();
        // Build the tree
        ASTNode *root = nullptr, *current = nullptr;
        while (scanner.get_token().type != TokenType::SEMI) {
            std::string name = scanner.next_string();
            int size = 1;
            if (scanner.get_token().type == TokenType::LBRACKET) {
                scanner.lbracket();
                size = scanner.next_int_literal();
                scanner.rbracket();
            }
            symbol_tables[function_name].insert(name, data_type, size);
            if (root == nullptr)
                root = current = Tree::variable_declaration(name, data_type, size);
            else {
                current->left = Tree::variable_declaration(name, data_type, size);
                current = current->left;
            }
            if (scanner.get_token().type != TokenType::SEMI)
                scanner.comma();
        }

        return root;
    }

    ASTNode *Parser::function_declaration(DataType return_type, const std::string &name) {
        // Tell if the function is already declared
        bool declared = function.is_declared(name);
        if (declared)
            assert(return_type == function.get_prototype(name).return_type);
        // Fetch the parameter list
        scanner.lparen();
        if (declared) {
            // Update the parameter list
            std::vector<parameter> &parameters = function.get_prototype(name).parameters;
            for (auto &parameter: parameters) {
                assert(scanner.next_data_type() == parameter.type);
                if (scanner.get_token().type == TokenType::IDENTIFIER)
                    parameter.name = scanner.next_string();
                if (scanner.get_token().type != TokenType::RPAREN)
                    scanner.comma();
            }
        } else {
            // Build the parameter list
            std::vector<parameter> parameters;
            while (scanner.get_token().type != TokenType::RPAREN) {
                DataType data_type = scanner.next_data_type();
                std::string identifier;
                if (scanner.get_token().type == TokenType::IDENTIFIER)
                    identifier = scanner.next_string();
                parameters.emplace_back(data_type, identifier);
                if (scanner.get_token().type != TokenType::RPAREN)
                    scanner.comma();
            }
            function.add_prototype(name, return_type, parameters);
        }
        scanner.rparen();
        // If next token is a semicolon, it's a declaration
        if (scanner.get_token().type == TokenType::SEMI) {
            scanner.semi();
            return nullptr;
        }
        // Otherwise, it's a definition
        // Import the parameter list to the symbol table
        const std::vector<parameter> &parameters = function.get_prototype(name).parameters;
        for (const auto &parameter: parameters) {
            assert(!parameter.name.empty());
            symbol_tables[name].insert(parameter.name, parameter.type, 1);
        }
        // Build the tree
        context.push(name);
        ASTNode *tree = Parser::code_block();
        context.pop();
        return Tree::function_declaration(name, tree);
    }

    ASTNode *Parser::code_block() {
        scanner.lbrace();
        ASTNode *root = nullptr, *n = nullptr;
        while (scanner.get_token().type != TokenType::RBRACE) {
            if (root == nullptr)
                root = n = Tree::unary(ASTNodeType::COMPOUND, Parser::statement());
            else {
                n->right = Tree::unary(ASTNodeType::COMPOUND, Parser::statement());
                n = n->right;
            }
        }
        scanner.rbrace();

        return root;
    }

    ASTNode *Parser::statement() {
        switch (scanner.get_token().type) {
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
                if (scanner.is_data_type())
                    node = Parser::variable_declaration();
                else
                    node = Expression::build_tree(20);
                scanner.semi();
                return node;
        }
    }

    ASTNode *Parser::if_statement() {
        scanner.match(TokenType::IF, "if");

        scanner.lparen();
        ASTNode *condition = Expression::build_tree(20);
        scanner.rparen();

        ASTNode *block_if = Parser::code_block();

        if (scanner.get_token().type != TokenType::ELSE)
            return Tree::join(ASTNodeType::IF, condition, block_if);

        scanner.match(TokenType::ELSE, "else");

        ASTNode *block_else = Parser::code_block();

        return Tree::join(ASTNodeType::IF, condition,
                          Tree::join(ASTNodeType::ELSE, block_if, block_else));
    }

    ASTNode *Parser::while_statement() {
        scanner.match(TokenType::WHILE, "while");

        scanner.lparen();
        ASTNode *condition = Expression::build_tree(20);
        scanner.rparen();

        ASTNode *block = Parser::code_block();

        return Tree::join(ASTNodeType::WHILE, condition, block);
    }

    ASTNode *Parser::for_statement() {
        scanner.match(TokenType::FOR, "for");

        scanner.lparen();
        ASTNode *init = Expression::build_tree(20);
        scanner.semi();
        ASTNode *condition = Expression::build_tree(20);
        scanner.semi();
        ASTNode *increment = Expression::build_tree(20);
        scanner.rparen();

        ASTNode *block = Parser::code_block();

        return Tree::join(ASTNodeType::FOR,
                          Tree::join(ASTNodeType::GLUE, init, condition),
                          Tree::join(ASTNodeType::GLUE, increment, block));
    }

    ASTNode *Parser::return_statement() {
        scanner.match(TokenType::RETURN, "return");

        ASTNode *node = Expression::build_tree(20);

        scanner.semi();

        return Tree::unary(ASTNodeType::RETURN, node);
    }
} // myComp