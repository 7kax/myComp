#include "defs.h"
#include "objects.h"
#include "data.h"

namespace myComp {
    const std::map<DataType, int> Type::data_size = {
            {DataType::CHAR,     1},
            {DataType::INT,      4},
            {DataType::LONG,     8},
            {DataType::CHAR_PTR, 8},
            {DataType::INT_PTR,  8},
            {DataType::LONG_PTR, 8},
            {DataType::VOID_PTR, 8},
            {DataType::NONE,     0}
    };


    bool myComp::Type::is_integer(DataType data_type) {
        return data_type == DataType::INT
               || data_type == DataType::CHAR
               || data_type == DataType::LONG;
    }

    bool Type::is_pointer(DataType data_type) {
        return data_type == DataType::INT_PTR
               || data_type == DataType::CHAR_PTR
               || data_type == DataType::LONG_PTR
               || data_type == DataType::VOID_PTR;
    }

    void Type::integer_widen(ASTNode *&left, ASTNode *&right) {
        int left_size = Type::data_size.at(left->data_type);
        int right_size = Type::data_size.at(right->data_type);

        if (left_size == right_size)
            return;

        if (left_size > right_size) {
            right = Tree::unary(ASTNodeType::WIDEN, right);
            right->data_type = left->data_type;
        } else {
            left = Tree::unary(ASTNodeType::WIDEN, left);
            left->data_type = right->data_type;
        }
    }

    void Type::integer_scale(ASTNode *&pointer, ASTNode *&integer) {
        int size = Type::data_size.at(ptr_to_data.at(pointer->data_type));

        integer = Tree::unary(ASTNodeType::SCALE, integer);
        integer->data_type = pointer->data_type;
        integer->val = size;
    }

    bool Type::convertable_to(DataType src, DataType dest) {
        if (Type::is_integer(src) && Type::is_integer(dest))
            return true;
        if (Type::is_pointer(src) && Type::is_pointer(dest))
            return true;
        return false;
    }

    DataType Type::get_data_type(const std::string &name) {
        if (!context.is_global() && symbol_tables[context.get_name()].contains(name))
            return symbol_tables[context.get_name()].get_data_type(name);
        return symbol_tables["global"].get_data_type(name);
    }
} // myComp