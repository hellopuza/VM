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

std::ostream& operator<<(std::ostream& os, const std::unique_ptr<ASNode>&)
{
    return os << "Root";
}

ClassNode::ClassNode(std::string name_) : ASNode(NodeType::CLASS), name(std::move(name_)) {}

std::ostream& operator<<(std::ostream& os, const std::unique_ptr<ClassNode>& node)
{
    return os << "Class " + node->name;
}

FieldNode::FieldNode(std::string name_, AccessType access_type_, VariableType var_type_) :
    ASNode(NodeType::FIELD), name(std::move(name_)), access_type(access_type_), var_type(var_type_)
{}

std::ostream& operator<<(std::ostream& os, const std::unique_ptr<FieldNode>& node)
{
    return os << ACCESS[static_cast<int>(node->access_type)] + " " + \
        VARIABLE[static_cast<int>(node->var_type)] + " " + node->name;
}

MethodNode::MethodNode(std::string name_, AccessType access_type_, MethodType modifier_, VariableType ret_type_) :
    ASNode(NodeType::METHOD), name(std::move(name_)), access_type(access_type_), modifier(modifier_), ret_type(ret_type_)
{}

std::ostream& operator<<(std::ostream& os, const std::unique_ptr<MethodNode>& node)
{
    return os << ACCESS[static_cast<int>(node->access_type)] + " " + \
        METHOD[static_cast<int>(node->modifier)] + " " + \
        VARIABLE[static_cast<int>(node->ret_type)] + " " + node->name;
}

MethodParameterNode::MethodParameterNode(std::string name_, VariableType var_type_) :
    ASNode(NodeType::METHODPARAMETER), name(std::move(name_)), var_type(var_type_)
{}

std::ostream& operator<<(std::ostream& os, const std::unique_ptr<MethodParameterNode>& node)
{
    return os << VARIABLE[static_cast<int>(node->var_type)] + " " + node->name;
}

ScopeNode::ScopeNode() : ASNode(NodeType::SCOPE) {}

std::ostream& operator<<(std::ostream& os, const std::unique_ptr<ScopeNode>&)
{
    return os << "Scope";
}

OperationNode::OperationNode(OperationType op_type_) : ASNode(NodeType::OPERATION), op_type(op_type_) {}

std::ostream& operator<<(std::ostream& os, const std::unique_ptr<OperationNode>& node)
{
    return os << OPERATION[static_cast<int>(node->op_type)];
}

ControlNode::ControlNode(ControlType control_type_) : ASNode(NodeType::CONTROL), control_type(control_type_) {}

std::ostream& operator<<(std::ostream& os, const std::unique_ptr<ControlNode>& node)
{
    return os << CONTROL[static_cast<int>(node->control_type)];
}