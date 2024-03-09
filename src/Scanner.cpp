#include "Scanner.h"

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
        throw std::runtime_error("cannot open file " + filename);
}

void Scanner::next() {
    // Skip whitespace and get the first character
    char ch = skip_white_space();

    // Determine the type based on the first character
    switch (ch) {
    case EOF:
        this->token = TokenFactory::getToken(TokenType::T_EOF);
        return;
    case '*':
        this->token = TokenFactory::getToken(TokenType::STAR);
        return;
    case '/':
        this->token = TokenFactory::getToken(TokenType::SLASH);
        return;
    case '^':
        this->token = TokenFactory::getToken(TokenType::XOR);
        return;
    case '~':
        this->token = TokenFactory::getToken(TokenType::INVERT);
        return;
    case ';':
        this->token = TokenFactory::getToken(TokenType::SEMI);
        return;
    case ',':
        this->token = TokenFactory::getToken(TokenType::COMMA);
        return;
    case '{':
        this->token = TokenFactory::getToken(TokenType::LBRACE);
        return;
    case '}':
        this->token = TokenFactory::getToken(TokenType::RBRACE);
        return;
    case '(':
        this->token = TokenFactory::getToken(TokenType::LPAREN);
        return;
    case ')':
        this->token = TokenFactory::getToken(TokenType::RPAREN);
        return;
    case '[':
        this->token = TokenFactory::getToken(TokenType::LBRACKET);
        return;
    case ']':
        this->token = TokenFactory::getToken(TokenType::RBRACKET);
        return;
    case '+':
        ch = next_char();
        if (ch == '+')
            this->token = TokenFactory::getToken(TokenType::INC);
        else {
            doPutback(ch);
            this->token = TokenFactory::getToken(TokenType::PLUS);
        }
        return;
    case '-':
        ch = next_char();
        if (ch == '-')
            this->token = TokenFactory::getToken(TokenType::DEC);
        else {
            doPutback(ch);
            this->token = TokenFactory::getToken(TokenType::MINUS);
        }
        return;
    case '|':
        ch = next_char();
        if (ch == '|')
            this->token = TokenFactory::getToken(TokenType::LOGICAL_OR);
        else {
            doPutback(ch);
            this->token = TokenFactory::getToken(TokenType::OR);
        }
        return;
    case '&':
        ch = next_char();
        if (ch == '&')
            this->token = TokenFactory::getToken(TokenType::LOGICAL_AND);
        else {
            doPutback(ch);
            this->token = TokenFactory::getToken(TokenType::AND);
        }
        return;
    case '=':
        ch = next_char();
        if (ch == '=')
            this->token = TokenFactory::getToken(TokenType::EQUALS);
        else {
            doPutback(ch);
            this->token = TokenFactory::getToken(TokenType::ASSIGN);
        }
        return;
    case '!':
        ch = next_char();
        if (ch == '=')
            this->token = TokenFactory::getToken(TokenType::NEQ);
        else {
            doPutback(ch);
            this->token = TokenFactory::getToken(TokenType::NOT);
        }
        return;
    case '<':
        ch = next_char();
        if (ch == '=')
            this->token = TokenFactory::getToken(TokenType::LESS_EQ);
        else if (ch == '<')
            this->token = TokenFactory::getToken(TokenType::L_SHIFT);
        else {
            doPutback(ch);
            this->token = TokenFactory::getToken(TokenType::LESS);
        }
        return;
    case '>':
        ch = next_char();
        if (ch == '=')
            this->token = TokenFactory::getToken(TokenType::GREATER_EQ);
        else if (ch == '>')
            this->token = TokenFactory::getToken(TokenType::R_SHIFT);
        else {
            doPutback(ch);
            this->token = TokenFactory::getToken(TokenType::GREATER);
        }
        return;
    case '\'':
        this->token = TokenFactory::getIntegerLiteral(scan_char());
        if (next_char() != '\'') {
            throw std::runtime_error("expected closing single quote on line " +
                                     std::to_string(this->line));
        }
        return;
    case '"':
        this->token = TokenFactory::getStringLiteral(scan_string());
        return;
    default:
        if (std::isdigit(ch)) {
            this->token = TokenFactory::getIntegerLiteral(scan_int(ch));
            return;
        }

        if (std::isalpha(ch) || ch == '_') {
            if (std::string str = scan_identifier(ch); keywords.contains(str)) {
                this->token = TokenFactory::getToken(keywords.at(str));
            } else {
                this->token = TokenFactory::getIdentifier(str);
            }
            return;
        }
    }
    // If we reach here, there is an unrecognized token
    throw std::runtime_error(std::string("unexpected character ") + ch +
                             " on line " + std::to_string(this->line));
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
    putback_char = c;
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
            throw std::runtime_error("unknown escape sequence \\" + ch);
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

} // namespace myComp
