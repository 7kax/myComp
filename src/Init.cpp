#include "defs.h"
#include "objects.h"
#include "data.h"

#include "x_86.h"
#include "Init.h"


namespace myComp {
    void Init::init() {
        symbol_tables.emplace("global", SymbolTable{});

        asm_generator = new x_86();
    }

    void Init::end() {
        delete asm_generator;
    }
} // myComp