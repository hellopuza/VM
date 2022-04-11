#ifndef COMPILER_AST_ASNODE_H
#define COMPILER_AST_ASNODE_H

#include <fstream>
#include <memory>
#include <string>

enum class NodeType
{
    ROOT,
    CLASS,
    FIELD,
    METHOD,
    METHODPARAMETER,
    SCOPE,
    OPERATION,
    CONTROL,
};

enum class AccessType
{
    PUBLIC,
    PRIVATE,
};

enum class MethodType
{
    NONSTATIC,
    STATIC,
    NATIVE,
};

enum class VariableType
{
    VOID,
    BOOLEAN,
    BYTE,
    CHAR,
    SHORT,
    INT,
    LONG,
    FLOAT,
    DOUBLE,
    REFERENCE,
};

enum class OperationType
{
    OR,
    AND,
    EQ,
    NEQ,
    LEQ,
    GEQ,
    STL,
    STG,
    ADD,
    SUB,
    MUL,
    DIV,
    DOT,
    COMMA,
    ASSOP,
    NEW,
    RETURN,
};

enum class ControlType
{
    IF,
    ELSE,
    FOR,
    WHILE,
};

struct ASNode
{
    NodeType type = NodeType::ROOT;

    ASNode(NodeType type_);
    virtual ~ASNode() = default;
};

std::ostream& operator<<(std::ostream& os, const std::unique_ptr<ASNode>&);

struct ClassNode : public ASNode
{
    std::string name;
    ClassNode(std::string name_);
};

std::ostream& operator<<(std::ostream& os, const std::unique_ptr<ClassNode>& node);

struct FieldNode : public ASNode
{
    std::string name;
    AccessType access_type;
    VariableType var_type;

    FieldNode(std::string name_, AccessType access_type_, VariableType var_type_);
};

std::ostream& operator<<(std::ostream& os, const std::unique_ptr<FieldNode>& node);

struct MethodNode : public ASNode
{
    std::string name;
    AccessType access_type;
    MethodType modifier;
    VariableType ret_type;

    MethodNode(std::string name_, AccessType access_type_, MethodType modifier_, VariableType ret_type_);
};

std::ostream& operator<<(std::ostream& os, const std::unique_ptr<MethodNode>& node);

struct MethodParameterNode : public ASNode
{
    std::string name;
    VariableType var_type;

    MethodParameterNode(std::string name_, VariableType var_type_);
};

std::ostream& operator<<(std::ostream& os, const std::unique_ptr<MethodParameterNode>& node);

struct ScopeNode : public ASNode
{
    ScopeNode();
};

std::ostream& operator<<(std::ostream& os, const std::unique_ptr<ScopeNode>&);

struct OperationNode : public ASNode
{
    OperationType op_type;

    OperationNode(OperationType op_type_);
};

std::ostream& operator<<(std::ostream& os, const std::unique_ptr<OperationNode>& node);

struct ControlNode : public ASNode
{
    ControlType control_type;

    ControlNode(ControlType control_type_);
};

std::ostream& operator<<(std::ostream& os, const std::unique_ptr<ControlNode>& node);

#endif // COMPILER_AST_ASNODE_H