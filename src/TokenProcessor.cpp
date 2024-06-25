#include "TokenProcessor.h"
#include "Errors.h"
namespace myComp {

void TokenProcessor::print(std::ostream &output) {
    for (auto &token : tokens) {
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
std::pair<Token *, int> TokenProcessor::peek_token() {
    if (current_token >= tokens.size()) {
        return {nullptr, -1};
    }
    return {tokens[current_token], line_numbers[current_token]};
}

Type *TokenProcessor::next_data_type() {
    auto [token, line] = this->next_token();
    Type *ret = nullptr;
    switch (token->type()) {
    case TokenType::VOID:
        ret = TypeFactory::get_void();
        break;
    case TokenType::CHAR:
        ret = TypeFactory::get_char();
        break;
    case TokenType::INT:
        ret = TypeFactory::get_signed(4);
        break;
    case TokenType::LONG:
        ret = TypeFactory::get_signed(8);
        break;
    default:
        throw InvalidException("data type", line);
    }
    // std::tie(token, line) = this->next_token();
    while (this->peek_token().first->type() == TokenType::STAR) {
        ret = TypeFactory::get_pointer(ret);
        this->next_token();
    }
    return ret;
}

std::string TokenProcessor::next_identifier() {
    auto [token, line] = this->next_token();
    if (token->type() != TokenType::IDENTIFIER) {
        throw UnexpectedTokenException(token->type(), line,
                                       TokenType::IDENTIFIER);
    }
    return token->string_val();
}

void TokenProcessor::match(TokenType type) {
    auto [token, line] = this->next_token();
    if (token->type() != type) {
        throw UnexpectedTokenException(token->type(), line, type);
    }
}

long long TokenProcessor::next_integer() {
    auto [token, line] = this->next_token();
    if (token->type() != TokenType::INT_LITERAL) {
        throw UnexpectedTokenException(token->type(), line,
                                       TokenType::INT_LITERAL);
    }
    return token->integer_val();
}

std::string TokenProcessor::next_string() {
    auto [token, line] = this->next_token();
    if (token->type() != TokenType::STRING_LITERAL) {
        throw UnexpectedTokenException(token->type(), line,
                                       TokenType::STRING_LITERAL);
    }
    return token->string_val();
}
} // namespace myComp