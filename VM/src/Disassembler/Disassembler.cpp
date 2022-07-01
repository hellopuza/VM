#include "Disassembler/Disassembler.h"

#include <iomanip>

#define HEX_OUT4 std::setfill('0') << std::setw(sizeof(int32_t) * 2) << std::hex << std::uppercase
#define HEX_OUT1 std::setfill('0') << std::setw(sizeof(int8_t) * 2) << std::hex << std::uppercase

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

bool Disassembler::disassemble(const std::string& file_name)
{
    std::ifstream file(file_name);
    if (file.is_open())
    {
        std::stringstream ss;
        ss << file.rdbuf();
        std::string klass = ss.str();

        size_t pos = 0;
        getClassName(klass, &pos);
        getConstantPool(klass, &pos);
        getFields(klass, &pos);
        getMethods(klass, &pos);
        getInstructions(klass, &pos);
    }
    else
    {
        return false;
    }

    return true;
}

void Disassembler::print(std::ostream& os) const
{
    os << text_.rdbuf() << "\n";
}

void Disassembler::getClassName(const std::string& klass, size_t* pos)
{
    text_ << "Class " << getString(klass, pos) << "\n";
}

void Disassembler::getConstantPool(const std::string& klass, size_t* pos)
{
    auto cp_size = *reinterpret_cast<const uint16_t*>(&klass[*pos]);
    (*pos) += sizeof(cp_size);

    text_ << "  Constant Pool [" << cp_size << "]\n";

    for (uint16_t i = 0; i < cp_size; i++)
    {
        auto type = static_cast<uint8_t>(klass[*pos]);
        (*pos)++;

        text_ << "    [" << i << "] ";
        switch (type)
        {
        case static_cast<uint8_t>(cp::AbstractType::Type::INTEGER):
        {
            auto value = *reinterpret_cast<const int32_t*>(&klass[*pos]);
            (*pos) += sizeof(value);
            const_pool_.emplace_back(std::make_unique<cp::IntegerType>(cp::IntegerType(value)));
            text_ << "Integer " << value << "\n";
            break;
        }
        case static_cast<uint8_t>(cp::AbstractType::Type::FLOAT):
        {
            auto value = *reinterpret_cast<const float*>(&klass[*pos]);
            (*pos) += sizeof(value);
            const_pool_.emplace_back(std::make_unique<cp::FloatType>(cp::FloatType(value)));
            text_ << "Float " << value << "\n";
            break;
        }
        case static_cast<uint8_t>(cp::AbstractType::Type::STRING):
        {
            auto value = getString(klass, pos);
            const_pool_.emplace_back(std::make_unique<cp::StringType>(cp::StringType(value)));
            text_ << "String \"" << value << "\"\n";
            break;
        }
        case static_cast<uint8_t>(cp::AbstractType::Type::SYMBOL):
        {
            auto value = *reinterpret_cast<const char*>(&klass[*pos]);
            (*pos)++;
            const_pool_.emplace_back(std::make_unique<cp::SymbolType>(cp::SymbolType(value)));
            text_ << "Symbol \'" << value << "\'\n";
            break;
        }
        case static_cast<uint8_t>(cp::AbstractType::Type::FIELD):
        {
            auto class_name = getString(klass, pos);
            auto field_name = getString(klass, pos);
            const_pool_.emplace_back(std::make_unique<cp::FieldType>(cp::FieldType(class_name, field_name)));
            text_ << "Field \"" << class_name << "." << field_name << "\"\n";
            break;
        }
        case static_cast<uint8_t>(cp::AbstractType::Type::METHOD):
        {
            auto class_name = getString(klass, pos);
            auto method_name = getString(klass, pos);
            const_pool_.emplace_back(std::make_unique<cp::MethodType>(cp::MethodType(class_name, method_name)));
            text_ << "Method \"" << class_name << "." << method_name << "\"\n";
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
                const_pool_.emplace_back(std::make_unique<cp::DataType>(cp::DataType(pkm::DataType(data_type, type_name))));
                text_ << "Type \"" << type_name << "\"\n";
                break;
            }
            const_pool_.emplace_back(std::make_unique<cp::DataType>(cp::DataType(pkm::DataType(data_type))));
            text_ << "Type \"" << pkm::DataType(data_type).name << "\"\n";
            break;
        }
        default:
            break;
        }
    }
    text_ << "\n";
}

