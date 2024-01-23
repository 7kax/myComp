#ifndef TOKENPROCESSOR_H
#define TOKENPROCESSOR_H

#include "Scanner.h"


namespace myComp {
    class TokenProcessor {
    public:

        void print(std::ostream &output);

        void set_input(const std::string &filename) { scanner.set_input(filename); }

        // Read all tokens from the input file
        void process();

        bool eof() { return current_token >= tokens.size(); }

        // Get the next token
        std::pair<Token *, int> next_token();

        std::pair<Token *, int> peek_token();

        // Utils
        Type_ *next_data_type();

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
}

#endif //TOKENPROCESSOR_H
