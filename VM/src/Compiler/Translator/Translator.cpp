#include "Compiler/Translator/Translator.h"
#include "Opcodes.h"

Translator::Translator(AST* ast) : ast_(ast) {}

void Translator::translate(std::ofstream* file)
{
    auto* class_node = static_cast<AST*>(&((*ast_)[0]));
    std::string class_name = static_cast<ClassNode*>(class_node->value().get())->name;
    file->write(class_name.c_str(), static_cast<std::streamsize>(class_name.length() + 1));

    std::stringstream class_content;
    writeFields(class_node, &class_content);

    std::stringstream instructions;
    writeMethods(class_node, &class_content, &instructions);

    writeConstantPool(file);
    *file << class_content.str();
    *file << instructions.str();
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
            file->write(reinterpret_cast<const char*>(value.c_str()), static_cast<std::streamsize>(value.length() + 1));
            break;
        }
        }
    }
}

void Translator::writeFields(AST* class_node, std::stringstream* class_content)
{
    uint8_t fields_num = 0;
    for (size_t i = 0; i < class_node->branches_num(); i++)
    {
        if ((*class_node)[i].value()->type() == NodeType::FIELD)
        {
            fields_num++;
        }
    }
    class_content->write(reinterpret_cast<char*>(&fields_num), sizeof(fields_num));

    for (size_t i = 0; i < class_node->branches_num(); i++)
    {
        if ((*class_node)[i].value()->type() == NodeType::FIELD)
        {
            auto* field_node = static_cast<FieldNode*>((*class_node)[i].value().get());
            class_content->write(reinterpret_cast<char*>(&field_node->access_type), 1);
            class_content->write(reinterpret_cast<char*>(&field_node->var_type), 1);

            auto cp_size = static_cast<uint16_t>(const_pool_.size());
            class_content->write(reinterpret_cast<char*>(&cp_size), sizeof(cp_size));
            const_pool_[std::make_unique<StringType>(StringType(field_node->name))] = cp_size;
        }
    }
}

void Translator::writeMethods(AST* class_node, std::stringstream* class_content, std::stringstream* instructions)
{
    uint8_t methods_num = 0;
    for (size_t i = 0; i < class_node->branches_num(); i++)
    {
        if ((*class_node)[i].value()->type() == NodeType::METHOD)
        {
            methods_num++;
        }
    }
    class_content->write(reinterpret_cast<char*>(&methods_num), sizeof(methods_num));

    for (size_t i = 0; i < class_node->branches_num(); i++)
    {
        if ((*class_node)[i].value()->type() == NodeType::METHOD)
        {
            auto* method_node = static_cast<MethodNode*>((*class_node)[i].value().get());
            class_content->write(reinterpret_cast<char*>(&method_node->access_type), 1);
            class_content->write(reinterpret_cast<char*>(&method_node->modifier), 1);
            class_content->write(reinterpret_cast<char*>(&method_node->ret_type), 1);

            auto cp_size = static_cast<uint16_t>(const_pool_.size());
            class_content->write(reinterpret_cast<char*>(&cp_size), sizeof(cp_size));
            const_pool_[std::make_unique<StringType>(StringType(method_node->name))] = cp_size;
            
            locals_.clear();
            writeMethodParams(static_cast<AST*>(&((*class_node)[i])), class_content);

            auto* scope_node = static_cast<AST*>(&(*class_node)[i][(*class_node)[i].branches_num() - 1]);
            uint32_t offset = writeInstructions(scope_node, instructions);
            class_content->write(reinterpret_cast<char*>(&offset), sizeof(offset));

            uint32_t null = 0;
            auto op_code = static_cast<uint8_t>(Opcode::RETURN);
            instructions->write(reinterpret_cast<char*>(&op_code), 1);
            instructions->write(reinterpret_cast<char*>(&null), 3);
        }
    }
}

void Translator::writeMethodParams(AST* method_node, std::stringstream* class_content)
{
    uint8_t mps_num = 0;
    for (size_t i = 0; i < method_node->branches_num(); i++)
    {
        if ((*method_node)[i].value()->type() == NodeType::MET_PAR)
        {
            mps_num++;
        }
    }
    class_content->write(reinterpret_cast<char*>(&mps_num), sizeof(mps_num));

    for (size_t i = 0; i < method_node->branches_num(); i++)
    {
        if ((*method_node)[i].value()->type() == NodeType::MET_PAR)
        {
            auto* mp_node = static_cast<MethodParameterNode*>((*method_node)[i].value().get());
            class_content->write(reinterpret_cast<char*>(&mp_node->var_type), 1);

            auto locals_size = static_cast<uint16_t>(locals_.size());
            locals_[mp_node->name] = std::make_pair(locals_size, mp_node->var_type);
        }
    }
}

void Translator::appendLocal(VariableDeclarationNode* var_decl_node)
{
    auto locals_size = static_cast<uint16_t>(locals_.size());
    locals_[var_decl_node->name] = std::make_pair(locals_size, var_decl_node->var_type);
}

