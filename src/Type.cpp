#include "Type.h"
#include "Errors.h"
namespace {
bool is_double(const myComp::Type *type) {
    return type->is_float() && type->size() == 8;
}

bool is_float(const myComp::Type *type) {
    return type->is_float() && type->size() == 4;
}

bool same_signedness(const myComp::Type *a, const myComp::Type *b) {
    return !(a->is_signed() ^ b->is_signed());
}
} // namespace

namespace myComp {

std::string SignedIntegerType::str() const {
    switch (this->_size) {
    case 1:
        return "signed char";
    case 2:
        return "short";
    case 4:
        return "int";
    case 8:
        return "long";
    default:
        throw std::runtime_error("Invalid integer size");
    }
}

std::string UnsignedIntegerType::str() const {
    switch (this->_size) {
    case 1:
        return "unsigned char";
    case 2:
        return "unsigned short";
    case 4:
        return "unsigned int";
    case 8:
        return "unsigned long";
    default:
        throw std::runtime_error("Invalid integer size");
    }
}

std::string FloatType::str() const {
    switch (this->_size) {
    case 4:
        return "float";
    case 8:
        return "double";
    default:
        throw std::runtime_error("Invalid floating point size");
    }
}

bool convertable_to(Type *src, Type *dest) {
    if (src == dest) {
        return true;
    }
    if (src->is_pointer() && dest->is_pointer()) {
        return true;
    }
    if (src->is_integer() && dest->is_pointer()) {
        return true;
    }
    if (src->is_pointer() && dest->is_integer()) {
        return true;
    }
    if (src->is_arithmetic() && dest->is_arithmetic()) {
        return true;
    }
    return false;
}

bool is_compatible(Type *a, Type *b) {
    if (a == b) {
        return true;
    }
    if (a->is_pointer() && b->is_pointer()) {
        auto *pa = dynamic_cast<PointerType *>(a);
        auto *pb = dynamic_cast<PointerType *>(b);
        return is_compatible(pa->pointee(), pb->pointee());
    }
    if (a->is_array() && b->is_array()) {
        auto *pa = dynamic_cast<ArrayType *>(a);
        auto *pb = dynamic_cast<ArrayType *>(b);
        return is_compatible(pa->element(), pb->element()) &&
               (pa->size() == pb->size() || pa->size() == 0 || pb->size() == 0);
    }
    if (a->is_enum() && b->is_enum()) {
        auto *ea = dynamic_cast<EnumType *>(a);
        auto *eb = dynamic_cast<EnumType *>(b);
        return ea->fields() == eb->fields();
    }
    if (a->is_struct() && b->is_struct()) {
        auto *sa = dynamic_cast<StructType *>(a);
        auto *sb = dynamic_cast<StructType *>(b);
        if (sa->fields().size() != sb->fields().size()) {
            return false;
        }
        for (size_t i = 0; i < sa->fields().size(); i++) {
            if (!is_compatible(sa->fields()[i].first, sb->fields()[i].first) ||
                sa->fields()[i].second != sb->fields()[i].second) {
                return false;
            }
        }
        return true;
    }
    if (a->is_union() && b->is_union()) {
        auto *ua = dynamic_cast<UnionType *>(a);
        auto *ub = dynamic_cast<UnionType *>(b);
        if (ua->fields().size() != ub->fields().size()) {
            return false;
        }
        for (size_t i = 0; i < ua->fields().size(); i++) {
            if (!is_compatible(ua->fields()[i].first, ub->fields()[i].first) ||
                ua->fields()[i].second != ub->fields()[i].second) {
                return false;
            }
        }
        return true;
    }
    return false;
}

Type *integer_promotion(Type *type) {
    if (type->size() > 4)
        return type;
    if (type->is_unsigned() && type->size() == 4)
        return type;
    return TypeFactory::get_signed(4);
}

Type *usual_arithmetic_conversion(Type *a, Type *b) {
    if (!(a->is_arithmetic() && b->is_arithmetic())) {
        throw InvalidException("type of usual arithmetic conversion");
    }
    if (is_double(a) || is_double(b)) {
        return TypeFactory::get_float(8);
    }
    if (is_float(a) || is_float(b)) {
        return TypeFactory::get_float(4);
    }
    a = integer_promotion(a);
    b = integer_promotion(b);
    if (a == b) {
        return a;
    }
    if (same_signedness(a, b)) {
        if (a->size() > b->size())
            return a;
        return b;
    }
    Type *unsigned_type, *signed_type;
    if (a->is_unsigned()) {
        unsigned_type = a;
        signed_type = b;
    } else {
        unsigned_type = b;
        signed_type = a;
    }
    if (unsigned_type->size() >= signed_type->size()) {
        return unsigned_type;
    }
    return TypeFactory::get_unsigned(signed_type->size());
}

Type *int_literal_type(long long val) {
    if (val >= -2147483648 && val <= 2147483647)
        return TypeFactory::get_signed(4);
    return TypeFactory::get_signed(8);
}
} // namespace myComp