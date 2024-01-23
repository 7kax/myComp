#ifndef MYCOMP_ERRORS_H
#define MYCOMP_ERRORS_H

#include "defs.h"

namespace myComp {
    class Errors {
    public:
        // Error message
        static void syntax_error(const std::string &msg, int line);

        static void unexpected_token(Token *token, int line);

        static void expected(const std::string &msg, int line);

        static void fatal_error(const std::string &msg);
    };
} // myComp

#endif //MYCOMP_ERRORS_H
