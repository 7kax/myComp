#ifndef TYPE_H
#define TYPE_H

#include <memory>
#include <string>
#include <unordered_map>

namespace myComp {
  // Base class for all types
  class Type_ {
  public:
    virtual ~Type_() = default;

    virtual std::string str() const = 0;

    virtual bool is_integer() const = 0;

    virtual bool is_pointer() const = 0;

    virtual bool is_void() const = 0;

    virtual bool is_array() const = 0;

    virtual bool is_base_type() const = 0;

    virtual bool convertable_to(const Type_ *dest) const = 0;

    virtual int size() const = 0;
  };

  // Base class for base types
  class BaseType : public Type_ {
  public:
    bool is_pointer() const override { return false; }
    bool is_array() const override { return false; }
    bool is_base_type() const override { return true; }
  };

  // Class for integer types
  class IntegerType : public BaseType {
  public:
    IntegerType(int size, bool is_signed) : _size(size), _is_signed(is_signed) {
    }

    std::string str() const override {
      return integer_str(this->_size, this->_is_signed);
    }

    bool is_integer() const override { return true; }
    bool is_void() const override { return false; }

    bool convertable_to(const Type_ *dest) const override {
      if (dest->is_integer() && dest->size() >= this->size())
        return true;
      return false;
    }

    int size() const override { return this->_size; }

    static std::string integer_str(int size, bool is_signed) {
      std::string s{};
      if (!is_signed)
        s += "unsigned ";
      switch (size) {
        case 1:
          s += "char";
          break;
        case 2:
          s += "short";
          break;
        case 4:
          s += "int";
          break;
        case 8:
          s += "long";
          break;
        default:
          throw std::runtime_error("Invalid integer size");
      }
      return s;
    }

  private:
    int _size; // can be 1, 2, 4, 8
    bool _is_signed;
  };

  // Class for void type
  class VoidType : public BaseType {
  public:
    std::string str() const override { return "void"; }
    bool is_integer() const override { return false; }
    bool is_void() const override { return true; }
    bool convertable_to(const Type_ *dest) const override { return false; }
    int size() const override { return 0; }
  };

  // Base class for compound types
  class CompoundType : public Type_ {
  public:
    bool is_integer() const override { return false; }
    bool is_void() const override { return false; }
    bool is_base_type() const override { return false; }
  };

  // Class for pointer types
  class PointerType : public CompoundType {
  public:
    explicit PointerType(Type_ *pointee) : _pointee(pointee) {
    }

    std::string str() const override { return this->_pointee->str() + " *"; }
    bool is_pointer() const override { return true; }
    bool is_array() const override { return false; }

    bool convertable_to(const Type_ *dest) const override {
      if (dest->is_pointer())
        return true;
      return false;
    }

    int size() const override { return 8; }

    Type_ *pointee() const { return this->_pointee; }

  private:
    Type_ *_pointee;
  };

  // Class for array types
  class ArrayType : public CompoundType {
  public:
    ArrayType(Type_ *element_type, int size)
      : _element_type(element_type), _size(size) {
    }

    std::string str() const override {
      return this->_element_type->str() + "[" + std::to_string(this->_size) + "]";
    }

    bool is_pointer() const override { return false; }
    bool is_array() const override { return true; }

    bool convertable_to(const Type_ *dest) const override {
      if (dest->is_pointer())
        return true;
      return false;
    }

    int size() const override {
      return this->_size * this->_element_type->size();
    }

    Type_ *element_type() const { return this->_element_type; }
    int size_of_element() const { return this->_element_type->size(); }
    int size_of_array() const { return this->_size; }

  private:
    Type_ *_element_type;
    int _size;
  };

  // Factory to create types
  class TypeFactory {
  public:
    static Type_ *getIntegerType(int size, bool is_signed) {
      return getOrCreate<IntegerType>(
        IntegerType::integer_str(size, is_signed), size, is_signed);
    }

    static Type_ *getVoidType() { return getOrCreate<VoidType>("void"); }

    static Type_ *getPointerType(Type_ *pointee) {
      return getOrCreate<PointerType>(pointee->str() + " *", pointee);
    }

    static Type_ *getArrayType(Type_ *element_type, int size) {
      return getOrCreate<ArrayType>(element_type->str() + "[" +
                                    std::to_string(size) + "]",
                                    element_type, size);
    }

    static Type_ *arrayToPointer(Type_ *array) {
      if (!array->is_array())
        throw std::runtime_error("Not an array");
      return getPointerType(
        dynamic_cast<ArrayType *>(array)->element_type());
    }

  private:
    static std::unordered_map<std::string, std::unique_ptr<Type_> > &getCache() {
      static std::unordered_map<std::string, std::unique_ptr<Type_> > typeCache;
      return typeCache;
    }

    template<typename T, typename... Args>
    static Type_ *getOrCreate(const std::string &typeName, Args &&... args) {
      if (const auto it = getCache().find(typeName);
        it != getCache().end())
        return it->second.get();
      // auto type = std::make_unique<T>(std::forward<Args>(args)...);
      Type_ *ptr = new T(std::forward<Args>(args)...);
      // auto ptr = type.get();
      // this->typeCache[typeName] = std::move(type);
      getCache()[typeName] = std::unique_ptr<Type_>(ptr);
      return ptr;
    }
  };
} // namespace myComp

#endif // TYPE_H
