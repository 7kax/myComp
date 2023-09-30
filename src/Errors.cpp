#include "defs.h"
#include "objects.h"
#include "data.h"

namespace myComp {
    void Errors::syntax_error(const std::string &msg, int line) {
        std::cerr << "Syntax error: " << msg << " on line " << line << '\n';
        exit(1);
    }

    void Errors::unexpected_token(TokenType token_type, int line) {
        std::cerr << "Syntax error: unexpected token: " << token_str.at(token_type) << " on line " << line << '\n';
        exit(1);
    }

    void Errors::expected(const std::string &msg, int line) {
        std::cerr << "Syntax error: expected " << msg << " on line " << line << '\n';
        exit(1);
    }

    void Errors::fatal_error(const std::string &msg) {
        std::cerr << "Fatal error: " << msg << '\n';
        exit(1);
    }
} // myComp