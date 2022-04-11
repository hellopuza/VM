#include "Compiler/AST/AST.h"

AST::AST(std::unique_ptr<ASNode> value) : Tree(std::move(value)) {}