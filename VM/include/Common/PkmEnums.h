#ifndef PKMENUMS_H
#define PKMENUMS_H

enum class NodeType
{
    ROOT,
    CLASS,
    FIELD,
    METHOD,
    MET_PAR,
    SCOPE,
    OPERATION,
    CONTROL,
    FUNCTION,
    VAR_DECL,
    VAR,
    NUMBER,
};

enum class AccessType
{
    PUBLIC,
    PRIVATE,
};

enum class MethodType
{
    INSTANCE,
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
    ASSIGN,
    NEW,
    RETURN,
};

enum class ControlType
{
    IF,
    ELSE,
    ELIF,
    FOR,
    WHILE,
};

#endif // PKMENUMS_H