uint32_t Translator::writeInstructions(AST* scope_node, std::stringstream* instructions)
{
    auto offset = static_cast<uint32_t>(instructions->tellg());
    for (size_t i = 0; i < scope_node->branches_num(); i++)
    {
        switch ((*scope_node)[i].value()->type())
        {
        case NodeType::SCOPE:
            writeInstructions(static_cast<AST*>(&(*scope_node)[i]), instructions);
            break;
        case NodeType::OPERATION:
            writeOperation(static_cast<AST*>(&(*scope_node)[i]), instructions);
            break;
        case NodeType::FUNCTION:
            writeFunction(static_cast<AST*>(&(*scope_node)[i]), instructions);
            break;
        case NodeType::VAR_DECL:
            appendLocal(static_cast<VariableDeclarationNode*>((*scope_node)[i].value().get()));
            break;
        default:
            break;
        }
    }
    return offset;
}

VariableType Translator::writeObject(AST* obj_node, std::stringstream* instructions)
{
    switch (obj_node->value()->type())
    {
    case NodeType::OPERATION:
        return writeOperation(obj_node, instructions);
    case NodeType::FUNCTION:
        return writeFunction(obj_node, instructions);
    case NodeType::VAR:
        return writeLoad(static_cast<VariableNode*>(obj_node->value().get())->name, instructions);
    case NodeType::NUMBER:
        return writeNumber(static_cast<NumberNode*>(obj_node->value().get()), instructions);
    default:
        break;
    }
    return VariableType::VOID;
}

VariableType Translator::writeOperation(AST* op_node, std::stringstream* instructions)
{
    switch (static_cast<OperationNode*>(op_node->value().get())->op_type)
    {
    case OperationType::ADD:
    case OperationType::SUB:
    case OperationType::MUL:
    case OperationType::DIV:
    {
        auto* lhs = static_cast<AST*>(&(*op_node)[0]);
        auto* rhs = static_cast<AST*>(&(*op_node)[1]);

        VariableType ret_type = writeObject(rhs, instructions);
        writeObject(lhs, instructions);

        auto op_type = static_cast<uint32_t>(static_cast<OperationNode*>(op_node->value().get())->op_type);
        op_type -= static_cast<uint32_t>(OperationType::ADD);

        uint32_t null = 0;
        uint32_t op_code = 0;
        switch (ret_type)
        {
        case VariableType::INT:
            op_code = static_cast<uint8_t>(Opcode::IADD) + op_type;
            break;
        case VariableType::LONG:
            op_code = static_cast<uint8_t>(Opcode::LADD) + op_type;
            break;
        case VariableType::FLOAT:
            op_code = static_cast<uint8_t>(Opcode::FADD) + op_type;
            break;
        case VariableType::DOUBLE:
            op_code = static_cast<uint8_t>(Opcode::DADD) + op_type;
            break;
        default:
            break;
        }

        instructions->write(reinterpret_cast<char*>(&op_code), 1);
        instructions->write(reinterpret_cast<char*>(&null), 3);
        return ret_type;
    }
    case OperationType::ASSIGN:
    {
        auto* lhs = &(*op_node)[0];
        auto* rhs = &(*op_node)[1];

        VariableType ret_type = writeObject(static_cast<AST*>(rhs), instructions);

        switch (lhs->value()->type())
        {
        case NodeType::VAR_DECL:
        {
            auto* var_decl_node = static_cast<VariableDeclarationNode*>(lhs->value().get());
            appendLocal(var_decl_node);
            writeStore(var_decl_node->name, instructions);
            return ret_type;
        }
        case NodeType::VAR:
        {
            auto* var_node = static_cast<VariableNode*>(lhs->value().get());
            writeStore(var_node->name, instructions);
            return ret_type;
        }
        default:
            break;
        }
        return ret_type;
    }
    case OperationType::RETURN:
    {
        VariableType ret_type = writeObject(static_cast<AST*>(&(*op_node)[0]), instructions);

        uint32_t null = 0;
        uint8_t op_code = 0;
        switch (ret_type)
        {
        case VariableType::INT:
            op_code = static_cast<uint8_t>(Opcode::IRETURN);
            break;
        case VariableType::LONG:
            op_code = static_cast<uint8_t>(Opcode::LRETURN);
            break;
        case VariableType::FLOAT:
            op_code = static_cast<uint8_t>(Opcode::FRETURN);
            break;
        case VariableType::DOUBLE:
            op_code = static_cast<uint8_t>(Opcode::DRETURN);
            break;
        default:
            break;
        }

        instructions->write(reinterpret_cast<char*>(&op_code), 1);
        instructions->write(reinterpret_cast<char*>(&null), 3);

        return ret_type;
    }
    default:
        break;
    }

    return VariableType::VOID;
}

