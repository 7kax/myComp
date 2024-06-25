#include "Variable.h"
#include "Context.h"
#include "Errors.h"
namespace myComp {
Variable *VariableManager::find(const std::string &name) {
    auto &cache = getCache();

    // Search for the variable in current scope
    auto it = cache.find(Context::get_name() + "_" + name);
    if (it != cache.end()) {
        return it->second.get();
    }

    // Search for the variable in global scope
    it = cache.find("global_" + name);
    if (it != cache.end()) {
        return it->second.get();
    }

    throw LogicException("Variable " + name + " not defined");
}

void VariableManager::insert(Type *type, const std::string &name,
                             const std::string &scope) {
    if (getCache().contains(scope + "_" + name)) {
        throw LogicException("Variable " + name + " already defined" + " in " +
                             scope);
    }

    auto &cache = getCache();
    cache[scope + "_" + name] = std::make_unique<Variable>(type, name, scope);
}

std::vector<Variable *>
VariableManager::get_variables_in_scope(const std::string &scope) {
    std::vector<Variable *> ret;
    auto &cache = getCache();
    for (auto &it : cache) {
        if (it.second->scope == scope) {
            ret.push_back(it.second.get());
        }
    }
    return ret;
}
} // namespace myComp