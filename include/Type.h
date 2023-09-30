#ifndef MYCOMP_TYPE_H
#define MYCOMP_TYPE_H

#include "defs.h"

namespace myComp {

    class Type {
    public:
        static const std::map<DataType, int> data_size;

        static bool is_integer(DataType data_type);

        static bool is_pointer(DataType data_type);

        static void integer_widen(ASTNode *&left, ASTNode *&right);

        static void integer_scale(ASTNode *&pointer, ASTNode *&integer);

        static bool convertable_to(DataType src, DataType dest);

        static DataType get_data_type(const std::string &name);
    };

} // myComp

#endif //MYCOMP_TYPE_H
