#include "Expression.h"
#include "Errors.h"
#include "data.h"

using namespace std;

namespace {
using namespace myComp;
using namespace std;

const unordered_map<ASTNodeType, int> lbp = {
    {ASTNodeType::MULTIPLY, 25},   {ASTNodeType::DIVIDE, 25},
    {ASTNodeType::MODULO, 25},     {ASTNodeType::ADD, 23},
    {ASTNodeType::SUBTRACT, 23},   {ASTNodeType::L_SHIFT, 21},
    {ASTNodeType::R_SHIFT, 21},    {ASTNodeType::LESS, 19},
    {ASTNodeType::GREATER, 19},    {ASTNodeType::LESS_EQ, 19},
    {ASTNodeType::GREATER_EQ, 19}, {ASTNodeType::EQUALS, 17},
    {ASTNodeType::NEQ, 17},        {ASTNodeType::AND, 15},
    {ASTNodeType::XOR, 13},        {ASTNodeType::OR, 11},
    {ASTNodeType::LOGICAL_AND, 9}, {ASTNodeType::LOGICAL_OR, 7},
    {ASTNodeType::ASSIGN, 6},
};

const unordered_map<ASTNodeType, int> rbp = {
    {ASTNodeType::MULTIPLY, 26},    {ASTNodeType::DIVIDE, 26},
    {ASTNodeType::MODULO, 26},      {ASTNodeType::ADD, 24},
    {ASTNodeType::SUBTRACT, 24},    {ASTNodeType::L_SHIFT, 22},
    {ASTNodeType::R_SHIFT, 22},     {ASTNodeType::LESS, 20},
    {ASTNodeType::GREATER, 20},     {ASTNodeType::LESS_EQ, 20},
    {ASTNodeType::GREATER_EQ, 20},  {ASTNodeType::EQUALS, 18},
    {ASTNodeType::NEQ, 18},         {ASTNodeType::AND, 16},
    {ASTNodeType::XOR, 14},         {ASTNodeType::OR, 12},
    {ASTNodeType::LOGICAL_AND, 10}, {ASTNodeType::LOGICAL_OR, 8},
    {ASTNodeType::ASSIGN, 5},
};

int get_lbp(ASTNodeType type) { return lbp.at(type); }
int get_rbp(ASTNodeType type) { return rbp.at(type); }

bool is_operator(TokenType type) { return token_to_op.contains(type); }
bool is_prefix_operator(TokenType type) {
    return type == TokenType::STAR || type == TokenType::AND ||
           type == TokenType::INVERT || type == TokenType::NOT ||
           type == TokenType::INC || type == TokenType::DEC ||
           type == TokenType::MINUS || type == TokenType::PLUS;
}
bool is_postfix_operator(TokenType type) {
    return type == TokenType::INC || type == TokenType::DEC;
}
ASTNodeType token_to_op_(TokenType type) { return token_to_op.at(type); }
DereferenceNode *subscript_builder(const string &identifier,
                                   ExpressionNode *index) {
    Variable *var = VariableManager::find(identifier);

    VariableNode *var_node = new VariableNode(var);
    AddNode *add = new AddNode(var_node, index);
    return new DereferenceNode(add);
}

BinaryExpressionNode *binary_builder(ASTNodeType type, ExpressionNode *left,
                                     ExpressionNode *right) {
    switch (type) {
    case ASTNodeType::ADD:
        return new AddNode(left, right);
    case ASTNodeType::SUBTRACT:
        return new SubtractNode(left, right);
    case ASTNodeType::MULTIPLY:
        return new MultiplyNode(left, right);
    case ASTNodeType::DIVIDE:
        return new DivideNode(left, right);
    case ASTNodeType::MODULO:
        return new ModuloNode(left, right);
    case ASTNodeType::OR:
        return new OrNode(left, right);
    case ASTNodeType::AND:
        return new AndNode(left, right);
    case ASTNodeType::XOR:
        return new XorNode(left, right);
    case ASTNodeType::L_SHIFT:
        return new LeftShiftNode(left, right);
    case ASTNodeType::R_SHIFT:
        return new RightShiftNode(left, right);
    case ASTNodeType::LESS:
        return new LessNode(left, right);
    case ASTNodeType::GREATER:
        return new GreaterNode(left, right);
    case ASTNodeType::LESS_EQ:
        return new LessEqualsNode(left, right);
    case ASTNodeType::GREATER_EQ:
        return new GreaterEqualsNode(left, right);
    case ASTNodeType::EQUALS:
        return new EqualsNode(left, right);
    case ASTNodeType::NEQ:
        return new NotEqualsNode(left, right);
    case ASTNodeType::LOGICAL_OR:
        return new LogicalOrNode(left, right);
    case ASTNodeType::LOGICAL_AND:
        return new LogicalAndNode(left, right);
    case ASTNodeType::ASSIGN:
        return new AssignNode(left, right);
    default:
        throw InvalidException("binary operator");
    }
}

UnaryExpressionNode *unary_builder(ASTNodeType type, ExpressionNode *oprand) {
    switch (type) {
    case ASTNodeType::INVERT:
        return new InvertNode(oprand);
    case ASTNodeType::NOT:
        return new NotNode(oprand);
    case ASTNodeType::ADDRESS:
        return new AddressNode(oprand);
    case ASTNodeType::DEREFERENCE:
        return new DereferenceNode(oprand);
    case ASTNodeType::POST_INC:
        return new PostIncrementNode(oprand);
    case ASTNodeType::POST_DEC:
        return new PostDecrementNode(oprand);
    case ASTNodeType::PRE_INC:
        return new PreIncrementNode(oprand);
    case ASTNodeType::PRE_DEC:
        return new PreDecrementNode(oprand);
    case ASTNodeType::NEGATIVE:
        return new NegativeNode(oprand);
    case ASTNodeType::POSITIVE:
        return new PositiveNode(oprand);
    default:
        throw InvalidException("unary operator");
    }
}
} // namespace

