#ifndef MYCOMP_SCANNER_H
#define MYCOMP_SCANNER_H

// This is the header file for the scanner

#include "defs.h"

namespace myComp {
    class Scanner {
    private:
        // The next type
        Token *token = nullptr;

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

        // Scan the next token from the input
        void next();

        // Get the current token
        [[nodiscard]] Token *get_token() { return this->token; }

        // Get line number
        [[nodiscard]] int get_line() const { return this->line; }

        // Destructor: close the input file
        ~Scanner() { input.close(); }
    };
} // namespace myComp

#endif // MYCOMP_SCANNER_H
