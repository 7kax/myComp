#include "defs.h"
#include "objects.h"
#include "data.h"

namespace myComp {
    Function::Function() {
        Type_ *void_type = TypeFactory::getVoidType();
        Type_ *long_type = TypeFactory::getIntegerType(8, true);
        this->function_prototypes.emplace(
            "printint", FunctionPrototype{void_type, {{long_type, "x"}}});
        this->function_prototypes.emplace(
            "printchar", FunctionPrototype{void_type, {{long_type, "x"}}});
    }
} // namespace myComp
