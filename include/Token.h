#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <unordered_map>

namespace myComp {
// Token types
enum class TokenType {
    // EOF
    T_EOF,
    // Operators
    PLUS,
    MINUS,
    STAR,
    SLASH,
    MOD,
    EQUALS,
    NEQ,
    LESS,
    GREATER,
    LESS_EQ,
    GREATER_EQ,
    ASSIGN,
    AND,
    LOGICAL_AND,
    OR,
    LOGICAL_OR,
    XOR,
    L_SHIFT,
    R_SHIFT,
    INVERT,
    NOT,
    INC,
    DEC,
    // Punctuation
    SEMI,
    LBRACE,
    RBRACE,
    LPAREN,
    RPAREN,
    COMMA,
    LBRACKET,
    RBRACKET,
    ELLIPSIS,
    DOT,
    // Keywords: control flow
    WHILE,
    FOR,
    IF,
    ELSE,
    RETURN,
    // Keywords: data types
    INT_LITERAL,
    INT,
    CHAR,
    VOID,
    LONG,
    STRING_LITERAL,
    // Identifier
    IDENTIFIER,
};

// Token class
class Token {
  public:
    Token(TokenType type, long long integer_val, std::string string_val)
        : type_(type), integer_val_(integer_val),
          string_val_(std::move(string_val)) {}

    TokenType type() const { return this->type_; }

    std::string str() const {
        if (this->type_ == TokenType::INT_LITERAL)
            return "int_literal(" + std::to_string(this->integer_val_) + ")";

        if (this->type_ == TokenType::STRING_LITERAL)
            return "string_literal(" + this->string_val_ + ")";

        if (this->type_ == TokenType::IDENTIFIER)
            return "identifier(" + this->string_val_ + ")";

        return token_str.at(this->type_);
    }

    long long integer_val() const { return this->integer_val_; }

    std::string string_val() const { return this->string_val_; }

    inline static const std::unordered_map<TokenType, std::string> token_str = {
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

  private:
    TokenType type_;
    long long integer_val_;
    std::string string_val_;
};

// Token factory
class TokenFactory {
  public:
    static Token *getToken(TokenType type) { return getOrCreate(type, 0, ""); }

    static Token *getStringLiteral(const std::string &str) {
        return getOrCreate(TokenType::STRING_LITERAL, 0, str);
    }

    static Token *getIntegerLiteral(long long val) {
        return getOrCreate(TokenType::INT_LITERAL, val, "");
    }

    static Token *getIdentifier(const std::string &str) {
        return getOrCreate(TokenType::IDENTIFIER, 0, str);
    }

  private:
    static std::unordered_map<std::string, Token> &getCache() {
        static std::unordered_map<std::string, Token> cache;
        return cache;
    }

    static Token *getOrCreate(TokenType type, long long integer_val,
                              const std::string &string_val) {
        std::string str = Token::token_str.at(type);
        if (type == TokenType::INT_LITERAL)
            str += "(" + std::to_string(integer_val) + ")";
        else if (type == TokenType::STRING_LITERAL ||
                 type == TokenType::IDENTIFIER)
            str += "(" + string_val + ")";
        if (const auto it = getCache().find(str); it != getCache().end())
            return &it->second;
        if (type == TokenType::INT_LITERAL)
            return &getCache()
                        .emplace(str, Token(type, integer_val, ""))
                        .first->second;
        if (type == TokenType::STRING_LITERAL || type == TokenType::IDENTIFIER)
            return &getCache()
                        .emplace(str, Token(type, 0, string_val))
                        .first->second;
        return &getCache().emplace(str, Token(type, 0, "")).first->second;
    }
};
} // namespace myComp

#endif // TOKEN_H
