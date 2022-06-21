#ifndef PKMENUMS_H
#define PKMENUMS_H

#include <string>

enum class ASTNodeType
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
    VARIABLE,
    NUMBER,
    STRING,
    SYMBOL,
    TYPE,
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
    NOT,
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
    ASSIGN,
    SQR_BR,
    NEW,
    RETURN,
};

enum class ControlType
{
    IF,
    ELSE,
    ELIF,
    WHILE,
};

namespace pkm_str {

static const std::string ACCESS[] = {
    "public",
    "private",
};

static const std::string METHOD[] = {
    "instance",
    "static",
    "native",
};

static const std::string TYPES[] = {
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
    "!",
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
    "=",
    "[]",
    "new",
    "return",
};

static const std::string CONTROL[] = {
    "if",
    "else",
    "elif",
    "while",
};

} // namespace pkm_str

#endif // PKMENUMS_H