#include "VM/ClassLinker.h"

void ClassLinker::link(Klasses* klasses)
{
    for (auto& klass : *klasses)
    {
        appendClass(&klass);
    }
}

void ClassLinker::appendClass(std::string* klass)
{
    size_t pos = 0;
    std::string class_name = getString(*klass, &pos);

    getConstantPool(&classes[class_name].const_pool, *klass, &pos);

    getFields(&classes[class_name].fields, *klass, &pos);
    getMethods(&classes[class_name].methods, *klass, &pos);

    classes[class_name].bytecode = std::move(*klass);
}

std::string ClassLinker::getString(const std::string& klass, size_t* pos)
{
    size_t start = *pos;
    while ((*pos < klass.length()) && klass[*pos])
    {
        (*pos)++;
    }
    (*pos)++;
    return klass.substr(start, *pos - 1 - start);
}

void ClassLinker::getConstantPool(ConstantPool* const_pool, const std::string& klass, size_t* pos)
{
    auto cp_size = *reinterpret_cast<const uint16_t*>(&klass[*pos]);
    (*pos) += sizeof(cp_size);
    const_pool_.clear();

    for (uint16_t i = 0; i < cp_size; i++)
    {
        auto type = static_cast<uint8_t>(klass[*pos]);
        (*pos)++;
        switch (type)
        {
        case static_cast<uint8_t>(AbstractType::Type::INTEGER):
        {
            auto value = *reinterpret_cast<const int32_t*>(&klass[*pos]);
            (*pos) += sizeof(value);
            (*const_pool)[std::make_unique<IntegerType>(IntegerType(value))] = i;
            const_pool_.emplace_back(std::make_unique<IntegerType>(IntegerType(value)));
            break;
        }
        case static_cast<uint8_t>(AbstractType::Type::FLOAT):
        {
            auto value = *reinterpret_cast<const float*>(&klass[*pos]);
            (*pos) += sizeof(value);
            (*const_pool)[std::make_unique<FloatType>(FloatType(value))] = i;
            const_pool_.emplace_back(std::make_unique<FloatType>(FloatType(value)));
            break;
        }
        case static_cast<uint8_t>(AbstractType::Type::STRING):
        {
            auto value = getString(klass, pos);
            (*const_pool)[std::make_unique<StringType>(StringType(value))] = i;
            const_pool_.emplace_back(std::make_unique<StringType>(StringType(value)));
            break;
        }
        default:
            break;
        }
    }
}

void ClassLinker::getFields(PkmFields* fields, const std::string& klass, size_t* pos)
{
    auto fields_num = static_cast<uint8_t>(klass[*pos]);
    (*pos)++;

    for (uint8_t i = 0; i < fields_num; i++)
    {
        auto access_type = static_cast<uint8_t>(klass[*pos]);
        (*pos)++;
        auto var_type = static_cast<uint8_t>(klass[*pos]);
        (*pos)++;
        auto name = *reinterpret_cast<const uint16_t*>(&klass[*pos]);
        (*pos) += sizeof(name);

        auto field_name = static_cast<StringType*>(const_pool_[name].get())->value;
        (*fields)[field_name].access_type = static_cast<AccessType>(access_type);
        (*fields)[field_name].var_type = static_cast<VariableType>(var_type);
    }
}

void ClassLinker::getMethods(PkmMethods* methods, const std::string& klass, size_t* pos)
{
    auto methods_num = static_cast<uint8_t>(klass[*pos]);
    (*pos)++;

    for (uint8_t i = 0; i < methods_num; i++)
    {
        auto access_type = static_cast<uint8_t>(klass[*pos]);
        (*pos)++;
        auto modifier = static_cast<uint8_t>(klass[*pos]);
        (*pos)++;
        auto ret_type = static_cast<uint8_t>(klass[*pos]);
        (*pos)++;
        auto name = *reinterpret_cast<const uint16_t*>(&klass[*pos]);
        (*pos) += sizeof(name);

        auto method_name = static_cast<StringType*>(const_pool_[name].get())->value;
        (*methods)[method_name].access_type = static_cast<AccessType>(access_type);
        (*methods)[method_name].modifier = static_cast<MethodType>(modifier);
        (*methods)[method_name].ret_type = static_cast<VariableType>(ret_type);

        auto mps_num = static_cast<uint8_t>(klass[*pos]);
        (*pos)++;

        for (uint8_t m = 0; m < mps_num; m++)
        {
            auto var_type = static_cast<uint8_t>(klass[*pos]);
            (*pos)++;
            (*methods)[method_name].met_params.push_back(static_cast<VariableType>(var_type));
        }
    }
}