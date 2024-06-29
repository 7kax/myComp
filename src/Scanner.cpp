#include "Scanner.h"
#include "Errors.h"

using namespace std;

namespace {
using namespace myComp;
using namespace std;

// List of keywords
const unordered_map<string, TokenType> keywords = {
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
} // namespace

namespace myComp {
void Scanner::set_input(const string &filename) {
    // Open the input file
    _input.open(filename);

    // If the file cannot be opened, throw an error
    if (!_input.is_open())
        throw IOException("cannot open file " + filename);
}

void Scanner::next() {
    // Skip whitespace and peek the first character
    int ch = next_char();

    // Determine the type based on the first character
    switch (ch) {
    case char_traits<char>::eof():
        _token = TokenFactory::getToken(TokenType::T_EOF);
        return;
    case '*':
        _token = TokenFactory::getToken(TokenType::STAR);
        return;
    case '/':
        _token = TokenFactory::getToken(TokenType::SLASH);
        return;
    case '%':
        _token = TokenFactory::getToken(TokenType::MOD);
        return;
    case '^':
        _token = TokenFactory::getToken(TokenType::XOR);
        return;
    case '~':
        _token = TokenFactory::getToken(TokenType::INVERT);
        return;
    case ';':
        _token = TokenFactory::getToken(TokenType::SEMI);
        return;
    case ',':
        _token = TokenFactory::getToken(TokenType::COMMA);
        return;
    case '{':
        _token = TokenFactory::getToken(TokenType::LBRACE);
        return;
    case '}':
        _token = TokenFactory::getToken(TokenType::RBRACE);
        return;
    case '(':
        _token = TokenFactory::getToken(TokenType::LPAREN);
        return;
    case ')':
        _token = TokenFactory::getToken(TokenType::RPAREN);
        return;
    case '[':
        _token = TokenFactory::getToken(TokenType::LBRACKET);
        return;
    case ']':
        _token = TokenFactory::getToken(TokenType::RBRACKET);
        return;
    case '+':
        ch = _input.peek();
        if (ch == '+') {
            _input.get();
            _token = TokenFactory::getToken(TokenType::INC);
        } else {
            _token = TokenFactory::getToken(TokenType::PLUS);
        }
        return;
    case '-':
        ch = _input.peek();
        if (ch == '-') {
            _input.get();
            _token = TokenFactory::getToken(TokenType::DEC);
        } else {
            _token = TokenFactory::getToken(TokenType::MINUS);
        }
        return;
    case '|':
        ch = _input.peek();
        if (ch == '|') {
            _input.get();
            _token = TokenFactory::getToken(TokenType::LOGICAL_OR);
        } else {
            _token = TokenFactory::getToken(TokenType::OR);
        }
        return;
    case '&':
        ch = _input.peek();
        if (ch == '&') {
            _input.get();
            _token = TokenFactory::getToken(TokenType::LOGICAL_AND);
        } else {
            _token = TokenFactory::getToken(TokenType::AND);
        }
        return;
    case '=':
        ch = _input.peek();
        if (ch == '=') {
            _input.get();
            _token = TokenFactory::getToken(TokenType::EQUALS);
        } else {
            _token = TokenFactory::getToken(TokenType::ASSIGN);
        }
        return;
    case '!':
        ch = _input.peek();
        if (ch == '=') {
            _input.get();
            _token = TokenFactory::getToken(TokenType::NEQ);
        } else {
            _token = TokenFactory::getToken(TokenType::NOT);
        }
        return;
    case '<':
        ch = _input.peek();
        if (ch == '=') {
            _input.get();
            _token = TokenFactory::getToken(TokenType::LESS_EQ);
        } else if (ch == '<') {
            _input.get();
            _token = TokenFactory::getToken(TokenType::L_SHIFT);
        } else {
            _token = TokenFactory::getToken(TokenType::LESS);
        }
        return;
    case '>':
        ch = _input.peek();
        if (ch == '=') {
            _input.get();
            _token = TokenFactory::getToken(TokenType::GREATER_EQ);
        } else if (ch == '>') {
            _input.get();
            _token = TokenFactory::getToken(TokenType::R_SHIFT);
        } else {
            _token = TokenFactory::getToken(TokenType::GREATER);
        }
        return;
    case '.':
        ch = _input.peek();
        if (ch == '.') {
            _input.get();
            ch = _input.get();
            if (ch != '.')
                break;
            _token = TokenFactory::getToken(TokenType::ELLIPSIS);
        } else {
            _token = TokenFactory::getToken(TokenType::DOT);
        }
        return;
    case '\'':
        _token = TokenFactory::getIntegerLiteral(scan_char());
        return;
    case '"':
        _token = TokenFactory::getStringLiteral(scan_string());
        return;
    default:
        if (isdigit(ch)) {
            _token = TokenFactory::getIntegerLiteral(scan_int(ch));
            return;
        }

        if (isalpha(ch) || ch == '_') {
            if (string str = scan_identifier(ch); keywords.contains(str)) {
                _token = TokenFactory::getToken(keywords.at(str));
            } else {
                _token = TokenFactory::getIdentifier(str);
            }
            return;
        }
    }

    // If we reach here, there is an unrecognized token
    throw SyntaxException("unrecognized character " + to_string(ch), _line);
}

int Scanner::next_char() {
    while (isspace(_input.peek())) {
        if (_input.peek() == '\n')
            _line++;
        _input.get();
    }

    return _input.get();
}

int Scanner::scan_int(int c) {
    int k = c - '0';

    // Convert each character into an integer and add it to the total
    while (isdigit(_input.peek())) {
        c = _input.get();
        k = k * 10 + (c - '0');
    }

    return k;
}

string Scanner::scan_identifier(int c) {
    string identifier{static_cast<char>(c)};

    // Add each character to the identifier
    while (isalnum(_input.peek()) || _input.peek() == '_') {
        identifier += _input.get();
    }

    return identifier;
}

int Scanner::scan_escape_sequence() {
    int ch = _input.get();

    // Deal with escape sequences
    if (ch == '\\') {
        ch = _input.get();
        switch (ch) {
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
            throw SyntaxException("invalid escape sequence", _line);
        }
    }

    return ch;
}

int Scanner::scan_char() {
    int ch = scan_escape_sequence();

    // Ensure that the character is closed
    if (_input.get() != '\'')
        throw SyntaxException("expected closing '", _line);

    return ch;
}

string Scanner::scan_string() {
    string str;
    int ch;
    while ((ch = scan_escape_sequence()) != '"') {
        str += ch;
    }
    return str;
}

} // namespace myComp
