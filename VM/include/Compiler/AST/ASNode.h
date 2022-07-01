#ifndef COMPILER_AST_ASNODE_H
#define COMPILER_AST_ASNODE_H

#include "Compiler/CompilationError.h"
#include "PkmEnums.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace ast {

struct ASNode
{
    virtual ~ASNode() = default;
    virtual pkm::ASTNodeType type() const;
    virtual std::string print() const;

    CodeLocation location;
};

struct ClassNode : public ASNode
{
    ClassNode(std::string name_);
    pkm::ASTNodeType type() const override;
    std::string print() const override;

    std::string name;
};

struct FieldNode : public ASNode
{
    FieldNode(std::string name_, pkm::AccessType access_type_, pkm::DataType var_type_);
    pkm::ASTNodeType type() const override;
    std::string print() const override;

    std::string name;
    pkm::AccessType access_type;
    pkm::DataType var_type;
};

struct MethodNode : public ASNode
{
    MethodNode(std::string name_, pkm::AccessType access_type_, pkm::MethodType modifier_, pkm::DataType ret_type_);
    pkm::ASTNodeType type() const override;
    std::string print() const override;

    std::string name;
    pkm::AccessType access_type;
    pkm::MethodType modifier;
    pkm::DataType ret_type;
};

struct MethodParameterNode : public ASNode
{
    MethodParameterNode(std::string name_, pkm::DataType var_type_);
    pkm::ASTNodeType type() const override;
    std::string print() const override;

    std::string name;
    pkm::DataType var_type;
};

struct ScopeNode : public ASNode
{
    ScopeNode() = default;
    pkm::ASTNodeType type() const override;
    std::string print() const override;
};

struct OperationNode : public ASNode
{
    OperationNode(pkm::OperationType op_type_);
    pkm::ASTNodeType type() const override;
    std::string print() const override;

    pkm::OperationType op_type;
    pkm::DataType var_type;
};

struct ConversionNode : public ASNode
{
    ConversionNode(pkm::DataType from_, pkm::DataType to_);
    pkm::ASTNodeType type() const override;
    std::string print() const override;

    pkm::DataType from;
    pkm::DataType to;
};

struct ControlNode : public ASNode
{
    ControlNode(pkm::ControlType control_type_);
    pkm::ASTNodeType type() const override;
    std::string print() const override;

    pkm::ControlType control_type;
};

struct FunctionNode : public ASNode
{
    FunctionNode(std::string name_);
    pkm::ASTNodeType type() const override;
    std::string print() const override;

    std::string name;
    pkm::DataType ret_type;

    pkm::MethodType modifier;
    std::vector<std::pair<std::string, pkm::DataType>> name_parts;
};

struct VariableDeclarationNode : public ASNode
{
    VariableDeclarationNode(std::string name_, pkm::DataType var_type_);
    pkm::ASTNodeType type() const override;
    std::string print() const override;

    std::string name;
    pkm::DataType var_type;
};

struct VariableNode : public ASNode
{
    VariableNode(std::string name_);
    pkm::ASTNodeType type() const override;
    std::string print() const override;

    std::string name;
    pkm::DataType var_type;

    std::vector<std::pair<std::string, pkm::DataType>> name_parts;
};

struct NumberNode : public ASNode
{
    NumberNode(bool num);
    NumberNode(int32_t num);
    NumberNode(float num);
    pkm::ASTNodeType type() const override;
    std::string print() const override;

    pkm::VariableType num_type;
    union Num {
        int32_t i;
        float f;
    } number;
};

struct StringNode : public ASNode
{
    StringNode(std::string value_);
    pkm::ASTNodeType type() const override;
    std::string print() const override;

    std::string value;
};

struct SymbolNode : public ASNode
{
    SymbolNode(char value_);
    pkm::ASTNodeType type() const override;
    std::string print() const override;

    char value;
};

struct TypeNode : public ASNode
{
    TypeNode(pkm::DataType data_type_);
    pkm::ASTNodeType type() const override;
    std::string print() const override;

    pkm::DataType data_type;
};

} // namespace ast

#endif // COMPILER_AST_ASNODE_H