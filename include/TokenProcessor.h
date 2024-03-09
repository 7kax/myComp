#ifndef TOKENPROCESSOR_H
#define TOKENPROCESSOR_H

#include "Scanner.h"
#include "Type.h"

namespace myComp {
class TokenProcessor {
  public:
    void print(std::ostream &output);
    void set_input(const std::string &filename) { scanner.set_input(filename); }
    void process(); // Read all tokens from the input file
    bool eof() { return current_token >= tokens.size(); }
    std::pair<Token *, int> next_token(); // Get the next token
    std::pair<Token *, int> peek_token(); // Peek the next token
    int current_line() { return line_numbers[current_token]; }

    // Utils
    TokenType peek_type() { return peek_token().first->type(); }
    Type *next_data_type();
    std::string next_identifier();
    std::string next_string();
    long long next_integer();
    void match(TokenType type);
    void semi() { this->match(TokenType::SEMI); }
    void lbracket() { this->match(TokenType::LBRACKET); }
    void rbracket() { this->match(TokenType::RBRACKET); }
    void comma() { this->match(TokenType::COMMA); }
    void lparen() { this->match(TokenType::LPAREN); }
    void rparen() { this->match(TokenType::RPAREN); }
    void lbrace() { this->match(TokenType::LBRACE); }
    void rbrace() { this->match(TokenType::RBRACE); }

  private:
    Scanner scanner;
    std::vector<Token *> tokens;
    std::vector<int> line_numbers;
    int current_token = 0;
};
} // namespace myComp

#endif // TOKENPROCESSOR_H
