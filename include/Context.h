#ifndef MYCOMP_CONTEXT_H
#define MYCOMP_CONTEXT_H

#include "defs.h"

namespace myComp {
    class Context {
    private:
        std::stack<ContextNode> context_stack;
        
    public:
        Context();

        void push(const std::string &name);

        void push(const std::string &name, const std::string &end_label);

        void pop();

        [[nodiscard]] ContextType get_type() const;

        [[nodiscard]] const std::string &get_name() const;

        [[nodiscard]] const std::string &get_end_label() const;

        [[nodiscard]] bool is_global() const;
    };
}


#endif //MYCOMP_CONTEXT_H
