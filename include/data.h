#ifndef MYCOMP_DATA_H
#define MYCOMP_DATA_H

namespace myComp {
    // Debug flag
    extern bool debug;

    // Convert token to data type
    // extern const std::map<TokenType, DataType> token_to_data;

    // Data type in tokens
    extern const std::set<TokenType> data_types;

    // Precedence of operators
    extern const std::map<ASTNodeType, int> precedence;

    // Convert a token type to an AST node type
    extern const std::map<TokenType, ASTNodeType> token_to_op;

    // Convert a data type to a pointer data type
    // extern const std::map<DataType, DataType> data_to_ptr;

    // Convert a pointer data type to a data type
    // extern const std::map<DataType, DataType> ptr_to_data;

    // Convert an AST node type to a string
    extern const std::map<ASTNodeType, const char *> ASTNode_str;

    // Convert a token type to a string
    extern const std::map<TokenType, const char *> token_str;

    // Convert a data type to a string
    // extern const std::map<DataType, const char *> data_type_str;

    // Info of data types
    // extern const std::map<DataType, TypeInfo> data_type_info;

    // String literals
    extern std::map<std::string, std::string> string_literals;

    // Symbol tables
    extern std::map<std::string, SymbolTable> symbol_tables;

    // Scanner
    extern Scanner scanner;

    // Generate code
    extern Generator generator;

    // Assembly code generator
    extern AsmGenerator *asm_generator;

    // Function
    extern Function function;

    // Context
    extern Context context;

    // Type factory
    // extern TypeFactory type_factory;
} // namespace myComp

#endif // MYCOMP_DATA_H
