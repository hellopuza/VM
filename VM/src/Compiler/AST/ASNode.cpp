#include "Compiler/AST/ASNode.h"

static const std::string ACCESS[] = {
    "public",
    "private",
};

static const std::string METHOD[] = {
    "",
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
    "for",
    "while",
};

ASNode::ASNode(NodeType type_) : type(type_) {}

std::string ASNode::print() const
{
    return "Root";
}

ClassNode::ClassNode(std::string name_) : ASNode(NodeType::CLASS), name(std::move(name_)) {}

std::string ClassNode::print() const
{
    return "class " + name;
}

FieldNode::FieldNode(std::string name_, AccessType access_type_, VariableType var_type_) :
    ASNode(NodeType::FIELD), name(std::move(name_)), access_type(access_type_), var_type(var_type_)
{}

std::string FieldNode::print() const
{
    return ACCESS[static_cast<int>(access_type)] + " " + VARIABLE[static_cast<int>(var_type)] + " " + name;
}

MethodNode::MethodNode(std::string name_, AccessType access_type_, MethodType modifier_, VariableType ret_type_) :
    ASNode(NodeType::METHOD), name(std::move(name_)), access_type(access_type_), modifier(modifier_), ret_type(ret_type_)
{}

std::string MethodNode::print() const
{
    return ACCESS[static_cast<int>(access_type)] + " " + METHOD[static_cast<int>(modifier)] + " " + \
           VARIABLE[static_cast<int>(ret_type)] + " " + name;
}

MethodParameterNode::MethodParameterNode(std::string name_, VariableType var_type_) :
    ASNode(NodeType::METHODPARAMETER), name(std::move(name_)), var_type(var_type_)
{}

std::string MethodParameterNode::print() const
{
    return VARIABLE[static_cast<int>(var_type)] + " " + name;
}

ScopeNode::ScopeNode() : ASNode(NodeType::SCOPE) {}

std::string ScopeNode::print() const
{
    return "{}";
}

OperationNode::OperationNode(OperationType op_type_) : ASNode(NodeType::OPERATION), op_type(op_type_) {}

std::string OperationNode::print() const
{
    return OPERATION[static_cast<int>(op_type)];
}

ControlNode::ControlNode(ControlType control_type_) : ASNode(NodeType::CONTROL), control_type(control_type_) {}

std::string ControlNode::print() const
{
    return CONTROL[static_cast<int>(control_type)];
}