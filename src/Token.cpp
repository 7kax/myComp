#include "Token.h"

using namespace std;

namespace myComp {
const unordered_map<TokenType, string> token_str{
    {TokenType::T_EOF, "eof"},
    {TokenType::PLUS, "plus"},
    {TokenType::MINUS, "minus"},
    {TokenType::STAR, "star"},
    {TokenType::SLASH, "slash"},
    {TokenType::MOD, "mod"},
    {TokenType::EQUALS, "equals"},
    {TokenType::NEQ, "neq"},
    {TokenType::LESS, "less"},
    {TokenType::GREATER, "greater"},
    {TokenType::LESS_EQ, "less_eq"},
    {TokenType::GREATER_EQ, "greater_eq"},
    {TokenType::ASSIGN, "assign"},
    {TokenType::AND, "and"},
    {TokenType::LOGICAL_AND, "logical_and"},
    {TokenType::OR, "or"},
    {TokenType::LOGICAL_OR, "logical_or"},
    {TokenType::XOR, "xor"},
    {TokenType::L_SHIFT, "l_shift"},
    {TokenType::R_SHIFT, "r_shift"},
    {TokenType::INVERT, "invert"},
    {TokenType::NOT, "not"},
    {TokenType::INC, "inc"},
    {TokenType::DEC, "dec"},
    {TokenType::SEMI, "semi"},
    {TokenType::LBRACE, "lbrace"},
    {TokenType::RBRACE, "rbrace"},
    {TokenType::LPAREN, "lparen"},
    {TokenType::RPAREN, "rparen"},
    {TokenType::COMMA, "comma"},
    {TokenType::LBRACKET, "lbracket"},
    {TokenType::RBRACKET, "rbracket"},
    {TokenType::ELLIPSIS, "ellipsis"},
    {TokenType::DOT, "dot"},
    {TokenType::IF, "if"},
    {TokenType::ELSE, "else"},
    {TokenType::WHILE, "while"},
    {TokenType::FOR, "for"},
    {TokenType::RETURN, "return"},
    {TokenType::INT_LITERAL, "int_literal"},
    {TokenType::INT, "int"},
    {TokenType::CHAR, "char"},
    {TokenType::VOID, "void"},
    {TokenType::LONG, "long"},
    {TokenType::STRING_LITERAL, "string_literal"},
    {TokenType::IDENTIFIER, "identifier"},
};

string Token::str() const {
    if (type_ == TokenType::INT_LITERAL)
        return "int_literal(" + to_string(int_val_) + ")";

    if (type_ == TokenType::STRING_LITERAL)
        return "string_literal(" + str_val_ + ")";

    if (type_ == TokenType::IDENTIFIER)
        return "identifier(" + str_val_ + ")";

    return token_str.at(type_);
}

unordered_map<string, Token> &TokenFactory::getCache() {
    static unordered_map<string, Token> cache;
    return cache;
}

Token *TokenFactory::getOrCreate(TokenType type, long long int_val,
                                 const string &str_val) {
    string str = token_str.at(type);
    if (type == TokenType::INT_LITERAL)
        str += "(" + to_string(int_val) + ")";
    else if (type == TokenType::STRING_LITERAL || type == TokenType::IDENTIFIER)
        str += "(" + str_val + ")";
    if (const auto it = getCache().find(str); it != getCache().end())
        return &it->second;
    if (type == TokenType::INT_LITERAL)
        return &getCache().emplace(str, Token(type, int_val, "")).first->second;
    if (type == TokenType::STRING_LITERAL || type == TokenType::IDENTIFIER)
        return &getCache().emplace(str, Token(type, 0, str_val)).first->second;
    return &getCache().emplace(str, Token(type, 0, "")).first->second;
}

} // namespace myComp