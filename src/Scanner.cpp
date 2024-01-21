#include "defs.h"
#include "objects.h"
#include "data.h"

namespace myComp {
    const std::map<std::string, TokenType> Scanner::keywords = {
        // Types
        {"void", TokenType::VOID},
        {"int", TokenType::INT},
        {"char", TokenType::CHAR},
        {"long", TokenType::LONG},
        // Control flow
        {"if", TokenType::IF},
        {"else", TokenType::ELSE},
        {"while", TokenType::WHILE},
        {"for", TokenType::FOR},
        {"return", TokenType::RETURN},
    };

    void Scanner::set_input(const std::string &filename) {
        // Open the input file
        this->input.open(filename);

        // If the file cannot be opened, throw an error
        if (!this->input.is_open())
            Errors::fatal_error("cannot open input file " + filename);
    }

    void Scanner::next() {
        // Skip whitespace and get the first character
        char ch = skip_white_space();

        // Determine the type based on the first character
        switch (ch) {
            case EOF:
                this->token.type = TokenType::T_EOF;
                return;
            case '*':
                this->token.type = TokenType::STAR;
                return;
            case '/':
                this->token.type = TokenType::SLASH;
                return;
            case '^':
                this->token.type = TokenType::XOR;
                return;
            case '~':
                this->token.type = TokenType::INVERT;
                return;
            case ';':
                this->token.type = TokenType::SEMI;
                return;
            case ',':
                this->token.type = TokenType::COMMA;
                return;
            case '{':
                this->token.type = TokenType::LBRACE;
                return;
            case '}':
                this->token.type = TokenType::RBRACE;
                return;
            case '(':
                this->token.type = TokenType::LPAREN;
                return;
            case ')':
                this->token.type = TokenType::RPAREN;
                return;
            case '[':
                this->token.type = TokenType::LBRACKET;
                return;
            case ']':
                this->token.type = TokenType::RBRACKET;
                return;
            case '+':
                ch = next_char();
                if (ch == '+')
                    this->token.type = TokenType::INC;
                else {
                    doPutback(ch);
                    this->token.type = TokenType::PLUS;
                }
                return;
            case '-':
                ch = next_char();
                if (ch == '-')
                    this->token.type = TokenType::DEC;
                else {
                    doPutback(ch);
                    this->token.type = TokenType::MINUS;
                }
                return;
            case '|':
                ch = next_char();
                if (ch == '|')
                    this->token.type = TokenType::LOGICAL_OR;
                else {
                    doPutback(ch);
                    this->token.type = TokenType::OR;
                }
                return;
            case '&':
                ch = next_char();
                if (ch == '&')
                    this->token.type = TokenType::LOGICAL_AND;
                else {
                    doPutback(ch);
                    this->token.type = TokenType::AND;
                }
                return;
            case '=':
                ch = next_char();
                if (ch == '=')
                    this->token.type = TokenType::EQUALS;
                else {
                    doPutback(ch);
                    this->token.type = TokenType::ASSIGN;
                }
                return;
            case '!':
                ch = next_char();
                if (ch == '=')
                    this->token.type = TokenType::NEQ;
                else {
                    doPutback(ch);
                    this->token.type = TokenType::NOT;
                }
                return;
            case '<':
                ch = next_char();
                if (ch == '=')
                    this->token.type = TokenType::LESS_EQ;
                else if (ch == '<')
                    this->token.type = TokenType::L_SHIFT;
                else {
                    doPutback(ch);
                    this->token.type = TokenType::LESS;
                }
                return;
            case '>':
                ch = next_char();
                if (ch == '=')
                    this->token.type = TokenType::GREATER_EQ;
                else if (ch == '>')
                    this->token.type = TokenType::R_SHIFT;
                else {
                    doPutback(ch);
                    this->token.type = TokenType::GREATER;
                }
                return;
            case '\'':
                this->token.type = TokenType::INT_LITERAL;
                this->token.val = scan_char();
                if (next_char() != '\'')
                    Errors::syntax_error("Expected single quote at end of char literal", this->line);
                return;
            case '"':
                this->token.type = TokenType::STRING_LITERAL;
                this->token.string = scan_string();
                return;
            default:
                // If the first character is a digit, scan the integer literal
                if (std::isdigit(ch)) {
                    this->token.type = TokenType::INT_LITERAL;
                    this->token.val = scan_int(ch);
                    return;
                }

            // If the first character is a letter or underscore, scan the identifier
                if (std::isalpha(ch) || ch == '_') {
                    this->token.string = scan_identifier(ch);

                    // Determine if the identifier is a keyword
                    if (keywords.contains(this->token.string)) {
                        this->token.type = keywords.at(this->token.string);
                    } else {
                        this->token.type = TokenType::IDENTIFIER;
                    }
                    return;
                }
        }
        // If we reach here, there is an unrecognized token
        Errors::syntax_error("unrecognized token " + std::string(1, ch), this->line);
    }

