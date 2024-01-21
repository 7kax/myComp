#include "defs.h"
#include "objects.h"
#include "data.h"

namespace myComp {
    ASTNode *Tree::int_literal(int value) {
        if (value >= 0 && value <= 255)
            return new ASTNode(ASTNodeType::INT_LITERAL, value, {}, nullptr, nullptr, DataType::CHAR);
        else
            return new ASTNode(ASTNodeType::INT_LITERAL, value, {}, nullptr, nullptr, DataType::INT);
    }

    ASTNode *Tree::join(ASTNodeType type, ASTNode *left, ASTNode *right) {
        return new ASTNode(type, 0, {}, left, right);
    }

    ASTNode *Tree::unary(ASTNodeType type, ASTNode *left) {
        return new ASTNode(type, 0, {}, left, nullptr);
    }

    ASTNode *Tree::variable_declaration(std::string identifier, DataType data_type, int size) {
        return new ASTNode(ASTNodeType::VARIABLE_DECLARATION, size, std::move(identifier), nullptr, nullptr, data_type);
    }

    ASTNode *Tree::function_declaration(std::string identifier, ASTNode *code_block) {
        return new ASTNode(ASTNodeType::FUNCTION_DECLARATION, 0, std::move(identifier), code_block, nullptr,
                           DataType::NONE);
    }

    ASTNode *Tree::function_call(const std::string &identifier, ASTNode *parameters) {
        return new ASTNode(ASTNodeType::FUNCTION_CALL, 0, identifier, parameters, nullptr,
                           function.get_prototype(identifier).return_type);
    }

    ASTNode *Tree::variable(const std::string &identifier) {
        return new ASTNode(ASTNodeType::VARIABLE, 0, identifier, nullptr, nullptr,
                           Type::get_data_type(identifier));
    }

    ASTNode *Tree::parameter(ASTNode *_expression) {
        return new ASTNode(ASTNodeType::GLUE, 0, {}, nullptr, _expression, _expression->data_type);
    }

    ASTNode *Tree::dereference(ASTNode *address) {
        return new ASTNode(ASTNodeType::DEREFERENCE, 0, {}, address, nullptr, ptr_to_data.at(address->data_type));
    }

    ASTNode *Tree::address(ASTNode *left) {
        left->type = ASTNodeType::ADDRESS;
        left->data_type = data_to_ptr.at(left->data_type);
        return left;
    }

    // void Tree::print(std::ostream &out, const ASTNode *node, int indent) {
    //     for (int i = 0; i < indent; ++i) {
    //         out << '\t';
    //     }
    //
    //     out << ASTNode_str.at(node->type)
    //         << ": (left: "
    //         << (node->left == nullptr ? "null" : ASTNode_str.at(node->left->type))
    //         << ", right: "
    //         << (node->right == nullptr ? "null" : ASTNode_str.at(node->right->type))
    //         << ")\n";
    //
    //     if (node->left != nullptr)
    //         Tree::print(out, node->left, indent + 1);
    //     if (node->right != nullptr)
    //         Tree::print(out, node->right, indent + 1);
    // }

    ASTNode *Tree::dereference(ASTNode *address, ASTNode *offset) {
        DataType data_type = address->data_type;
        if (!Type::is_pointer(data_type))
            data_type = data_to_ptr.at(data_type);
        return new ASTNode(ASTNodeType::DEREFERENCE, 0, {}, address, offset, ptr_to_data.at(data_type));
    }

    ASTNode *Tree::string_literal(const std::string &str) {
        return new ASTNode(ASTNodeType::STRING_LITERAL, 0, str, nullptr, nullptr, DataType::CHAR_PTR);
    }

    ASTNode *Tree::negative(ASTNode *node) {
        if (node->data_type == DataType::CHAR)
            node = new ASTNode(ASTNodeType::WIDEN, 0, {}, node, nullptr, DataType::INT);
        return new ASTNode(ASTNodeType::NEGATIVE, 0, {}, node, nullptr, node->data_type);
    }

    ASTNode *Tree::NOT(ASTNode *node) {
        return new ASTNode(ASTNodeType::NOT, 0, {}, node, nullptr, node->data_type);
    }

    ASTNode *Tree::invert(ASTNode *node) {
        return new ASTNode(ASTNodeType::INVERT, 0, {}, node, nullptr, node->data_type);
    }

    ASTNode *Tree::increment(ASTNode *node, bool is_prefix) {
        if (is_prefix)
            return new ASTNode(ASTNodeType::PRE_INC, 0, {}, node, nullptr, node->data_type);
        else
            return new ASTNode(ASTNodeType::POST_INC, 0, {}, node, nullptr, node->data_type);
    }

    ASTNode *Tree::decrement(ASTNode *node, bool is_prefix) {
        if (is_prefix)
            return new ASTNode(ASTNodeType::PRE_DEC, 0, {}, node, nullptr, node->data_type);
        else
            return new ASTNode(ASTNodeType::POST_DEC, 0, {}, node, nullptr, node->data_type);
    }
} // myComp