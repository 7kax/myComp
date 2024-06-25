#ifndef MYCOMP_CONTEXT_H
#define MYCOMP_CONTEXT_H

#include <utility>
#include <stack>

#include "Variable.h"

namespace myComp {
struct ContextNode {
    std::string name_;
    bool has_return_ = false;
};

class Context {
  public:
    static std::string &get_name() { return get_stack().top().name_; }
    static void push(const std::string &name) { get_stack().push({name}); }
    static void pop() { get_stack().pop(); }
    static bool has_return() { return get_stack().top().has_return_; }
    static void set_return_flag() { get_stack().top().has_return_ = true; }

  private:
    static std::stack<ContextNode> &get_stack() {
        static std::stack<ContextNode> stack;
        return stack;
    }
};

struct FunctionPrototype {
    Type *return_type_ = nullptr;
    std::string name_;
    std::vector<Variable *> parameters_;

    std::string str() const;
};

class FunctionManager {
  public:
    static bool exists(const std::string &name);

    static void ensure_exists(const std::string &name);

    static FunctionPrototype *find(const std::string &name);

    static void insert(Type *return_type, const std::string &name,
                       std::vector<Variable *> parameters);

  private:
    static std::unordered_map<std::string, std::unique_ptr<FunctionPrototype>> &
    getCache() {
        static std::unordered_map<std::string,
                                  std::unique_ptr<FunctionPrototype>>
            cache;
        return cache;
    }
};

} // namespace myComp

#endif // MYCOMP_CONTEXT_H
