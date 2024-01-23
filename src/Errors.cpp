#include "defs.h"
#include "objects.h"
#include "data.h"

namespace myComp {
    void Errors::syntax_error(const std::string &msg, const int line) {
        throw std::runtime_error("Syntax error: " + msg + " on line " + std::to_string(line));
    }

    void Errors::unexpected_token(Token *token, const int line) {
        // std::cerr << "Syntax error: unexpected token: " << token_str.at(token_type) << " on line " << line << '\n';
        // exit(1);
        throw std::runtime_error(
            "Syntax error: unexpected token: " + token->str() + " on line " +
            std::to_string(line));
    }

    void Errors::expected(const std::string &msg, const int line) {
        // std::cerr << "Syntax error: expected " << msg << " on line " << line << '\n';
        // exit(1);
        throw std::runtime_error("Syntax error: expected " + msg + " on line " + std::to_string(line));
    }

    void Errors::fatal_error(const std::string &msg) {
        // std::cerr << "Fatal error: " << msg << '\n';
        // exit(1);
        throw std::runtime_error("Fatal error: " + msg);
    }
} // myComp
