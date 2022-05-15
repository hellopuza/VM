#include "Compiler/AST/ASNode.h"

using pkm_str::ACCESS;
using pkm_str::CONTROL;
using pkm_str::METHOD;
using pkm_str::OPERATION;
using pkm_str::TYPES;

ASTNodeType ASNode::type() const
{
    return ASTNodeType::ROOT;
}

std::string ASNode::print() const
{
    return "Root";
}

ClassNode::ClassNode(std::string name_) : name(std::move(name_)) {}

ASTNodeType ClassNode::type() const
{
    return ASTNodeType::CLASS;
}

std::string ClassNode::print() const
{
    return "class " + name;
}

FieldNode::FieldNode(std::string name_, AccessType access_type_, VariableType var_type_) :
    name(std::move(name_)), access_type(access_type_), var_type(var_type_)
{}

ASTNodeType FieldNode::type() const
{
    return ASTNodeType::FIELD;
}

std::string FieldNode::print() const
{
    return ACCESS[static_cast<int>(access_type)] + " " + TYPES[static_cast<int>(var_type)] + " " + name;
}

MethodNode::MethodNode(std::string name_, AccessType access_type_, MethodType modifier_, VariableType ret_type_) :
    name(std::move(name_)), access_type(access_type_), modifier(modifier_), ret_type(ret_type_)
{}

ASTNodeType MethodNode::type() const
{
    return ASTNodeType::METHOD;
}

std::string MethodNode::print() const
{
    return ACCESS[static_cast<int>(access_type)] + " " + METHOD[static_cast<int>(modifier)] + " " + \
           TYPES[static_cast<int>(ret_type)] + " " + name;
}

MethodParameterNode::MethodParameterNode(std::string name_, VariableType var_type_) :
    name(std::move(name_)), var_type(var_type_)
{}

ASTNodeType MethodParameterNode::type() const
{
    return ASTNodeType::MET_PAR;
}

std::string MethodParameterNode::print() const
{
    return TYPES[static_cast<int>(var_type)] + " " + name;
}

ASTNodeType ScopeNode::type() const
{
    return ASTNodeType::SCOPE;
}

std::string ScopeNode::print() const
{
    return "{}";
}

OperationNode::OperationNode(OperationType op_type_) : op_type(op_type_) {}

ASTNodeType OperationNode::type() const
{
    return ASTNodeType::OPERATION;
}

std::string OperationNode::print() const
{
    return OPERATION[static_cast<int>(op_type)];
}

ControlNode::ControlNode(ControlType control_type_) : control_type(control_type_) {}

ASTNodeType ControlNode::type() const
{
    return ASTNodeType::CONTROL;
}

std::string ControlNode::print() const
{
    return CONTROL[static_cast<int>(control_type)];
}

FunctionNode::FunctionNode(std::string name_) : name(std::move(name_)) {}

ASTNodeType FunctionNode::type() const
{
    return ASTNodeType::FUNCTION;
}

std::string FunctionNode::print() const
{
    return name;
}

VariableDeclarationNode::VariableDeclarationNode(std::string name_, VariableType var_type_) :
    name(std::move(name_)), var_type(var_type_)
{}

ASTNodeType VariableDeclarationNode::type() const
{
    return ASTNodeType::VAR_DECL;
}

std::string VariableDeclarationNode::print() const
{
    return TYPES[static_cast<int>(var_type)] + " " + name;
}

VariableNode::VariableNode(std::string name_) : name(std::move(name_)) {}

ASTNodeType VariableNode::type() const
{
    return ASTNodeType::VARIABLE;
}

std::string VariableNode::print() const
{
    return name;
}

NumberNode::NumberNode(bool num) : num_type(VariableType::BOOLEAN)
{
    number.i = static_cast<int32_t>(num);
}

NumberNode::NumberNode(int32_t num) : num_type(VariableType::INT)
{
    number.i = num;
}

NumberNode::NumberNode(float num) : num_type(VariableType::FLOAT)
{
    number.f = num;
}

ASTNodeType NumberNode::type() const
{
    return ASTNodeType::NUMBER;
}

std::string NumberNode::print() const
{
    switch (num_type)
    {
    case VariableType::BOOLEAN:
        return number.i ? "true" : "false";
    case VariableType::INT:
        return std::to_string(number.i);
    case VariableType::FLOAT:
        return std::to_string(number.f);
    default:
        break;
    }
    return "err";
}

StringNode::StringNode(std::string value_) : value(std::move(value_)) {}

ASTNodeType StringNode::type() const
{
    return ASTNodeType::STRING;
}

std::string StringNode::print() const
{
    return "\\\"" + value + "\\\"";
}

SymbolNode::SymbolNode(char value_) : value(value_) {}

ASTNodeType SymbolNode::type() const
{
    return ASTNodeType::SYMBOL;
}

std::string SymbolNode::print() const
{
    return "\\\'" + std::string(1, value) + "\\\'";
}

TypeNode::TypeNode(std::string str_) : str(std::move(str_)) {}

ASTNodeType TypeNode::type() const
{
    return ASTNodeType::TYPE;
}

std::string TypeNode::print() const
{
    return str;
}