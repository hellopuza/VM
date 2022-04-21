#ifndef CONSTANTPOOL_H
#define CONSTANTPOOL_H

#include <memory>
#include <unordered_map>

struct AbstractType
{
    enum class Type
    {
        INTEGER,
        FLOAT,
        STRING,
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

namespace std {

template <>
struct hash<std::unique_ptr<AbstractType>>
{
    size_t operator()(const std::unique_ptr<AbstractType>& key) const;
};

} // namespace std

bool operator==(const std::unique_ptr<AbstractType>& lhs, const std::unique_ptr<AbstractType>& rhs);

class ConstantPool : public std::unordered_map<std::unique_ptr<AbstractType>, uint16_t>
{
public:
    ConstantPool() = default;
};

#endif // CONSTANTPOOL_H