namespace myComp {

// Implement the Pratt Parse algorithm
ExpressionNode *Expression::build_tree(int pre_precedence) {
    ExpressionNode *left = primary();

    while (true) {
        // If the next token is not an operator, return
        if (!is_operator(token_processor_->peek_type()))
            break;

        // Get the binary operator
        ASTNodeType operator_type = token_to_op_(token_processor_->peek_type());

        // If the precedence is not high enough, return
        if (get_lbp(operator_type) < pre_precedence)
            break;

        // Fetch the next token
        auto [token, line] = token_processor_->next_token();

        // Build the right node
        ExpressionNode *right = build_tree(get_rbp(operator_type));

        // Join the tree
        left = binary_builder(operator_type, left, right);
    }

    return left;
}

ExpressionNode *Expression::primary() {
    // If there is a prefix operator, return the prefix node
    if (ExpressionNode *pref = prefix(); pref != nullptr)
        return pref;

    switch (token_processor_->peek_type()) {
    case TokenType::INT_LITERAL: {
        long long val = token_processor_->next_integer();
        return new LiteralNode(int_literal_type(val), val);
    }
    case TokenType::STRING_LITERAL: {
        string str = token_processor_->next_string();
        string_literals.insert({str, {}});
        return new LiteralNode(
            TypeFactory::get_pointer(TypeFactory::get_char()), str);
    }
    case TokenType::IDENTIFIER: {
        return identifier();
    }
    case TokenType::LPAREN: {
        token_processor_->lparen();

        ExpressionNode *node = build_tree(Expression::MAX_PRECEDENCE);

        token_processor_->rparen();

        return node;
    }
    default: {
        auto [type, line] = token_processor_->peek_token();
        throw UnexpectedTokenException(type->type(), line,
                                       "primary expression");
    }
    }
    return nullptr;
}

ExpressionNode *Expression::prefix() {
    // If there is no prefix operator, return nullptr
    if (!is_prefix_operator(token_processor_->peek_type()))
        return nullptr;

    auto [token, line] = token_processor_->next_token();
    ExpressionNode *node = primary();

    switch (token->type()) {
    case TokenType::STAR:
        return unary_builder(ASTNodeType::DEREFERENCE, node);
    case TokenType::AND:
        return unary_builder(ASTNodeType::ADDRESS, node);
    case TokenType::MINUS:
        return unary_builder(ASTNodeType::NEGATIVE, node);
    case TokenType::PLUS:
        return unary_builder(ASTNodeType::POSITIVE, node);
    case TokenType::NOT:
        return unary_builder(ASTNodeType::NOT, node);
    case TokenType::INVERT:
        return unary_builder(ASTNodeType::INVERT, node);
    case TokenType::INC:
        return unary_builder(ASTNodeType::PRE_INC, node);
    case TokenType::DEC:
        return unary_builder(ASTNodeType::PRE_DEC, node);
    default:
        throw InvalidException("prefix operator");
    }

    return nullptr;
}

ExpressionNode *Expression::identifier() {
    string str = token_processor_->next_identifier();

    switch (token_processor_->peek_type()) {
    case TokenType::LPAREN: {
        // Ensure the function prototype exists
        FunctionManager::ensure_exists(str);

        token_processor_->lparen();

        vector<ExpressionNode *> arguments = parse_arguments();

        token_processor_->rparen();

        return new FunctionCallNode(str, arguments);
    }
    case TokenType::LBRACKET: {
        token_processor_->lbracket();

        ExpressionNode *node = build_tree(Expression::MAX_PRECEDENCE);

        token_processor_->rbracket();
        return subscript_builder(str, node);
    }
    default: {
        if (ExpressionNode *postf = postfix(str); postf != nullptr)
            return postf;
        Variable *var = VariableManager::find(str);

        return new VariableNode(var);
    }
    }
}

vector<ExpressionNode *> Expression::parse_arguments() {
    vector<ExpressionNode *> arguments;
    while (token_processor_->peek_type() != TokenType::RPAREN) {
        ExpressionNode *param = build_tree(Expression::MAX_PRECEDENCE);
        arguments.push_back(param);
        if (token_processor_->peek_type() != TokenType::RPAREN)
            token_processor_->comma();
    }

    return arguments;
}

ExpressionNode *Expression::postfix(const string &identifier) {
    if (!is_postfix_operator(token_processor_->peek_type()))
        return nullptr;

    Variable *var = VariableManager::find(identifier);

    auto [token, line] = token_processor_->next_token();
    switch (token->type()) {
    case TokenType::INC:
        return new PostIncrementNode(new VariableNode(var));
    case TokenType::DEC:
        return new PostDecrementNode(new VariableNode(var));
    default:
        throw InvalidException("postfix operator");
    }

    return nullptr;
}
} // namespace myComp
