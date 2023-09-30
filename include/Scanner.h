#ifndef MYCOMP_SCANNER_H
#define MYCOMP_SCANNER_H

// This is the header file for the scanner

#include "defs.h"

namespace myComp {
    class Scanner {
    private:
        // The next type
        Token token;

        // The input file
        std::ifstream input;

        // Whether we should put the character back
        bool putback = false;

        // The character to put back
        char putback_char = '\n';

        // The current line number
        int line = 1;

        // List of keywords
        static const std::map<std::string, TokenType> keywords;

        // Get the next character from the input
        char next_char();

        // Skip whitespace and get the first character
        char skip_white_space();

        // Scan an integer literal
        int scan_int(char c);

        // Scan a character literal
        int scan_char();

        // Scan a string literal
        std::string scan_string();

        // Scan an identifier
        std::string scan_identifier(char c);

        // Put back a character
        void doPutback(char c);

    public:
        // Set the input file
        void set_input(const std::string &filename);

        // Scan the next type from the input
        void next();

        // Get the data type and fetch the next token
        DataType next_data_type();

        // Get an identifier and fetch the next token
        std::string next_string();

        // Get an integer literal and fetch the next token
        int next_int_literal();

        // Match an expected token
        void match(TokenType expected, const char *name);

        // Match a semi-colon
        void semi() { Scanner::match(TokenType::SEMI, ";"); }

        // Match an identifier
        void identifier() { Scanner::match(TokenType::IDENTIFIER, "identifier"); }

        // Match an equal sign
        void equals() { Scanner::match(TokenType::ASSIGN, "="); }

        // Match a left brace
        void lbrace() { Scanner::match(TokenType::LBRACE, "{"); }

        // Match a right brace
        void rbrace() { Scanner::match(TokenType::RBRACE, "}"); }

        // Match a left parenthesis
        void lparen() { Scanner::match(TokenType::LPAREN, "("); }

        // Match a right parenthesis
        void rparen() { Scanner::match(TokenType::RPAREN, ")"); }

        // Match a left bracket
        void lbracket() { Scanner::match(TokenType::LBRACKET, "["); }

        // Match a right bracket
        void rbracket() { Scanner::match(TokenType::RBRACKET, "]"); }

        // Match a comma
        void comma() { Scanner::match(TokenType::COMMA, ","); }

        // Tell if current token is a data type
        bool is_data_type() const;

        // Tell if current token is an operator
        bool is_operator() const;

        // Tell if current token is a prefix operator
        bool is_prefix_operator() const;

        // Tell if current token is a postfix operator
        bool is_postfix_operator() const;

        // Get the current token
        [[nodiscard]] const Token &get_token() const { return this->token; }

        // Get line number
        [[nodiscard]] int get_line() const { return this->line; }

        // Destructor: close the input file
        ~Scanner() { input.close(); }
    };

} // myComp

#endif //MYCOMP_SCANNER_H
