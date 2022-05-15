#include "VM/ClassLinker.h"

void ClassLinker::link(const Klasses& klasses)
{
    for (const auto& klass : klasses)
    {
        appendClass(klass);
    }
}

void ClassLinker::appendClass(const std::string& klass)
{
    size_t pos = 0;
    std::string class_name = getString(klass, &pos);

    getConstantPool(&classes[class_name].const_pool, klass, &pos);

    getFields(&classes[class_name].fields, klass, &pos);
    getMethods(&classes[class_name].methods, &classes[class_name], klass, &pos);

    classes[class_name].bytecode = klass.substr(pos);

    linkClasses();
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

void ClassLinker::getConstantPool(ConstPool* const_pool, const std::string& klass, size_t* pos)
{
    auto cp_size = *reinterpret_cast<const uint16_t*>(&klass[*pos]);
    (*pos) += sizeof(cp_size);
    const_pool_ptr_ = const_pool;

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
            const_pool->emplace_back(std::make_unique<IntegerType>(IntegerType(value)));
            break;
        }
        case static_cast<uint8_t>(AbstractType::Type::FLOAT):
        {
            auto value = *reinterpret_cast<const float*>(&klass[*pos]);
            (*pos) += sizeof(value);
            const_pool->emplace_back(std::make_unique<FloatType>(FloatType(value)));
            break;
        }
        case static_cast<uint8_t>(AbstractType::Type::STRING):
        {
            auto value = getString(klass, pos);
            const_pool->emplace_back(std::make_unique<StringType>(StringType(value)));
            break;
        }
        case static_cast<uint8_t>(AbstractType::Type::FUNCTION):
        {
            auto value = getString(klass, pos);
            const_pool->emplace_back(std::make_unique<FunctionType>(FunctionType(value)));
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

        auto field_name = static_cast<StringType*>((*const_pool_ptr_)[name].get())->value;
        (*fields)[field_name].access_type = static_cast<AccessType>(access_type);
        (*fields)[field_name].var_type = static_cast<VariableType>(var_type);
    }
}

void ClassLinker::getMethods(PkmMethods* methods, pclass cls, const std::string& klass, size_t* pos)
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

        auto method_name = static_cast<StringType*>((*const_pool_ptr_)[name].get())->value;
        (*methods)[method_name].name = name;
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

        auto offset = *reinterpret_cast<const uint32_t*>(&klass[*pos]);
        (*pos) += sizeof(offset);
        (*methods)[method_name].offset = offset;

        auto locals_num = *reinterpret_cast<const uint16_t*>(&klass[*pos]);
        (*pos) += sizeof(locals_num);
        (*methods)[method_name].locals_num = locals_num;

        (*methods)[method_name].cls = cls;
    }
}

void ClassLinker::linkClasses()
{
    for (auto& [name, cls] : classes)
    {
        for (auto& elem : cls.const_pool)
        {
            if (elem->type() == AbstractType::Type::FUNCTION)
            {
                std::string func_name = static_cast<FunctionType*>(elem.get())->value;

                size_t dot_pos = func_name.find('.');
                if (dot_pos == std::string::npos)
                {
                    elem = std::make_unique<PointerType>(PointerType(&(cls.methods[func_name])));
                }
                else
                {
                    std::string class_name = func_name.substr(0, dot_pos);
                    std::string method_name = func_name.substr(dot_pos + 1);
                    elem = std::make_unique<PointerType>(PointerType(&(classes[class_name].methods[method_name])));
                }
            }
        }
    }
}