VariableType Translator::writeFunction(AST* func_node, std::stringstream* instructions)
{
    for (size_t i = 0; i < func_node->branches_num(); i++)
    {
        writeObject(static_cast<AST*>(&(*func_node)[i]), instructions);
    }

    uint8_t null = 0;
    auto op_code = static_cast<uint8_t>(Opcode::INVOKESTATIC);

    auto cp_size = static_cast<uint16_t>(const_pool_.size());
    std::string func_name = static_cast<FunctionNode*>(func_node->value().get())->name;

    if (!const_pool_.contains(std::make_unique<StringType>(StringType(func_name))))
    {
        const_pool_[std::make_unique<StringType>(StringType(func_name))] = cp_size;
    }
    else
    {
        cp_size = const_pool_[std::make_unique<StringType>(StringType(func_name))];
    }

    instructions->write(reinterpret_cast<char*>(&op_code), 1);
    instructions->write(reinterpret_cast<char*>(&null), 1);
    instructions->write(reinterpret_cast<char*>(&cp_size), sizeof(cp_size));

    return VariableType::INT;
}

VariableType Translator::writeNumber(NumberNode* num_node, std::stringstream* instructions)
{
    uint8_t null = 0;
    auto op_code = static_cast<uint8_t>(Opcode::LDC);
    auto cp_size = static_cast<uint16_t>(const_pool_.size());
    VariableType ret_type = VariableType::VOID;

    switch (num_node->num_type)
    {
    case VariableType::BOOLEAN:
    case VariableType::INT:
        if (!const_pool_.contains(std::make_unique<IntegerType>(IntegerType(num_node->number.i))))
        {
            const_pool_[std::make_unique<IntegerType>(IntegerType(num_node->number.i))] = cp_size;
        }
        else
        {
            cp_size = const_pool_[std::make_unique<IntegerType>(IntegerType(num_node->number.i))];
        }
        ret_type = VariableType::INT;
        break;
    case VariableType::FLOAT:
        if (!const_pool_.contains(std::make_unique<FloatType>(FloatType(num_node->number.f))))
        {
            const_pool_[std::make_unique<FloatType>(FloatType(num_node->number.f))] = cp_size;
        }
        else
        {
            cp_size = const_pool_[std::make_unique<FloatType>(FloatType(num_node->number.f))];
        }
        ret_type = VariableType::FLOAT;
        break;
    default:
        break;
    }

    instructions->write(reinterpret_cast<char*>(&op_code), 1);
    instructions->write(reinterpret_cast<char*>(&null), 1);
    instructions->write(reinterpret_cast<char*>(&cp_size), sizeof(cp_size));

    return ret_type;
}

void Translator::writeStore(const std::string& name, std::stringstream* instructions)
{
    uint8_t null = 0;
    uint8_t op_code = 0;
    uint16_t index = locals_[name].first;

    switch (locals_[name].second)
    {
    case VariableType::BOOLEAN:
    case VariableType::BYTE:
    case VariableType::CHAR:
    case VariableType::SHORT:
    case VariableType::INT:
        op_code = static_cast<uint8_t>(Opcode::ISTORE);
        break;
    case VariableType::LONG:
        op_code = static_cast<uint8_t>(Opcode::LSTORE);
        break;
    case VariableType::FLOAT:
        op_code = static_cast<uint8_t>(Opcode::FSTORE);
        break;
    case VariableType::DOUBLE:
        op_code = static_cast<uint8_t>(Opcode::DSTORE);
        break;
    default:
        break;
    }

    instructions->write(reinterpret_cast<char*>(&op_code), 1);
    instructions->write(reinterpret_cast<char*>(&null), 1);
    instructions->write(reinterpret_cast<char*>(&index), sizeof(index));
}

VariableType Translator::writeLoad(const std::string& name, std::stringstream* instructions)
{
    uint8_t null = 0;
    uint8_t op_code = 0;
    uint16_t index = locals_[name].first;
    VariableType ret_type = VariableType::VOID;

    switch (locals_[name].second)
    {
    case VariableType::BOOLEAN:
    case VariableType::BYTE:
    case VariableType::CHAR:
    case VariableType::SHORT:
    case VariableType::INT:
        op_code = static_cast<uint8_t>(Opcode::ILOAD);
        ret_type = VariableType::INT;
        break;
    case VariableType::LONG:
        op_code = static_cast<uint8_t>(Opcode::LLOAD);
        ret_type = VariableType::LONG;
        break;
    case VariableType::FLOAT:
        op_code = static_cast<uint8_t>(Opcode::FLOAD);
        ret_type = VariableType::FLOAT;
        break;
    case VariableType::DOUBLE:
        op_code = static_cast<uint8_t>(Opcode::DLOAD);
        ret_type = VariableType::DOUBLE;
        break;
    default:
        break;
    }

    instructions->write(reinterpret_cast<char*>(&op_code), 1);
    instructions->write(reinterpret_cast<char*>(&null), 1);
    instructions->write(reinterpret_cast<char*>(&index), sizeof(index));

    return ret_type;
}