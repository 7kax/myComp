#include "Init.h"
#include "data.h"
#include "X86_CodeGenerator.h"

namespace myComp {
void Init::init() {
    Context::push("global");

    code_generator = new X86_CodeGenerator();
}

void Init::end() { delete code_generator; }
} // namespace myComp