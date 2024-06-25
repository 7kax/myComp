#ifndef TYPE_H
#define TYPE_H

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>

namespace myComp {
// Base class for all types
class Type {
  public:
    virtual ~Type() = default;

    virtual std::string str() const = 0;

    virtual bool is_void() const = 0;

    virtual bool is_basic() const = 0;

    virtual bool is_char() const = 0;

    virtual bool is_signed() const = 0;

    virtual bool is_unsigned() const = 0;

    virtual bool is_float() const = 0;

    virtual bool is_enum() const = 0;

    virtual bool is_derived() const = 0;

    virtual bool is_array() const = 0;

    virtual bool is_struct() const = 0;

    virtual bool is_union() const = 0;

    virtual bool is_pointer() const = 0;

    virtual size_t size() const = 0;

    virtual size_t alignment() const = 0;

    bool is_integer() const {
        return this->is_char() || this->is_signed() || this->is_unsigned() ||
               this->is_enum();
    }

    bool is_scalar() const {
        return this->is_arithmetic() || this->is_pointer();
    }

    bool is_aggregate() const { return this->is_array() || this->is_struct(); }

    bool is_arithmetic() const {
        return this->is_integer() || this->is_float();
    }
};

// Class for void type
class VoidType : public Type {
  public:
    std::string str() const override { return "void"; }

    bool is_void() const override { return true; }

    bool is_basic() const override { return false; }

    bool is_char() const override { return false; }

    bool is_signed() const override { return false; }

    bool is_unsigned() const override { return false; }

    bool is_float() const override { return false; }

    bool is_enum() const override { return false; }

    bool is_derived() const override { return false; }

    bool is_array() const override { return false; }

    bool is_struct() const override { return false; }

    bool is_union() const override { return false; }

    bool is_pointer() const override { return false; }

    size_t size() const override { return 0; }

    size_t alignment() const override { return 0; }
};

// Base class for base types
class BasicType : public Type {
  public:
    bool is_void() const override { return false; }

    bool is_basic() const override { return true; }

    bool is_enum() const override { return false; }

    bool is_derived() const override { return false; }

    bool is_array() const override { return false; }

    bool is_struct() const override { return false; }

    bool is_union() const override { return false; }

    bool is_pointer() const override { return false; }
};

// Class for char
class CharType : public BasicType {
  public:
    std::string str() const override { return "char"; }

    bool is_char() const override { return true; }

    bool is_signed() const override { return false; }

    bool is_unsigned() const override { return false; }

    bool is_float() const override { return false; }

    size_t size() const override { return 1; }

    size_t alignment() const override { return 1; }
};

// Class for signed integers
class SignedIntegerType : public BasicType {
  public:
    explicit SignedIntegerType(size_t size) : _size(size) {}

    std::string str() const override;

    bool is_char() const override { return this->_size == 1; }

    bool is_signed() const override { return true; }

    bool is_unsigned() const override { return false; }

    bool is_float() const override { return false; }

    size_t size() const override { return this->_size; }

    size_t alignment() const override { return this->_size; }

  private:
    size_t _size;
};

// Class for unsigned integers
class UnsignedIntegerType : public BasicType {
  public:
    explicit UnsignedIntegerType(size_t size) : _size(size) {}

    std::string str() const override;

    bool is_char() const override { return this->_size == 1; }

    bool is_signed() const override { return false; }

    bool is_unsigned() const override { return true; }

    bool is_float() const override { return false; }

    size_t size() const override { return this->_size; }

    size_t alignment() const override { return this->_size; }

  private:
    size_t _size;
};

// Class for floating points
class FloatType : public BasicType {
  public:
    explicit FloatType(size_t size) : _size(size) {}

    std::string str() const override;

    bool is_char() const override { return false; }

    bool is_signed() const override { return false; }

    bool is_unsigned() const override { return false; }

    bool is_float() const override { return true; }

    size_t size() const override { return this->_size; }

    size_t alignment() const override { return this->_size; }

  private:
    size_t _size;
};

// Class for enum
class EnumType : public Type {
  public:
    EnumType(const std::string &name, const std::vector<std::string> &fields)
        : _name(name), _fields(fields) {}

    std::string str() const override { return "enum " + this->_name; }

    bool is_void() const override { return false; }

    bool is_basic() const override { return false; }

    bool is_char() const override { return false; }

    bool is_signed() const override { return false; }

    bool is_unsigned() const override { return false; }

    bool is_float() const override { return false; }

    bool is_enum() const override { return true; }

    bool is_derived() const override { return false; }

    bool is_array() const override { return false; }

    bool is_struct() const override { return false; }

    bool is_union() const override { return false; }

    bool is_pointer() const override { return false; }

    size_t size() const override { return 4; }

    size_t alignment() const override { return 4; }

    const std::vector<std::string> &fields() const { return this->_fields; }

    const std::string &name() const { return this->_name; }

  private:
    std::string _name;
    std::vector<std::string> _fields;
};

// Base class for derived types
class DerivedType : public Type {
  public:
    bool is_void() const override { return false; }

    bool is_basic() const override { return false; }

    bool is_char() const override { return false; }

    bool is_signed() const override { return false; }

    bool is_unsigned() const override { return false; }

    bool is_float() const override { return false; }

    bool is_enum() const override { return false; }

    bool is_derived() const override { return true; }
};

// Class for pointer type
class PointerType : public DerivedType {
  public:
    explicit PointerType(Type *pointee) : _pointee(pointee) {}

    std::string str() const override { return this->_pointee->str() + " *"; }

    bool is_pointer() const override { return true; }

    bool is_struct() const override { return false; }

    bool is_union() const override { return false; }

    bool is_array() const override { return false; }

    size_t size() const override { return 8; }