    char Scanner::skip_white_space() {
        char ch = next_char();

        // Skip whitespace
        while (std::isspace(ch))
            ch = next_char();

        return ch;
    }

    char Scanner::next_char() {
        // If we have a character put back, return it
        if (this->putback) {
            this->putback = false;
            return this->putback_char;
        }

        // Otherwise, get the next character from the input
        char ch = static_cast<char>(this->input.get());

        // If we reach the end of the line, post_increment the line number
        if (ch == '\n')
            this->line++;

        return ch;
    }

    int Scanner::scan_int(char c) {
        int k = 0;

        // Convert each character into an integer and add it to the total
        while (std::isdigit(c)) {
            k = k * 10 + (c - '0');
            c = next_char();
        }

        // Put back the last character
        doPutback(c);

        return k;
    }

    std::string Scanner::scan_identifier(char c) {
        std::string identifier;

        // Add each character to the identifier
        while (std::isalnum(c) || c == '_') {
            identifier += c;
            c = next_char();
        }

        // Put back the last character
        doPutback(c);

        return identifier;
    }

    void Scanner::doPutback(char c) {
        putback = true;
        putback_char = static_cast<char>(c);
    }

    void Scanner::match(TokenType expected, const char *name) {
        if (this->token.type != expected) {
            std::cerr << "Syntax error: expected " << name << " but got token " << token_str.at(this->token.type)
                    << " on line "
                    << this->line << '\n';
            exit(1);
        }
        // Get the next token
        this->next();
    }

    DataType Scanner::next_data_type() {
        DataType ret = token_to_data.at(this->token.type);
        this->next();
        while (this->token.type == TokenType::STAR) {
            ret = data_to_ptr.at(ret);
            this->next();
        }
        return ret;
    }

    std::string Scanner::next_string() {
        std::string ret = this->token.string;
        this->next();
        return ret;
    }

    int Scanner::next_int_literal() {
        int ret = this->token.val;
        this->next();
        return ret;
    }

    int Scanner::scan_char() {
        char ch = next_char();
        if (ch == '\\') {
            switch (ch = next_char()) {
                case 'a':
                    return '\a';
                case 'b':
                    return '\b';
                case 'f':
                    return '\f';
                case 'n':
                    return '\n';
                case 'r':
                    return '\r';
                case 't':
                    return '\t';
                case 'v':
                    return '\v';
                case '\\':
                    return '\\';
                case '"':
                    return '"';
                case '\'':
                    return '\'';
                default:
                    Errors::syntax_error(std::string("got unexpected character ") + ch, this->line);
            }
        }
        return ch;
    }

    std::string Scanner::scan_string() {
        std::string str;
        char ch;
        while ((ch = static_cast<char>(scan_char())) != '"') {
            str += ch;
        }
        return str;
    }

    bool Scanner::is_data_type() const {
        return token_to_data.contains(this->token.type);
    }

    bool Scanner::is_prefix_operator() const {
        return this->token.type == TokenType::STAR
               || this->token.type == TokenType::AND
               || this->token.type == TokenType::INVERT
               || this->token.type == TokenType::NOT
               || this->token.type == TokenType::INC
               || this->token.type == TokenType::DEC
               || this->token.type == TokenType::MINUS;
    }

    bool Scanner::is_postfix_operator() const {
        return this->token.type == TokenType::INC
               || this->token.type == TokenType::DEC;
    }

    bool Scanner::is_operator() const {
        return token_to_op.contains(this->token.type);
    }
} // myComp
