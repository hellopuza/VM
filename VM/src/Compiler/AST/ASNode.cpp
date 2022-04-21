#include "Compiler/AST/ASNode.h"

static const std::string ACCESS[] = {
    "public",
    "private",
};

static const std::string METHOD[] = {
    "instance",
    "static",
    "native",
};

static const std::string VARIABLE[] = {
    "void",
    "boolean",
    "byte",
    "char",
    "short",
    "int",
    "long",
    "float",
    "double",
    "ref",
};

static const std::string OPERATION[] = {
    "||",
    "&&",
    "==",
    "!=",
    "<=",
    ">=",
    "<",
    ">",
    "+",
    "-",
    "*",
    "/",
    ".",
    ",",
    "=",
    "new",
    "return",
};

static const std::string CONTROL[] = {
    "if",
    "else",
    "elif",
    "for",
    "while",
};

NodeType ASNode::type() const
{
    return NodeType::ROOT;
}

std::string ASNode::print() const
{
    return "Root";
}

ClassNode::ClassNode(std::string name_) : name(std::move(name_)) {}

NodeType ClassNode::type() const
{
    return NodeType::CLASS;
}

std::string ClassNode::print() const
{
    return "class " + name;
}

FieldNode::FieldNode(std::string name_, AccessType access_type_, VariableType var_type_) :
    name(std::move(name_)), access_type(access_type_), var_type(var_type_)
{}

NodeType FieldNode::type() const
{
    return NodeType::FIELD;
}

std::string FieldNode::print() const
{
    return ACCESS[static_cast<int>(access_type)] + " " + VARIABLE[static_cast<int>(var_type)] + " " + name;
}

MethodNode::MethodNode(std::string name_, AccessType access_type_, MethodType modifier_, VariableType ret_type_) :
    name(std::move(name_)), access_type(access_type_), modifier(modifier_), ret_type(ret_type_)
{}

NodeType MethodNode::type() const
{
    return NodeType::METHOD;
}

std::string MethodNode::print() const
{
    return ACCESS[static_cast<int>(access_type)] + " " + METHOD[static_cast<int>(modifier)] + " " + \
           VARIABLE[static_cast<int>(ret_type)] + " " + name;
}

MethodParameterNode::MethodParameterNode(std::string name_, VariableType var_type_) :
    name(std::move(name_)), var_type(var_type_)
{}

NodeType MethodParameterNode::type() const
{
    return NodeType::MET_PAR;
}

std::string MethodParameterNode::print() const
{
    return VARIABLE[static_cast<int>(var_type)] + " " + name;
}

NodeType ScopeNode::type() const
{
    return NodeType::SCOPE;
}

std::string ScopeNode::print() const
{
    return "{}";
}

OperationNode::OperationNode(OperationType op_type_) : op_type(op_type_) {}

NodeType OperationNode::type() const
{
    return NodeType::OPERATION;
}

std::string OperationNode::print() const
{
    return OPERATION[static_cast<int>(op_type)];
}

ControlNode::ControlNode(ControlType control_type_) : control_type(control_type_) {}

NodeType ControlNode::type() const
{
    return NodeType::CONTROL;
}

std::string ControlNode::print() const
{
    return CONTROL[static_cast<int>(control_type)];
}

FunctionNode::FunctionNode(std::string name_) : name(std::move(name_)) {}

NodeType FunctionNode::type() const
{
    return NodeType::FUNCTION;
}

std::string FunctionNode::print() const
{
    return name;
}

VariableDeclarationNode::VariableDeclarationNode(std::string name_, VariableType var_type_) :
    name(std::move(name_)), var_type(var_type_)
{}

NodeType VariableDeclarationNode::type() const
{
    return NodeType::VAR_DECL;
}

std::string VariableDeclarationNode::print() const
{
    return VARIABLE[static_cast<int>(var_type)] + " " + name;
}

VariableNode::VariableNode(std::string name_) : name(std::move(name_)) {}

NodeType VariableNode::type() const
{
    return NodeType::VAR;
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

NodeType NumberNode::type() const
{
    return NodeType::NUMBER;
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