    size_t alignment() const override { return 8; }

    Type *pointee() const { return this->_pointee; }

  private:
    Type *_pointee;
};

// Class for array types
class ArrayType : public DerivedType {
  public:
    ArrayType(Type *element_type, size_t size)
        : _element_type(element_type), _size(size) {}

    std::string str() const override {
        return this->_element_type->str() + "[" + std::to_string(this->_size) +
               "]";
    }

    bool is_struct() const override { return false; }

    bool is_union() const override { return false; }

    bool is_pointer() const override { return false; }

    bool is_array() const override { return true; }

    size_t size() const override {
        return this->_size * this->_element_type->size();
    }

    size_t alignment() const override { return this->_element_type->size(); }

    Type *element() { return this->_element_type; }

    size_t num_elements() const { return this->_size; }

  private:
    Type *_element_type;
    size_t _size;
};

// Class for struct type
// This implementation is INCOMPLETE!!!
// todo...
class StructType : public DerivedType {
  public:
    StructType(const std::string &name,
               const std::vector<std::pair<Type *, std::string>> &fields)
        : _name(name), _fields(fields) {}

    std::string str() const override { return "struct " + this->_name; }

    bool is_array() const override { return false; }

    bool is_struct() const override { return true; }

    bool is_union() const override { return false; }

    bool is_pointer() const override { return false; }

    size_t size() const override { return 0; }

    size_t alignment() const override { return 0; }

    const std::vector<std::pair<Type *, std::string>> &fields() const {
        return this->_fields;
    }

  private:
    std::string _name;
    std::vector<std::pair<Type *, std::string>> _fields;
};

// Class for union type
// This implementation is INCOMPLETE!!!
// todo...
class UnionType : public DerivedType {
  public:
    UnionType(const std::string &name,
              const std::vector<std::pair<Type *, std::string>> &fields)
        : _name(name), _fields(fields) {}

    std::string str() const override { return "union " + this->_name; }

    bool is_array() const override { return false; }

    bool is_struct() const override { return false; }

    bool is_union() const override { return true; }

    bool is_pointer() const override { return false; }

    size_t size() const override { return 0; }

    size_t alignment() const override { return 0; }

    const std::vector<std::pair<Type *, std::string>> &fields() const {
        return this->_fields;
    }

  private:
    std::string _name;
    std::vector<std::pair<Type *, std::string>> _fields;
};

// Factory to create types
class TypeFactory {
  public:
    static VoidType *get_void() { return getOrCreate<VoidType>("void"); }
    static CharType *get_char() { return getOrCreate<CharType>("char"); }
    static SignedIntegerType *get_signed(size_t size) {
        std::string type_name;
        switch (size) {
        case 1:
            type_name = "signed char";
            break;
        case 2:
            type_name = "short";
            break;
        case 4:
            type_name = "int";
            break;
        case 8:
            type_name = "long";
            break;
        default:
            throw std::runtime_error("Invalid integer size");
        }
        return getOrCreate<SignedIntegerType>(type_name, size);
    }
    static UnsignedIntegerType *get_unsigned(size_t size) {
        std::string type_name;
        switch (size) {
        case 1:
            type_name = "unsigned char";
            break;
        case 2:
            type_name = "unsigned short";
            break;
        case 4:
            type_name = "unsigned int";
            break;
        case 8:
            type_name = "unsigned long";
            break;
        default:
            throw std::runtime_error("Invalid integer size");
        }
        return getOrCreate<UnsignedIntegerType>(type_name, size);
    }
    static FloatType *get_float(size_t size) {
        std::string type_name;
        switch (size) {
        case 4:
            type_name = "float";
            break;
        case 8:
            type_name = "double";
            break;
        default:
            throw std::runtime_error("Invalid floating point size");
        }
        return getOrCreate<FloatType>(type_name, size);
    }
    static ArrayType *get_array(Type *element_type, size_t size) {
        return getOrCreate<ArrayType>(element_type->str() + "[" +
                                          std::to_string(size) + "]",
                                      element_type, size);
    }

    static StructType *
    get_struct(const std::string &name,
               const std::vector<std::pair<Type *, std::string>> &fields) {
        return getOrCreate<StructType>("struct " + name, name, fields);
    }

    static UnionType *
    get_union(const std::string &name,
              const std::vector<std::pair<Type *, std::string>> &fields) {
        return getOrCreate<UnionType>("union " + name, name, fields);
    }

    static EnumType *get_enum(const std::string &name,
                              const std::vector<std::string> &fields) {
        return getOrCreate<EnumType>("enum " + name, name, fields);
    }

    static PointerType *get_pointer(Type *pointee) {
        return getOrCreate<PointerType>(pointee->str() + " *", pointee);
    }

    static PointerType *array_to_pointer(Type *array) {
        auto pointee = dynamic_cast<ArrayType *>(array)->element();
        return get_pointer(pointee);
    }

  private:
    static std::unordered_map<std::string, std::unique_ptr<Type>> &getCache() {
        static std::unordered_map<std::string, std::unique_ptr<Type>> typeCache;
        return typeCache;
    }

    template <typename T, typename... Args>
    static T *getOrCreate(const std::string &typeName, Args &&...args) {
        if (const auto it = getCache().find(typeName); it != getCache().end())
            return static_cast<T *>(it->second.get());
        T *ptr = new T(std::forward<Args>(args)...);
        getCache()[typeName] = std::unique_ptr<Type>(ptr);
        return ptr;
    }
};

// Utility functions
bool convertable_to(Type *src, Type *dest);

bool is_compatible(Type *a, Type *b);

Type *integer_promotion(Type *type);

Type *usual_arithmetic_conversion(Type *a, Type *b);

Type *int_literal_type(long long val);
} // namespace myComp

#endif // TYPE_H
