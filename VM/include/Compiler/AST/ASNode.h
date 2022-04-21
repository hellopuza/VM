#ifndef COMPILER_AST_ASNODE_H
#define COMPILER_AST_ASNODE_H

#include "PkmEnums.h"

#include <fstream>
#include <memory>
#include <string>

struct ASNode
{
    virtual ~ASNode() = default;
    virtual NodeType type() const;
    virtual std::string print() const;
};

struct ClassNode : public ASNode
{
    std::string name;

    ClassNode(std::string name_);
    NodeType type() const override;
    std::string print() const override;
};

struct FieldNode : public ASNode
{
    std::string name;
    AccessType access_type;
    VariableType var_type;

    FieldNode(std::string name_, AccessType access_type_, VariableType var_type_);
    NodeType type() const override;
    std::string print() const override;
};

struct MethodNode : public ASNode
{
    std::string name;
    AccessType access_type;
    MethodType modifier;
    VariableType ret_type;

    MethodNode(std::string name_, AccessType access_type_, MethodType modifier_, VariableType ret_type_);
    NodeType type() const override;
    std::string print() const override;
};

struct MethodParameterNode : public ASNode
{
    std::string name;
    VariableType var_type;

    MethodParameterNode(std::string name_, VariableType var_type_);
    NodeType type() const override;
    std::string print() const override;
};

struct ScopeNode : public ASNode
{
    ScopeNode() = default;
    NodeType type() const override;
    std::string print() const override;
};

struct OperationNode : public ASNode
{
    OperationType op_type;

    OperationNode(OperationType op_type_);
    NodeType type() const override;
    std::string print() const override;
};

struct ControlNode : public ASNode
{
    ControlType control_type;

    ControlNode(ControlType control_type_);
    NodeType type() const override;
    std::string print() const override;
};

struct VariableDeclarationNode : public ASNode
{
    std::string name;
    VariableType var_type;

    VariableDeclarationNode(std::string name_, VariableType var_type_);
    NodeType type() const override;
    std::string print() const override;
};

struct VariableNode : public ASNode
{
    std::string name;

    VariableNode(std::string name_);
    NodeType type() const override;
    std::string print() const override;
};

struct NumberNode : public ASNode
{
    VariableType num_type;
    union Num {
        bool bl;
        int8_t b;
        char c;
        int16_t s;
        int32_t i;
        int64_t l;
        float f;
        double d;
    } number;

    NumberNode(VariableType type_, bool num);
    NumberNode(VariableType type_, int8_t num);
    NumberNode(VariableType type_, char num);
    NumberNode(VariableType type_, int16_t num);
    NumberNode(VariableType type_, int32_t num);
    NumberNode(VariableType type_, int64_t num);
    NumberNode(VariableType type_, float num);
    NumberNode(VariableType type_, double num);
    NodeType type() const override;
    std::string print() const override;
};

#endif // COMPILER_AST_ASNODE_H