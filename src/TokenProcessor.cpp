#include "TokenProcessor.h"
#include "Errors.h"

namespace myComp {

    void TokenProcessor::print(std::ostream &output) {
        for (auto &token: tokens) {
            output << token->str() << std::endl;
        }
    }

    void TokenProcessor::process() {
        scanner.next();
        while (scanner.get_token()->type() != TokenType::T_EOF) {
            tokens.push_back(scanner.get_token());
            line_numbers.push_back(scanner.get_line());
            scanner.next();
        }
    }

    std::pair<Token *, int> TokenProcessor::next_token() {
        if (current_token >= tokens.size()) {
            return {nullptr, -1};
        }
        Token *token = tokens[current_token];
        int line_number = line_numbers[current_token];
        current_token++;
        return {token, line_number};
    }

    Type_ *TokenProcessor::next_data_type() {
        auto [token, line] = this->next_token();
        Type_ *ret = nullptr;
        switch (token->type()) {
            case TokenType::VOID:
                ret = TypeFactory::getVoidType();
                break;
            case TokenType::CHAR:
                ret = TypeFactory::getIntegerType(1, true);
                break;
            case TokenType::INT:
                ret = TypeFactory::getIntegerType(4, true);
                break;
            case TokenType::LONG:
                ret = TypeFactory::getIntegerType(8, true);
                break;
            default:
                // throw std::runtime_error("Invalid data type");
                Errors::syntax_error("invalid data type", line);
        }
        // std::tie(token, line) = this->next_token();
        while (this->peek_token().first->type() == TokenType::STAR) {
            ret = TypeFactory::getPointerType(ret);
            this->next_token();
        }
        return ret;
    }

    std::string TokenProcessor::next_identifier() {
        auto [token, line] = this->next_token();
        if (token->type() != TokenType::IDENTIFIER) {
            Errors::syntax_error("invalid identifier", line);
        }
        return token->string_val();
    }

    void TokenProcessor::match(TokenType type) {
        auto [token, line] = this->next_token();
        if (token->type() != type) {
            Errors::syntax_error("expect token " + Token::token_str.at(type) + " but got " + token->str(), line);
        }
    }

    std::pair<Token *, int> TokenProcessor::peek_token() {
        if (current_token >= tokens.size()) {
            return {nullptr, -1};
        }
        return {tokens[current_token], line_numbers[current_token]};
    }

    long long TokenProcessor::next_integer() {
        auto [token, line] = this->next_token();
        if (token->type() != TokenType::INT_LITERAL) {
            Errors::syntax_error("expect integer literal but got " + token->str(), line);
        }
        return token->integer_val();
    }

    std::string TokenProcessor::next_string() {
        auto [token, line] = this->next_token();
        if (token->type() != TokenType::STRING_LITERAL) {
            Errors::syntax_error("expect string literal but got " + token->str(), line);
        }
        return token->string_val();
    }
}