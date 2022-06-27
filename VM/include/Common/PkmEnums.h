#ifndef PKMENUMS_H
#define PKMENUMS_H

#include <string>

namespace pkm {

enum class ASTNodeType
{
    ROOT,
    CLASS,
    FIELD,
    METHOD,
    MET_PAR,
    SCOPE,
    CONTROL,
    VAR_DECL,
    OPERATION,
    FUNCTION,
    VARIABLE,
    CONVERSION,
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
    CHAR,
    INT,
    FLOAT,
    REFERENCE,
};

enum class OperationType
{
    RETURN,
    ASSIGN,
    OR,
    AND,
    EQ,
    NEQ,
    LEQ,
    GEQ,
    STL,
    STG,
    SHL,
    SHR,
    ADD,
    SUB,
    MUL,
    DIV,
    REM,
    NOT,
    SQR_BR,
    NEW,
};

enum class ControlType
{
    IF,
    ELSE,
    ELIF,
    WHILE,
};

namespace strings {

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
    "char",
    "int",
    "float",
    "$reference_type$",
};

static const std::string OPERATION[] = {
    "return",
    "=",
    "||",
    "&&",
    "==",
    "!=",
    "<=",
    ">=",
    "<",
    ">",
    "<<",
    ">>",
    "+",
    "-",
    "*",
    "/",
    "%",
    "!",
    "[]",
    "new",
};

static const std::string CONTROL[] = {
    "if",
    "else",
    "elif",
    "while",
};

static const std::string RESERVED_NAMES[] = {
    ACCESS[0],
    ACCESS[1],
    METHOD[0],
    METHOD[1],
    METHOD[2],
    TYPES[0],
    TYPES[1],
    TYPES[2],
    TYPES[3],
    OPERATION[0],
    OPERATION[19],
    CONTROL[0],
    CONTROL[1],
    CONTROL[2],
    CONTROL[3],
};

} // namespace strings

struct DataType
{
    DataType(VariableType var_type = VariableType::VOID);
    DataType(VariableType var_type, std::string name_);

    VariableType type;
    std::string name;
};

} // namespace pkm

#endif // PKMENUMS_H