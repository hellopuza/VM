#include "VM/ClassLinker.h"

static std::string getString(const std::string& klass, size_t* pos)
{
    size_t start = *pos;
    while ((*pos < klass.length()) && klass[*pos])
    {
        (*pos)++;
    }
    (*pos)++;
    return klass.substr(start, *pos - 1 - start);
}

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

void ClassLinker::getConstantPool(cp::ConstantPool* const_pool, const std::string& klass, size_t* pos)
{
    auto cp_size = *reinterpret_cast<const uint16_t*>(&klass[*pos]);
    (*pos) += sizeof(cp_size);
    const_pool_ = const_pool;

    for (uint16_t i = 0; i < cp_size; i++)
    {
        auto type = static_cast<uint8_t>(klass[*pos]);
        (*pos)++;
        switch (type)
        {
        case static_cast<uint8_t>(cp::AbstractType::Type::INTEGER):
        {
            auto value = *reinterpret_cast<const int32_t*>(&klass[*pos]);
            (*pos) += sizeof(value);
            const_pool->emplace_back(std::make_unique<cp::IntegerType>(cp::IntegerType(value)));
            break;
        }
        case static_cast<uint8_t>(cp::AbstractType::Type::FLOAT):
        {
            auto value = *reinterpret_cast<const float*>(&klass[*pos]);
            (*pos) += sizeof(value);
            const_pool->emplace_back(std::make_unique<cp::FloatType>(cp::FloatType(value)));
            break;
        }
        case static_cast<uint8_t>(cp::AbstractType::Type::STRING):
        {
            auto value = getString(klass, pos);
            const_pool->emplace_back(std::make_unique<cp::StringType>(cp::StringType(value)));
            break;
        }
        case static_cast<uint8_t>(cp::AbstractType::Type::SYMBOL):
        {
            auto value = *reinterpret_cast<const char*>(&klass[*pos]);
            (*pos)++;
            const_pool->emplace_back(std::make_unique<cp::SymbolType>(cp::SymbolType(value)));
            break;
        }
        case static_cast<uint8_t>(cp::AbstractType::Type::FIELD):
        {
            auto class_name = getString(klass, pos);
            auto field_name = getString(klass, pos);
            const_pool->emplace_back(std::make_unique<cp::FieldType>(cp::FieldType(class_name, field_name)));
            break;
        }
        case static_cast<uint8_t>(cp::AbstractType::Type::METHOD):
        {
            auto class_name = getString(klass, pos);
            auto method_name = getString(klass, pos);
            const_pool->emplace_back(std::make_unique<cp::MethodType>(cp::MethodType(class_name, method_name)));
            break;
        }
        case static_cast<uint8_t>(cp::AbstractType::Type::POINTER):
            break;
        case static_cast<uint8_t>(cp::AbstractType::Type::DATA_TYPE):
        {
            auto data_type = static_cast<pkm::VariableType>(*reinterpret_cast<const uint8_t*>(&klass[*pos]));
            (*pos)++;

            if (data_type == pkm::VariableType::REFERENCE)
            {
                auto type_name = getString(klass, pos);
                const_pool->emplace_back(std::make_unique<cp::DataType>(cp::DataType(pkm::DataType(data_type, type_name))));
                break;
            }
            const_pool->emplace_back(std::make_unique<cp::DataType>(cp::DataType(pkm::DataType(data_type))));
            break;
        }
        }
    }
}

void ClassLinker::getFields(PkmFields* fields, const std::string& klass, size_t* pos)
{
    auto fields_num = static_cast<uint8_t>(klass[*pos]);
    (*pos)++;

    for (uint8_t i = 0; i < fields_num; i++)
    {
        auto access_type = static_cast<pkm::AccessType>(static_cast<uint8_t>(klass[*pos]));
        (*pos)++;

        auto var_type_ind = *reinterpret_cast<const uint16_t*>(&klass[*pos]);
        (*pos) += sizeof(var_type_ind);
        auto var_type = static_cast<cp::DataType*>((*const_pool_)[var_type_ind].get())->data_type;

        auto name_ind = *reinterpret_cast<const uint16_t*>(&klass[*pos]);
        (*pos) += sizeof(name_ind);
        auto name = static_cast<cp::StringType*>((*const_pool_)[name_ind].get())->value;

        (*fields)[name].access_type = access_type;
        (*fields)[name].var_type = var_type;
        (*fields)[name].name = name;
    }
}

void ClassLinker::getMethods(PkmMethods* methods, pclass cls, const std::string& klass, size_t* pos)
{
    auto methods_num = static_cast<uint8_t>(klass[*pos]);
    (*pos)++;

    for (uint8_t i = 0; i < methods_num; i++)
    {
        auto access_type = static_cast<pkm::AccessType>(static_cast<uint8_t>(klass[*pos]));
        (*pos)++;

        auto modifier = static_cast<pkm::MethodType>(static_cast<uint8_t>(klass[*pos]));
        (*pos)++;

        auto ret_type_ind = *reinterpret_cast<const uint16_t*>(&klass[*pos]);
        (*pos) += sizeof(ret_type_ind);
        auto ret_type = static_cast<cp::DataType*>((*const_pool_)[ret_type_ind].get())->data_type;

        auto name_ind = *reinterpret_cast<const uint16_t*>(&klass[*pos]);
        (*pos) += sizeof(name_ind);
        auto name = static_cast<cp::StringType*>((*const_pool_)[name_ind].get())->value;

        auto mps_num = static_cast<uint8_t>(klass[*pos]);
        (*pos)++;

        for (uint8_t m = 0; m < mps_num; m++)
        {
            auto var_type_ind = *reinterpret_cast<const uint16_t*>(&klass[*pos]);
            (*pos) += sizeof(var_type_ind);
            auto var_type = static_cast<cp::DataType*>((*const_pool_)[var_type_ind].get())->data_type;
            (*methods)[name].met_params.push_back(var_type);
        }

        auto offset = *reinterpret_cast<const uint32_t*>(&klass[*pos]);
        (*pos) += sizeof(offset);

        auto locals_num = *reinterpret_cast<const uint16_t*>(&klass[*pos]);
        (*pos) += sizeof(locals_num);

        (*methods)[name].access_type = access_type;
        (*methods)[name].modifier = modifier;
        (*methods)[name].ret_type = ret_type;
        (*methods)[name].name = name;
        (*methods)[name].offset = offset;
        (*methods)[name].locals_num = locals_num;
        (*methods)[name].cls = cls;
    }
}

void ClassLinker::linkClasses()
{
    for (auto& [name, cls] : classes)
    {
        for (auto& elem : cls.const_pool)
        {
            switch (elem->type())
            {
            case cp::AbstractType::Type::FIELD:
            {
                std::string class_name = static_cast<cp::FieldType*>(elem.get())->class_name;
                std::string field_name = static_cast<cp::FieldType*>(elem.get())->field_name;
                elem = std::make_unique<cp::PointerType>(cp::PointerType(&classes[class_name].fields[field_name]));
                break;
            }
            case cp::AbstractType::Type::METHOD:
            {
                std::string class_name = static_cast<cp::MethodType*>(elem.get())->class_name;
                std::string method_name = static_cast<cp::MethodType*>(elem.get())->method_name;
                elem = std::make_unique<cp::PointerType>(cp::PointerType(&classes[class_name].methods[method_name]));
                break;
            }
            default:
                break;
            }
        }
    }
}