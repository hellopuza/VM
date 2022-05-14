#include "ConstantPool.h"

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

FunctionType::FunctionType(std::string value_) : value(std::move(value_)) {}

AbstractType::Type FunctionType::type() const
{
    return AbstractType::Type::FUNCTION;
}

PointerType::PointerType(void* value_) : value(value_) {}

AbstractType::Type PointerType::type() const
{
    return AbstractType::Type::POINTER;
}

namespace std {

size_t hash<std::unique_ptr<AbstractType>>::operator()(const std::unique_ptr<AbstractType>& key) const
{
    switch (key->type())
    {
    case AbstractType::Type::INTEGER:
        return hash<int32_t>()(static_cast<IntegerType*>(key.get())->value);
    case AbstractType::Type::FLOAT:
        return hash<float>()(static_cast<FloatType*>(key.get())->value);
    case AbstractType::Type::STRING:
        return hash<string>()(static_cast<StringType*>(key.get())->value);
    case AbstractType::Type::FUNCTION:
        return hash<string>()(static_cast<FunctionType*>(key.get())->value);
    case AbstractType::Type::POINTER:
        return hash<void*>()(static_cast<PointerType*>(key.get())->value);
    default:
        break;
    }
    return 0;
}

} // namespace std

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
    case AbstractType::Type::FUNCTION:
        return static_cast<FunctionType*>(lhs.get())->value == static_cast<FunctionType*>(rhs.get())->value;
    case AbstractType::Type::POINTER:
        return static_cast<PointerType*>(lhs.get())->value == static_cast<PointerType*>(rhs.get())->value;
    default:
        break;
    }
    return false;
}