#ifndef COMPILER_AST_AST_H
#define COMPILER_AST_AST_H

#include "Tree/Tree-impl.h"
#include "Compiler/AST/ASNode.h"

#include <memory>

class AST : public Tree<std::unique_ptr<ASNode>>
{
public:
    AST() = default;
    explicit AST(std::unique_ptr<ASNode> value);
};

#endif // COMPILER_AST_AST_H