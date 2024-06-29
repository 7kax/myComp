#ifndef MYCOMP_SCANNER_H
#define MYCOMP_SCANNER_H

#include <fstream>
#include <map>

#include "Token.h"

namespace myComp {
class Scanner {
  private:
    // The next type
    Token *_token = nullptr;

    // The input file
    std::ifstream _input;

    // The current line number
    int _line = 1;

    // Get the next character from the input
    // Ignore whitespace and increment line number
    int next_char();

    // Scan an integer literal
    int scan_int(int c);

    // Scan a character literal
    int scan_char();

    // Scan a string literal
    std::string scan_string();

    // Scan an identifier
    std::string scan_identifier(int c);

    // Scan an escape sequence
    int scan_escape_sequence();

  public:
    // Set the input file
    void set_input(const std::string &filename);

    // Scan the next token from the input
    void next();

    // Get the current token
    [[nodiscard]] Token *get_token() { return _token; }

    // Get line number
    [[nodiscard]] int get_line() const { return _line; }

    // Destructor: close the input file
    ~Scanner() { _input.close(); }
};
} // namespace myComp

#endif // MYCOMP_SCANNER_H
