#include "Compiler/Translator/Translator.h"
#include "Opcodes.h"

Translator::Translator(AST* ast) : ast_(ast) {}

void Translator::translate(std::ofstream* file)
{
    auto* class_node = static_cast<AST*>(&((*ast_)[0]));
    std::string class_name = static_cast<ClassNode*>(class_node->value().get())->name;
    file->write(class_name.c_str(), class_name.length() + 1);

    std::stringstream ss;
    writeFields(class_node, &ss);
    writeMethods(class_node, &ss);
    //writeInstructions

    writeConstantPool(file);
    *file << ss.str();
}

void Translator::writeConstantPool(std::ofstream* file)
{
    auto cp_size = static_cast<uint16_t>(const_pool_.size());
    file->write(reinterpret_cast<char*>(&cp_size), sizeof(cp_size));

    std::vector<std::pair<AbstractType*, uint16_t>> elems;
    for (const auto& [key, value] : const_pool_)
    {
        elems.emplace_back(std::make_pair(key.get(), value));
    }
    std::sort(elems.begin(), elems.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.second < rhs.second;
    });

    for (const auto& elem : elems)
    {
        auto type = static_cast<uint8_t>(elem.first->type());
        file->write(reinterpret_cast<char*>(&type), sizeof(type));
        switch (type)
        {
        case static_cast<uint8_t>(AbstractType::Type::INTEGER):
        {
            auto value = static_cast<IntegerType*>(elem.first)->value;
            file->write(reinterpret_cast<char*>(&value), sizeof(value));
            break;
        }
        case static_cast<uint8_t>(AbstractType::Type::FLOAT):
        {
            auto value = static_cast<FloatType*>(elem.first)->value;
            file->write(reinterpret_cast<char*>(&value), sizeof(value));
            break;
        }
        case static_cast<uint8_t>(AbstractType::Type::STRING):
        {
            auto value = static_cast<StringType*>(elem.first)->value;
            file->write(reinterpret_cast<const char*>(value.c_str()), value.length() + 1);
            break;
        }
        default:
            break;
        }
    }
}

void Translator::writeFields(AST* class_node, std::stringstream* ss)
{
    uint8_t fields_num = 0;
    for (size_t i = 0; i < class_node->branches_num(); i++)
    {
        if ((*class_node)[i].value()->type() == NodeType::FIELD)
        {
            fields_num++;
        }
    }
    ss->write(reinterpret_cast<char*>(&fields_num), sizeof(fields_num));

    for (size_t i = 0; i < class_node->branches_num(); i++)
    {
        if ((*class_node)[i].value()->type() == NodeType::FIELD)
        {
            auto* field_node = static_cast<FieldNode*>((*class_node)[i].value().get());
            ss->write(reinterpret_cast<char*>(&field_node->access_type), 1);
            ss->write(reinterpret_cast<char*>(&field_node->var_type), 1);

            auto cp_size = static_cast<uint16_t>(const_pool_.size());
            ss->write(reinterpret_cast<char*>(&cp_size), sizeof(cp_size));
            const_pool_[std::make_unique<StringType>(StringType(field_node->name))] = cp_size;
        }
    }
}

void Translator::writeMethods(AST* class_node, std::stringstream* ss)
{
    uint8_t methods_num = 0;
    for (size_t i = 0; i < class_node->branches_num(); i++)
    {
        if ((*class_node)[i].value()->type() == NodeType::METHOD)
        {
            methods_num++;
        }
    }
    ss->write(reinterpret_cast<char*>(&methods_num), sizeof(methods_num));

    for (size_t i = 0; i < class_node->branches_num(); i++)
    {
        if ((*class_node)[i].value()->type() == NodeType::METHOD)
        {
            auto* method_node = static_cast<MethodNode*>((*class_node)[i].value().get());
            ss->write(reinterpret_cast<char*>(&method_node->access_type), 1);
            ss->write(reinterpret_cast<char*>(&method_node->modifier), 1);
            ss->write(reinterpret_cast<char*>(&method_node->ret_type), 1);

            auto cp_size = static_cast<uint16_t>(const_pool_.size());
            ss->write(reinterpret_cast<char*>(&cp_size), sizeof(cp_size));
            const_pool_[std::make_unique<StringType>(StringType(method_node->name))] = cp_size;

            writeMethodParams(static_cast<AST*>(&((*class_node)[i])), ss);
        }
    }
}

void Translator::writeMethodParams(AST* method_node, std::stringstream* ss)
{
    uint8_t mps_num = 0;
    for (size_t i = 0; i < method_node->branches_num(); i++)
    {
        if ((*method_node)[i].value()->type() == NodeType::MET_PAR)
        {
            mps_num++;
        }
    }
    ss->write(reinterpret_cast<char*>(&mps_num), sizeof(mps_num));

    for (size_t i = 0; i < method_node->branches_num(); i++)
    {
        if ((*method_node)[i].value()->type() == NodeType::MET_PAR)
        {
            auto* mp_node = static_cast<MethodParameterNode*>((*method_node)[i].value().get());
            ss->write(reinterpret_cast<char*>(&mp_node->var_type), 1);
        }
    }
}