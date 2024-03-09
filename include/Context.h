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

    std::string str() const {
        std::string ret = return_type_->str();
        ret += " " + name_ + "(";
        for (auto &param : parameters_) {
            ret += param->str() + ", ";
        }
        if (!parameters_.empty()) {
            ret.pop_back();
            ret.pop_back();
        }
        ret += ")";
        return ret;
    }
};

class FunctionManager {
  public:
    static bool exists(const std::string &name) {
        auto &cache = getCache();
        return cache.find(name) != cache.end();
    }

    static void ensure_exists(const std::string &name) {
        if (!exists(name)) {
            throw std::runtime_error("function " + name + " not defined");
        }
    }

    static FunctionPrototype *find(const std::string &name) {
        auto &cache = getCache();
        auto it = cache.find(name);
        if (it == cache.end()) {
            return nullptr;
        }
        return it->second.get();
    }

    static void insert(Type *return_type, const std::string &name,
                       std::vector<Variable *> parameters) {
        auto &cache = getCache();
        auto it = cache.find(name);
        if (it != cache.end()) {
            throw std::runtime_error("function " + name + " already defined");
        }
        auto func = std::make_unique<FunctionPrototype>();
        func->return_type_ = return_type;
        func->name_ = name;
        func->parameters_ = std::move(parameters);
        cache[name] = std::move(func);
    }

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
