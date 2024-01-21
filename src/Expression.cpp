#include "defs.h"
#include "objects.h"
#include "data.h"

namespace myComp {
    ASTNode *Expression::build_tree(int pre_precedence) {
        ASTNode *left = Expression::primary();

        if (!scanner.is_operator())
            return left;

        ASTNodeType operator_type = token_to_op.at(scanner.get_token().type);

        while ((precedence.at(operator_type) < pre_precedence) ||
               (pre_precedence == precedence.at(ASTNodeType::ASSIGN) && operator_type == ASTNodeType::ASSIGN)) {
            scanner.next();
            ASTNode *right = Expression::build_tree(precedence.at(operator_type));

            // Do some type checking
            if (left->data_type == DataType::VOID || right->data_type == DataType::VOID)
                Errors::syntax_error("void value not ignored", scanner.get_line());
            if (Type::is_pointer(left->data_type) && Type::is_pointer(right->data_type) &&
                left->data_type != right->data_type)
                Errors::syntax_error("pointer type mismatch", scanner.get_line());
            if (Type::is_pointer(left->data_type) && Type::is_pointer(right->data_type) && operator_type ==
                ASTNodeType::ADD)
                Errors::syntax_error("invalid operands to binary +", scanner.get_line());

            if (Type::is_integer(left->data_type) && Type::is_integer(right->data_type))
                Type::integer_widen(left, right);
            else if (Type::is_pointer(left->data_type) && Type::is_integer(right->data_type))
                Type::integer_scale(left, right);
            else if (Type::is_integer(left->data_type) && Type::is_pointer(right->data_type))
                Type::integer_scale(right, left);

            left = Tree::join(operator_type, left, right);
            left->data_type = left->left->data_type;

            if (!scanner.is_operator())
                return left;
            operator_type = token_to_op.at(scanner.get_token().type);
        }
        return left;
    }

    ASTNode *Expression::primary() {
        if (ASTNode *prefix = Expression::prefix(); prefix != nullptr)
            return prefix;
        TokenType token_type = scanner.get_token().type;
        int val;
        std::string str;
        ASTNode *node;
        switch (token_type) {
            case TokenType::INT_LITERAL:
                val = scanner.next_int_literal();
                return Tree::int_literal(val);
            case TokenType::STRING_LITERAL:
                str = scanner.next_string();
                string_literals.insert({str, {}});
                return Tree::string_literal(str);
            case TokenType::IDENTIFIER:
                return Expression::identifier();
            case TokenType::LPAREN:
                scanner.lparen();
                node = Expression::build_tree(20);
                scanner.rparen();
                return node;
            default:
                Errors::unexpected_token(token_type, scanner.get_line());
        }

        return nullptr;
    }

    ASTNode *Expression::parameters(const std::string &name) {
        ASTNode *node = nullptr, *head = nullptr;
        for (const auto &parameter: function.get_prototype(name).parameters) {
            ASTNode *param = Expression::build_tree(20);
            assert(Type::convertable_to(param->data_type, parameter.type));
            if (head == nullptr) {
                head = node = Tree::parameter(param);
            } else {
                node->left = Tree::parameter(param);
                node = node->left;
            }
            if (scanner.get_token().type != TokenType::RPAREN)
                scanner.comma();
        }
        return head;
    }

    ASTNode *Expression::prefix() {
        if (!scanner.is_prefix_operator())
            return nullptr;
        TokenType token_type = scanner.get_token().type;
        scanner.next();

        ASTNode *node = Expression::primary();
        switch (token_type) {
            case TokenType::STAR:
                if (!Type::is_pointer(node->data_type))
                    Errors::expected("pointer", scanner.get_line());
                return Tree::dereference(node);
            case TokenType::AND:
                if (node->type != ASTNodeType::VARIABLE)
                    Errors::expected("variable", scanner.get_line());
                return Tree::address(node);
            case TokenType::MINUS:
                if (!Type::is_integer(node->data_type))
                    Errors::expected("integer", scanner.get_line());
                return Tree::negative(node);
            case TokenType::NOT:
                return Tree::NOT(node);
            case TokenType::INVERT:
                return Tree::invert(node);
            case TokenType::INC:
                if (node->type != ASTNodeType::VARIABLE)
                    Errors::expected("variable", scanner.get_line());
                return Tree::increment(node, true);
            case TokenType::DEC:
                if (node->type != ASTNodeType::VARIABLE)
                    Errors::expected("variable", scanner.get_line());
                return Tree::decrement(node, true);
            default:
                Errors::unexpected_token(token_type, scanner.get_line());
        }

        return nullptr;
    }

    ASTNode *Expression::identifier() {
        ASTNode *node;
        std::string str = scanner.next_string();

        TokenType token_type = scanner.get_token().type;
        switch (token_type) {
            case TokenType::LPAREN:
                // Ensure the function prototype exists
                if (!function.is_declared(str))
                    Errors::syntax_error("function " + str + " is not declared", scanner.get_line());

                scanner.lparen();
                node = Expression::parameters(str);
                scanner.rparen();
                return Tree::function_call(str, node);
            case TokenType::LBRACKET:
                scanner.lbracket();
                node = Expression::build_tree(20);
                scanner.rbracket();
                return Tree::dereference(Tree::variable(str), node);
            default:
                if (ASTNode *postfix = Expression::postfix(str); postfix != nullptr)
                    return postfix;
                return Tree::variable(str);
        }
    }

    ASTNode *Expression::postfix(const std::string &identifier) {
        if (!scanner.is_postfix_operator())
            return nullptr;
        TokenType token_type = scanner.get_token().type;
        scanner.next();
        switch (token_type) {
            case TokenType::INC:
                return Tree::increment(Tree::variable(identifier), false);
            case TokenType::DEC:
                return Tree::decrement(Tree::variable(identifier), false);
            default:
                Errors::unexpected_token(token_type, scanner.get_line());
        }

        return nullptr;
    }
} // myComp