void Disassembler::getFields(const std::string& klass, size_t* pos)
{
    auto fields_num = static_cast<int>(klass[*pos]);
    (*pos)++;

    text_ << "  Fields [" << fields_num << "]\n";

    for (int i = 0; i < fields_num; i++)
    {
        text_ << "    ";

        auto access_type = static_cast<uint8_t>(klass[*pos]);
        (*pos)++;
        text_ << pkm::strings::ACCESS[static_cast<int>(access_type)] << " ";

        auto var_type_ind = *reinterpret_cast<const uint16_t*>(&klass[*pos]);
        (*pos) += sizeof(var_type_ind);
        text_ << static_cast<cp::DataType*>(const_pool_[var_type_ind].get())->data_type.name << " ";

        auto name_ind = *reinterpret_cast<const uint16_t*>(&klass[*pos]);
        (*pos) += sizeof(name_ind);
        text_ << static_cast<cp::StringType*>(const_pool_[name_ind].get())->value << "\n";
    }
    text_ << "\n";
}

void Disassembler::getMethods(const std::string& klass, size_t* pos)
{
    auto methods_num = static_cast<int>(klass[*pos]);
    (*pos)++;

    text_ << "  Methods [" << methods_num << "]\n";

    for (int i = 0; i < methods_num; i++)
    {
        text_ << "    ";

        auto access_type = static_cast<uint8_t>(klass[*pos]);
        (*pos)++;
        text_ << pkm::strings::ACCESS[static_cast<int>(access_type)] << " ";

        auto modifier = static_cast<uint8_t>(klass[*pos]);
        (*pos)++;
        text_ << pkm::strings::METHOD[static_cast<int>(modifier)] << " ";

        auto ret_type_ind = *reinterpret_cast<const uint16_t*>(&klass[*pos]);
        (*pos) += sizeof(ret_type_ind);
        text_ << static_cast<cp::DataType*>(const_pool_[ret_type_ind].get())->data_type.name << " ";

        auto name_ind = *reinterpret_cast<const uint16_t*>(&klass[*pos]);
        (*pos) += sizeof(name_ind);
        text_ << static_cast<cp::StringType*>(const_pool_[name_ind].get())->value << "\n";

        auto mps_num = static_cast<uint8_t>(klass[*pos]);
        (*pos)++;

        text_ << "params(";
        for (uint8_t m = 0; m < mps_num; m++)
        {
            auto var_type_ind = *reinterpret_cast<const uint16_t*>(&klass[*pos]);
            (*pos) += sizeof(var_type_ind);
            text_ << static_cast<cp::DataType*>(const_pool_[var_type_ind].get())->data_type.name << " ";
        }
        text_ << ((mps_num == 0) ? ") " : "\b) ");

        auto offset = *reinterpret_cast<const uint32_t*>(&klass[*pos]);
        (*pos) += sizeof(offset);
        text_ << "offset(" << HEX_OUT4 << offset << ") ";

        auto locals_num = *reinterpret_cast<const uint16_t*>(&klass[*pos]);
        (*pos) += sizeof(locals_num);
        text_ << "locals_num(" << locals_num << ")\n";
    }
    text_ << "\n";
}

void Disassembler::getInstructions(const std::string& klass, size_t* pos)
{
    text_ << "  Instructions\n";
    size_t pc = 0;
    while (*pos < klass.size())
    {
        text_ << HEX_OUT4 << pc << ": ";
        for (size_t b = 0; b < sizeof(int32_t); b++)
        {
            const auto mask = 0xFF;
            auto byte = static_cast<int16_t>(klass[*pos]) & mask;
            (*pos)++;
            text_ << HEX_OUT1 << byte << " ";
        }
        text_ << "\b\n";

        pc += sizeof(int32_t);
    }
}