#include "Expression.h"

#include "Errors.h"
#include "Tree.h"

#include "data.h"

namespace myComp {
    ASTNode *Expression::build_tree(int pre_precedence) {
        ASTNode *left = Expression::primary();

        if (!Expression::is_operator(this->token_processor_->peek_token().first->type()))
            return left;

        ASTNodeType operator_type = token_to_op.at(this->token_processor_->peek_token().first->type());

        while ((precedence.at(operator_type) < pre_precedence) ||
               (pre_precedence == precedence.at(ASTNodeType::ASSIGN) &&
                operator_type == ASTNodeType::ASSIGN)) {
            auto [token, line] = this->token_processor_->next_token();
            ASTNode *right = Expression::build_tree(precedence.at(operator_type));

            // Do some type checking
            if (left->data_type->is_void() || right->data_type->is_void())
                Errors::syntax_error("void value not ignored", line);
            if (left->data_type->is_pointer() && right->data_type->is_pointer() &&
                left->data_type != right->data_type)
                Errors::syntax_error("pointer type mismatch", line);
            if (left->data_type->is_pointer() && right->data_type->is_pointer() &&
                operator_type == ASTNodeType::ADD)
                Errors::syntax_error("invalid operands to binary +", line);

            if (left->data_type->is_integer() && right->data_type->is_integer())
                Expression::integer_widen(left, right);
                // else if (Type::is_pointer(left->data_type) &&
                //          Type::is_integer(right->data_type))
            else if (left->data_type->is_pointer() && right->data_type->is_integer())
                Expression::integer_scale(left, right);
                // else if (Type::is_integer(left->data_type) &&
                //          Type::is_pointer(right->data_type))
            else if (left->data_type->is_integer() && right->data_type->is_pointer())
                Expression::integer_scale(right, left);

            left = Tree::join(operator_type, left, right);
            left->data_type = left->left->data_type;

            if (!Expression::is_operator(this->token_processor_->peek_token().first->type()))
                return left;
            operator_type = token_to_op.at(this->token_processor_->peek_token().first->type());
        }
        return left;
    }

    ASTNode *Expression::primary() {
        if (ASTNode *prefix = Expression::prefix(); prefix != nullptr)
            return prefix;
        long long val;
        std::string str;
        ASTNode *node;
        switch (this->token_processor_->peek_token().first->type()) {
            case TokenType::INT_LITERAL:
                val = this->token_processor_->next_integer();
                return Tree::int_literal(val);
            case TokenType::STRING_LITERAL:
                str = this->token_processor_->next_string();
                string_literals.insert({str, {}});
                return Tree::string_literal(str);
            case TokenType::IDENTIFIER:
                return Expression::identifier();
            case TokenType::LPAREN:
                this->token_processor_->lparen();
                node = Expression::build_tree(Expression::MAX_PRECEDENCE);
                this->token_processor_->rparen();
                return node;
            default:
                Errors::unexpected_token(this->token_processor_->peek_token().first,
                                         this->token_processor_->peek_token().second);
        }

        return nullptr;
    }

    ASTNode *Expression::parameters() {
        ASTNode *node = nullptr, *head = nullptr;
        while (this->token_processor_->peek_token().first->type() != TokenType::RPAREN) {
            ASTNode *param = Expression::build_tree(Expression::MAX_PRECEDENCE);
            if (head == nullptr) {
                head = node = Tree::parameter(param);
            } else {
                node->left = Tree::parameter(param);
                node = node->left;
            }
            if (this->token_processor_->peek_token().first->type() != TokenType::RPAREN)
                this->token_processor_->comma();
        }

        return head;
    }

    void Expression::integer_widen(ASTNode *&left, ASTNode *&right) {
        int left_size = left->data_type->size();
        int right_size = right->data_type->size();

        if (left_size == right_size)
            return;

        if (left_size > right_size) {
            right = Tree::unary(ASTNodeType::WIDEN, right);
            right->data_type = left->data_type;
        } else {
            left = Tree::unary(ASTNodeType::WIDEN, left);
            left->data_type = right->data_type;
        }
    }

