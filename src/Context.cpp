#include "defs.h"
#include "objects.h"
#include "data.h"

namespace myComp {

    void Context::push(const std::string &name) {
        this->context_stack.emplace(ContextType::FUNCTION, name, "");
    }

    void Context::push(const std::string &name, const std::string &end_label) {
        this->context_stack.emplace(ContextType::FUNCTION, name, end_label);
    }

    void Context::pop() {
        this->context_stack.pop();
    }

    ContextType Context::get_type() const {
        return this->context_stack.top().type;
    }

    const std::string &Context::get_name() const {
        return this->context_stack.top().name;
    }

    const std::string &Context::get_end_label() const {
        return this->context_stack.top().end_label;
    }

    Context::Context() {
        this->context_stack.emplace(ContextType::GLOBAL, "global", "");
    }

    bool Context::is_global() const {
        return this->context_stack.top().type == ContextType::GLOBAL;
    }
}