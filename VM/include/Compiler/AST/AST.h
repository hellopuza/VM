#ifndef COMPILER_AST_AST_H
#define COMPILER_AST_AST_H

#include "Tree/Tree-impl.h"
#include "Compiler/AST/ASNode.h"

namespace ast {

class AST : public Tree<std::shared_ptr<ASNode>>
{
public:
    AST() = default;
    explicit AST(std::shared_ptr<ASNode> value);

    int dot_dump(const char* dump_name) const;

private:
    void dot_dump(std::ofstream& dump_file) const;
};

} // namespace ast

#endif // COMPILER_AST_AST_H