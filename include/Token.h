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

extern const std::unordered_map<TokenType, std::string> token_str;

// Token class
class Token {
  public:
    Token(TokenType type, long long int_val, std::string str_val)
        : type_(type), int_val_(int_val), str_val_(std::move(str_val)) {}

    TokenType type() const { return type_; }

    std::string str() const;

    long long integer_val() const { return int_val_; }

    std::string string_val() const { return str_val_; }

  private:
    TokenType type_;
    long long int_val_;
    std::string str_val_;
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
    static std::unordered_map<std::string, Token> &getCache();

    static Token *getOrCreate(TokenType type, long long int_val,
                              const std::string &str_val);
};
} // namespace myComp
#endif // TOKEN_H
