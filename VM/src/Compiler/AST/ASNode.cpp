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
    ASNode(NodeType::MET_PAR), name(std::move(name_)), var_type(var_type_)
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

VariableDeclarationNode::VariableDeclarationNode(std::string name_, VariableType var_type_) :
    ASNode(NodeType::VAR_DECL), name(std::move(name_)), var_type(var_type_)
{}

std::string VariableDeclarationNode::print() const
{
    return VARIABLE[static_cast<int>(var_type)] + " " + name;
}

VariableNode::VariableNode(std::string name_) : ASNode(NodeType::VAR), name(std::move(name_)) {}

std::string VariableNode::print() const
{
    return name;
}

NumberNode::NumberNode(VariableType type_, bool num) :
    ASNode(NodeType::NUMBER), type(type_)
{
    number.bl = num;
}

NumberNode::NumberNode(VariableType type_, int8_t num) :
    ASNode(NodeType::NUMBER), type(type_)
{
    number.b = num;
}

NumberNode::NumberNode(VariableType type_, char num) :
    ASNode(NodeType::NUMBER), type(type_)
{
    number.c = num;
}

NumberNode::NumberNode(VariableType type_, int16_t num) :
    ASNode(NodeType::NUMBER), type(type_)
{
    number.s = num;
}

NumberNode::NumberNode(VariableType type_, int32_t num) :
    ASNode(NodeType::NUMBER), type(type_)
{
    number.i = num;
}

NumberNode::NumberNode(VariableType type_, int64_t num) :
    ASNode(NodeType::NUMBER), type(type_)
{
    number.l = num;
}

NumberNode::NumberNode(VariableType type_, float num) :
    ASNode(NodeType::NUMBER), type(type_)
{
    number.f = num;
}

NumberNode::NumberNode(VariableType type_, double num) :
    ASNode(NodeType::NUMBER), type(type_)
{
    number.d = num;
}

std::string NumberNode::print() const
{
    switch (type)
    {
    case VariableType::BOOLEAN: return std::to_string(static_cast<int>(number.bl));
    case VariableType::BYTE:    return std::to_string(number.b);
    case VariableType::CHAR:    return std::to_string(number.c);
    case VariableType::SHORT:   return std::to_string(number.s);
    case VariableType::INT:     return std::to_string(number.i);
    case VariableType::LONG:    return std::to_string(number.l);
    case VariableType::FLOAT:   return std::to_string(number.f);
    case VariableType::DOUBLE:  return std::to_string(number.d);
    }
    return "err";
}