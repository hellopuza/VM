#ifndef CONSTANTPOOL_H
#define CONSTANTPOOL_H

#include "PkmEnums.h"

#include <memory>
#include <vector>

namespace cp {

struct AbstractType
{
    enum class Type
    {
        INTEGER,
        FLOAT,
        STRING,
        SYMBOL,
        FIELD,
        METHOD,
        POINTER,
        DATA_TYPE,
    };

    virtual Type type() const = 0;
    virtual ~AbstractType() = default;
};

struct IntegerType : public AbstractType
{
    IntegerType(int32_t value_);
    Type type() const override;

    int32_t value;
};

struct FloatType : public AbstractType
{
    FloatType(float value_);
    Type type() const override;

    float value;
};

struct StringType : public AbstractType
{
    StringType(std::string value_);
    Type type() const override;

    std::string value;
};

struct SymbolType : public AbstractType
{
    SymbolType(char value_);
    Type type() const override;

    char value;
};

struct FieldType : public AbstractType
{
    FieldType(std::string class_name_, std::string field_name_);
    Type type() const override;

    std::string class_name;
    std::string field_name;
};

struct MethodType : public AbstractType
{
    MethodType(std::string class_name_, std::string method_name_);
    Type type() const override;

    std::string class_name;
    std::string method_name;
};

struct PointerType : public AbstractType
{
    PointerType(void* value_);
    Type type() const override;

    void* value;
};

struct DataType : public AbstractType
{
    DataType(pkm::DataType data_type_);
    Type type() const override;

    pkm::DataType data_type;
};

bool operator==(const std::unique_ptr<AbstractType>& lhs, const std::unique_ptr<AbstractType>& rhs);

using ConstantPool = std::vector<std::unique_ptr<AbstractType>>;

} // namespace cp

namespace std {

template <>
struct hash<std::unique_ptr<cp::AbstractType>>
{
    size_t operator()(const std::unique_ptr<cp::AbstractType>& key) const;
};

} // namespace std


#endif // CONSTANTPOOL_H