#ifndef MYCOMP_VARIABLE_H
#define MYCOMP_VARIABLE_H

#include "Type.h"

namespace myComp {
struct Variable {
    Type *type = nullptr;
    std::string name;
    std::string scope;

    std::string str() const { return type->str() + " " + name; }
    std::string id() const { return scope + "_" + name; }
};

class VariableManager {
  public:
    static Variable *find(const std::string &name);

    static void insert(Type *type, const std::string &name,
                       const std::string &scope);

    static std::vector<Variable *>
    get_variables_in_scope(const std::string &scope);

  private:
    static std::unordered_map<std::string, std::unique_ptr<Variable>> &
    getCache() {
        static std::unordered_map<std::string, std::unique_ptr<Variable>> cache;
        return cache;
    }
};
} // namespace myComp

#endif // MYCOMP_VARIABLE_H
