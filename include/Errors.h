#ifndef MYCOMP_ERRORS_H
#define MYCOMP_ERRORS_H

#include "Token.h"
#include <exception>
#include <string_view>

namespace myComp {
// Throw this exception when an unexpected token is found
class UnexpectedTokenException : public std::exception {
  public:
    UnexpectedTokenException(TokenType got, int line,
                             std::string_view expected) {
        msg_ = "Expected ";
        msg_ += expected;
        msg_ += " but got token ";
        msg_ += Token::token_str.at(got);
        msg_ += " on line ";
        msg_ += std::to_string(line);
    }

    UnexpectedTokenException(TokenType got, int line, TokenType expected) {
        msg_ = "Expected ";
        msg_ += Token::token_str.at(expected);
        msg_ += " but got token ";
        msg_ += Token::token_str.at(got);
        msg_ += " on line ";
        msg_ += std::to_string(line);
    }

    const char *what() const noexcept override { return msg_.c_str(); }

  private:
    std::string msg_;
};

// Throw this exception when an invalid operation is found
class InvalidException : public std::exception {
  public:
    explicit InvalidException(std::string_view msg) {
        msg_ = "Invalid ";
        msg_ += msg;
    }

    InvalidException(std::string_view msg, int line) {
        msg_ = "Invalid ";
        msg_ += msg;
        msg_ += " on line ";
        msg_ += std::to_string(line);
    }

    const char *what() const noexcept override { return msg_.c_str(); }

  private:
    std::string msg_;
};

// Throw this exception when an unreachable code is reached
class UnreachableException : public std::exception {
  public:
    explicit UnreachableException(std::string_view function_name) {
        msg_ = "Unreachable code reached in function ";
        msg_ += function_name;
    }

    const char *what() const noexcept override { return msg_.c_str(); }

  private:
    std::string msg_;
};

// Throw this exception when a logic error in the code is found
class LogicException : public std::exception {
  public:
    explicit LogicException(std::string_view msg) {
        msg_ = "Logic error: ";
        msg_ += msg;
    }

    const char *what() const noexcept override { return msg_.c_str(); }

  private:
    std::string msg_;
};

// Throw this exception when a syntax error is found
class SyntaxException : public std::exception {
  public:
    explicit SyntaxException(std::string_view msg) {
        msg_ = "Syntax error: ";
        msg_ += msg;
    }

    SyntaxException(std::string_view msg, int line) {
        msg_ = "Syntax error: ";
        msg_ += msg;
        msg_ += " on line ";
        msg_ += std::to_string(line);
    }

    const char *what() const noexcept override { return msg_.c_str(); }

  private:
    std::string msg_;
};

// Throw this exception when an IO error is found
class IOException : public std::exception {
  public:
    explicit IOException(std::string_view msg) {
        msg_ = "IO error: ";
        msg_ += msg;
    }

    const char *what() const noexcept override { return msg_.c_str(); }

  private:
    std::string msg_;
};
} // namespace myComp

#endif