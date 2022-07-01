#include "ConstantPool.h"

namespace cp {

IntegerType::IntegerType(int32_t value_) : value(value_) {}

AbstractType::Type IntegerType::type() const
{
    return AbstractType::Type::INTEGER;
}

FloatType::FloatType(float value_) : value(value_) {}

AbstractType::Type FloatType::type() const
{
    return AbstractType::Type::FLOAT;
}

StringType::StringType(std::string value_) : value(std::move(value_)) {}

AbstractType::Type StringType::type() const
{
    return AbstractType::Type::STRING;
}

SymbolType::SymbolType(char value_) : value(value_) {}

AbstractType::Type SymbolType::type() const
{
    return AbstractType::Type::SYMBOL;
}

FieldType::FieldType(std::string class_name_, std::string field_name_) :
    class_name(std::move(class_name_)), field_name(std::move(field_name_))
{}

AbstractType::Type FieldType::type() const
{
    return AbstractType::Type::FIELD;
}

MethodType::MethodType(std::string class_name_, std::string method_name_) :
    class_name(std::move(class_name_)), method_name(std::move(method_name_))
{}

AbstractType::Type MethodType::type() const
{
    return AbstractType::Type::METHOD;
}

PointerType::PointerType(void* value_) : value(value_) {}

AbstractType::Type PointerType::type() const
{
    return AbstractType::Type::POINTER;
}

DataType::DataType(pkm::DataType data_type_) : data_type(std::move(data_type_)) {}

AbstractType::Type DataType::type() const
{
    return AbstractType::Type::DATA_TYPE;
}

bool operator==(const std::unique_ptr<AbstractType>& lhs, const std::unique_ptr<AbstractType>& rhs)
{
    if (lhs->type() != rhs->type())
    {
        return false;
    }

    switch (lhs->type())
    {
    case AbstractType::Type::INTEGER:
        return static_cast<IntegerType*>(lhs.get())->value == static_cast<IntegerType*>(rhs.get())->value;
    case AbstractType::Type::FLOAT:
        return static_cast<FloatType*>(lhs.get())->value == static_cast<FloatType*>(rhs.get())->value;
    case AbstractType::Type::STRING:
        return static_cast<StringType*>(lhs.get())->value == static_cast<StringType*>(rhs.get())->value;
    case AbstractType::Type::SYMBOL:
        return static_cast<SymbolType*>(lhs.get())->value == static_cast<SymbolType*>(rhs.get())->value;
    case AbstractType::Type::FIELD:
        return (static_cast<FieldType*>(lhs.get())->class_name == static_cast<FieldType*>(rhs.get())->class_name) &&
            (static_cast<FieldType*>(lhs.get())->field_name == static_cast<FieldType*>(rhs.get())->field_name);
    case AbstractType::Type::METHOD:
        return (static_cast<MethodType*>(lhs.get())->class_name == static_cast<MethodType*>(rhs.get())->class_name) &&
            (static_cast<MethodType*>(lhs.get())->method_name == static_cast<MethodType*>(rhs.get())->method_name);
    case AbstractType::Type::POINTER:
        return static_cast<PointerType*>(lhs.get())->value == static_cast<PointerType*>(rhs.get())->value;
    case AbstractType::Type::DATA_TYPE:
        return static_cast<DataType*>(lhs.get())->data_type.name == static_cast<DataType*>(rhs.get())->data_type.name;
    }
    return false;
}

} // namespace cp

namespace std {

size_t hash<std::unique_ptr<cp::AbstractType>>::operator()(const std::unique_ptr<cp::AbstractType>& key) const
{
    switch (key->type())
    {
    case cp::AbstractType::Type::INTEGER:
        return hash<int32_t>()(static_cast<cp::IntegerType*>(key.get())->value);
    case cp::AbstractType::Type::FLOAT:
        return hash<float>()(static_cast<cp::FloatType*>(key.get())->value);
    case cp::AbstractType::Type::STRING:
        return hash<string>()(static_cast<cp::StringType*>(key.get())->value);
    case cp::AbstractType::Type::SYMBOL:
        return hash<char>()(static_cast<cp::SymbolType*>(key.get())->value);
    case cp::AbstractType::Type::FIELD:
        return hash<string>()(static_cast<cp::FieldType*>(key.get())->class_name + static_cast<cp::FieldType*>(key.get())->field_name);
    case cp::AbstractType::Type::METHOD:
        return hash<string>()(static_cast<cp::MethodType*>(key.get())->class_name + static_cast<cp::MethodType*>(key.get())->method_name);
    case cp::AbstractType::Type::POINTER:
        return hash<void*>()(static_cast<cp::PointerType*>(key.get())->value);
    case cp::AbstractType::Type::DATA_TYPE:
        return hash<string>()(static_cast<cp::DataType*>(key.get())->data_type.name);
    }
    return 0;
}

} // namespace std