#include "Compiler/AST/AST.h"

AST::AST(std::unique_ptr<ASNode> value) : value_(std::move(value)) {}