    void Expression::integer_scale(ASTNode *&pointer, ASTNode *&integer) {
        int size = dynamic_cast<PointerType *>(pointer->data_type)->pointee()->size();

        integer = Tree::unary(ASTNodeType::SCALE, integer);
        integer->data_type = pointer->data_type;
        integer->val = size;
    }

    ASTNode *Expression::prefix() {
        if (!Expression::is_prefix_operator(this->token_processor_->peek_token().first->type()))
            return nullptr;
        // Token *token = scanner.get_token();
        // scanner.next();
        auto [token, line] = this->token_processor_->next_token();
        ASTNode *node = Expression::primary();
        switch (token->type()) {
            case TokenType::STAR:
                if (!node->data_type->is_pointer())
                    Errors::expected("pointer", line);
                return Tree::dereference(node);
            case TokenType::AND:
                if (node->type != ASTNodeType::VARIABLE)
                    Errors::expected("variable", line);
                return Tree::address(node);
            case TokenType::MINUS:
                if (!node->data_type->is_integer())
                    Errors::expected("integer", line);
                return Tree::negative(node);
            case TokenType::NOT:
                return Tree::NOT(node);
            case TokenType::INVERT:
                return Tree::invert(node);
            case TokenType::INC:
                if (node->type != ASTNodeType::VARIABLE)
                    Errors::expected("variable", line);
                return Tree::increment(node, true);
            case TokenType::DEC:
                if (node->type != ASTNodeType::VARIABLE)
                    Errors::expected("variable", line);
                return Tree::decrement(node, true);
            default:
                Errors::unexpected_token(token, line);
        }

        return nullptr;
    }

    ASTNode *Expression::identifier() {
        ASTNode *node;
        std::string str = this->token_processor_->next_identifier();

        switch (this->token_processor_->peek_token().first->type()) {
            case TokenType::LPAREN:
                // Ensure the function prototype exists
                if (!function.is_declared(str))
                    Errors::syntax_error("function " + str + " is not declared",
                                         this->token_processor_->peek_token().second);
                this->token_processor_->lparen();
                node = Expression::parameters();
                this->token_processor_->rparen();
                return Tree::function_call(str, node);
            case TokenType::LBRACKET:
                this->token_processor_->lbracket();
                node = Expression::build_tree(Expression::MAX_PRECEDENCE);
                this->token_processor_->rbracket();
                return Tree::dereference(Tree::variable(str), node);
            default:
                if (ASTNode *postfix = Expression::postfix(str); postfix != nullptr)
                    return postfix;
                return Tree::variable(str);
        }
    }

    ASTNode *Expression::postfix(const std::string &identifier) {
        if (!Expression::is_postfix_operator(this->token_processor_->peek_token().first->type()))
            return nullptr;
        auto [token, line] = this->token_processor_->next_token();
        switch (token->type()) {
            case TokenType::INC:
                return Tree::increment(Tree::variable(identifier), false);
            case TokenType::DEC:
                return Tree::decrement(Tree::variable(identifier), false);
            default:
                Errors::unexpected_token(token, line);
        }

        return nullptr;
    }

    bool Expression::is_prefix_operator(TokenType type) {
        return type == TokenType::STAR ||
               type == TokenType::AND ||
               type == TokenType::INVERT ||
               type == TokenType::NOT ||
               type == TokenType::INC ||
               type == TokenType::DEC ||
               type == TokenType::MINUS;
    }

    bool Expression::is_postfix_operator(TokenType type) {
        return type == TokenType::INC ||
               type == TokenType::DEC;
    }

    bool Expression::is_operator(TokenType type) {
        return token_to_op.contains(type);
    }
} // namespace myComp
