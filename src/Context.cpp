#include "Context.h"
#include "Errors.h"

namespace myComp {
std::string FunctionPrototype::str() const {
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

bool FunctionManager::exists(const std::string &name) {
    auto &cache = getCache();
    return cache.find(name) != cache.end();
}

void FunctionManager::ensure_exists(const std::string &name) {
    if (!exists(name)) {
        throw SyntaxException("function " + name + " not defined");
    }
}

FunctionPrototype *FunctionManager::find(const std::string &name) {
    auto &cache = getCache();
    auto it = cache.find(name);
    if (it == cache.end()) {
        return nullptr;
    }
    return it->second.get();
}

void FunctionManager::insert(Type *return_type, const std::string &name,
                             std::vector<Variable *> parameters, bool is_variadic) {
    auto &cache = getCache();
    auto it = cache.find(name);
    if (it != cache.end()) {
        throw SyntaxException("function " + name + " already defined");
    }
    auto func = std::make_unique<FunctionPrototype>();
    func->return_type_ = return_type;
    func->name_ = name;
    func->parameters_ = std::move(parameters);
    func->is_variadic_ = is_variadic;
    cache[name] = std::move(func);
}
} // namespace myComp