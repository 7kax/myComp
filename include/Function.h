#ifndef MYCOMP_FUNCTION_H
#define MYCOMP_FUNCTION_H

#include "defs.h"

namespace myComp {

    class Function {
    private:
        std::map<std::string, FunctionPrototype> function_prototypes;

    public:
        Function() {
            this->function_prototypes.emplace("printint",
                                              FunctionPrototype{DataType::VOID, {{DataType::LONG, "x"}}});
            this->function_prototypes.emplace("printchar",
                                              FunctionPrototype{DataType::VOID, {{DataType::LONG, "x"}}});
        }

        // Add a function prototype
        void add_prototype(const std::string &name, DataType return_type,
                           const std::vector<parameter> &params) {
            this->function_prototypes.emplace(name, FunctionPrototype{return_type, params});
        }

        // Get the function prototype
        FunctionPrototype &get_prototype(const std::string &name) {
            return this->function_prototypes.at(name);
        }

        // Tell if a function is defined
        [[nodiscard]] bool is_declared(const std::string &name) const {
            return this->function_prototypes.contains(name);
        }
    };

} // myComp

#endif //MYCOMP_FUNCTION_H
