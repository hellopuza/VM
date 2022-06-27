#include "Compiler/AST/ASNode.h"

using pkm::strings::ACCESS;
using pkm::strings::CONTROL;
using pkm::strings::METHOD;
using pkm::strings::OPERATION;

namespace ast {

pkm::ASTNodeType ASNode::type() const
{
    return pkm::ASTNodeType::ROOT;
}

std::string ASNode::print() const
{
    return "Root";
}

ClassNode::ClassNode(std::string name_) : name(std::move(name_)) {}

pkm::ASTNodeType ClassNode::type() const
{
    return pkm::ASTNodeType::CLASS;
}

std::string ClassNode::print() const
{
    return "class " + name;
}

FieldNode::FieldNode(std::string name_, pkm::AccessType access_type_, pkm::DataType var_type_) :
    name(std::move(name_)), access_type(access_type_), var_type(std::move(var_type_))
{}

pkm::ASTNodeType FieldNode::type() const
{
    return pkm::ASTNodeType::FIELD;
}

std::string FieldNode::print() const
{
    return ACCESS[static_cast<int>(access_type)] + " " + var_type.name + " " + name;
}

MethodNode::MethodNode(std::string name_, pkm::AccessType access_type_, pkm::MethodType modifier_, pkm::DataType ret_type_) :
    name(std::move(name_)), access_type(access_type_), modifier(modifier_), ret_type(std::move(ret_type_))
{}

pkm::ASTNodeType MethodNode::type() const
{
    return pkm::ASTNodeType::METHOD;
}

std::string MethodNode::print() const
{
    return ACCESS[static_cast<int>(access_type)] + " " + METHOD[static_cast<int>(modifier)] + " " + \
           ret_type.name + " " + name;
}

MethodParameterNode::MethodParameterNode(std::string name_, pkm::DataType var_type_) :
    name(std::move(name_)), var_type(std::move(var_type_))
{}

pkm::ASTNodeType MethodParameterNode::type() const
{
    return pkm::ASTNodeType::MET_PAR;
}

std::string MethodParameterNode::print() const
{
    return var_type.name + " " + name;
}

pkm::ASTNodeType ScopeNode::type() const
{
    return pkm::ASTNodeType::SCOPE;
}

std::string ScopeNode::print() const
{
    return "{}";
}

OperationNode::OperationNode(pkm::OperationType op_type_) : op_type(op_type_) {}

pkm::ASTNodeType OperationNode::type() const
{
    return pkm::ASTNodeType::OPERATION;
}

std::string OperationNode::print() const
{
    return OPERATION[static_cast<int>(op_type)];
}

ConversionNode::ConversionNode(pkm::DataType from_, pkm::DataType to_) : from(std::move(from_)), to(std::move(to_)) {}

pkm::ASTNodeType ConversionNode::type() const
{
    return pkm::ASTNodeType::CONVERSION;
}

std::string ConversionNode::print() const
{
    return from.name + " to " + to.name;
}

ControlNode::ControlNode(pkm::ControlType control_type_) : control_type(control_type_) {}

pkm::ASTNodeType ControlNode::type() const
{
    return pkm::ASTNodeType::CONTROL;
}

std::string ControlNode::print() const
{
    return CONTROL[static_cast<int>(control_type)];
}

FunctionNode::FunctionNode(std::string name_) : name(std::move(name_)) {}

pkm::ASTNodeType FunctionNode::type() const
{
    return pkm::ASTNodeType::FUNCTION;
}

std::string FunctionNode::print() const
{
    return name;
}

VariableDeclarationNode::VariableDeclarationNode(std::string name_, pkm::DataType var_type_) :
    name(std::move(name_)), var_type(std::move(var_type_))
{}

pkm::ASTNodeType VariableDeclarationNode::type() const
{
    return pkm::ASTNodeType::VAR_DECL;
}

std::string VariableDeclarationNode::print() const
{
    return var_type.name + " " + name;
}

VariableNode::VariableNode(std::string name_) : name(std::move(name_)) {}

pkm::ASTNodeType VariableNode::type() const
{
    return pkm::ASTNodeType::VARIABLE;
}

std::string VariableNode::print() const
{
    return name;
}

NumberNode::NumberNode(int32_t num) : num_type(pkm::VariableType::INT)
{
    number.i = num;
}

NumberNode::NumberNode(float num) : num_type(pkm::VariableType::FLOAT)
{
    number.f = num;
}

pkm::ASTNodeType NumberNode::type() const
{
    return pkm::ASTNodeType::NUMBER;
}

std::string NumberNode::print() const
{
    switch (num_type)
    {
    case pkm::VariableType::INT:
        return std::to_string(number.i);
    case pkm::VariableType::FLOAT:
        return std::to_string(number.f);
    default:
        break;
    }
    return "err";
}

StringNode::StringNode(std::string value_) : value(std::move(value_)) {}

pkm::ASTNodeType StringNode::type() const
{
    return pkm::ASTNodeType::STRING;
}

std::string StringNode::print() const
{
    return "\\\"" + value + "\\\"";
}

SymbolNode::SymbolNode(char value_) : value(value_) {}

pkm::ASTNodeType SymbolNode::type() const
{
    return pkm::ASTNodeType::SYMBOL;
}

std::string SymbolNode::print() const
{
    return "\\\'" + std::string(1, value) + "\\\'";
}

TypeNode::TypeNode(pkm::DataType data_type_) : data_type(std::move(data_type_)) {}

pkm::ASTNodeType TypeNode::type() const
{
    return pkm::ASTNodeType::TYPE;
}

std::string TypeNode::print() const
{
    return data_type.name;
}

} // namespace ast