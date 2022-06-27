#include "Compiler/Translator/Translator.h"
#include "Opcodes.h"

#include <unordered_map>

using ast::AST;

template <typename T>
static uint16_t getConstPoolSize(ConstantPool* cp, const T& key)
{
    auto cp_size = static_cast<uint16_t>(cp->size());
    if (!cp->contains(std::make_unique<T>(key)))
    {
        (*cp)[std::make_unique<T>(key)] = cp_size;
    }
    else
    {
        cp_size = (*cp)[std::make_unique<T>(key)];
    }
    return cp_size;
}

Translator::Translator(AST* class_tree) : class_tree_(class_tree) {}

void Translator::translate(std::ofstream* output_file)
{
    auto* class_node = class_tree_;
    std::string class_name = static_cast<ast::ClassNode*>(class_node->value().get())->name;
    output_file->write(class_name.c_str(), static_cast<std::streamsize>(class_name.length() + 1));

    std::stringstream class_content;
    writeFields(class_node, &class_content);

    std::stringstream instructions;
    writeMethods(class_node, &class_content, &instructions);

    writeConstantPool(output_file);
    *output_file << class_content.str();
    *output_file << instructions.str();
}

void Translator::writeConstantPool(std::ofstream* output_file)
{
    auto cp_size = static_cast<uint16_t>(const_pool_.size());
    output_file->write(reinterpret_cast<char*>(&cp_size), sizeof(cp_size));

    std::vector<std::pair<cp::AbstractType*, uint16_t>> elems;
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
        output_file->write(reinterpret_cast<char*>(&type), sizeof(type));
        switch (type)
        {
        case static_cast<uint8_t>(cp::AbstractType::Type::INTEGER):
        {
            auto value = static_cast<cp::IntegerType*>(elem.first)->value;
            output_file->write(reinterpret_cast<char*>(&value), sizeof(value));
            break;
        }
        case static_cast<uint8_t>(cp::AbstractType::Type::FLOAT):
        {
            auto value = static_cast<cp::FloatType*>(elem.first)->value;
            output_file->write(reinterpret_cast<char*>(&value), sizeof(value));
            break;
        }
        case static_cast<uint8_t>(cp::AbstractType::Type::STRING):
        {
            auto value = static_cast<cp::StringType*>(elem.first)->value;
            output_file->write(reinterpret_cast<const char*>(value.c_str()), static_cast<std::streamsize>(value.length() + 1));
            break;
        }
        case static_cast<uint8_t>(cp::AbstractType::Type::SYMBOL):
        {
            auto value = static_cast<cp::SymbolType*>(elem.first)->value;
            output_file->write(reinterpret_cast<char*>(&value), 1);
            break;
        }
        case static_cast<uint8_t>(cp::AbstractType::Type::FIELD):
        {
            auto class_name = static_cast<cp::FieldType*>(elem.first)->class_name;
            auto field_name = static_cast<cp::FieldType*>(elem.first)->field_name;
            output_file->write(reinterpret_cast<const char*>(class_name.c_str()), static_cast<std::streamsize>(class_name.length() + 1));
            output_file->write(reinterpret_cast<const char*>(field_name.c_str()), static_cast<std::streamsize>(field_name.length() + 1));
            break;
        }
        case static_cast<uint8_t>(cp::AbstractType::Type::METHOD):
        {
            auto class_name = static_cast<cp::MethodType*>(elem.first)->class_name;
            auto method_name = static_cast<cp::MethodType*>(elem.first)->method_name;
            output_file->write(reinterpret_cast<const char*>(class_name.c_str()), static_cast<std::streamsize>(class_name.length() + 1));
            output_file->write(reinterpret_cast<const char*>(method_name.c_str()), static_cast<std::streamsize>(method_name.length() + 1));
            break;
        }
        case static_cast<uint8_t>(cp::AbstractType::Type::POINTER):
            break;
        case static_cast<uint8_t>(cp::AbstractType::Type::DATA_TYPE):
        {
            auto data_type = static_cast<cp::DataType*>(elem.first)->data_type.type;
            output_file->write(reinterpret_cast<char*>(&data_type), 1);

            if (data_type == pkm::VariableType::REFERENCE)
            {
                auto name = static_cast<cp::DataType*>(elem.first)->data_type.name;
                output_file->write(reinterpret_cast<const char*>(name.c_str()), static_cast<std::streamsize>(name.length() + 1));
                break;
            }
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
        auto* node = static_cast<AST*>(&(*class_node)[i]);
        if (node->value()->type() == pkm::ASTNodeType::FIELD)
        {
            fields_num++;
        }
    }
    class_content->write(reinterpret_cast<char*>(&fields_num), 1);

    for (size_t i = 0; i < class_node->branches_num(); i++)
    {
        auto* node = static_cast<AST*>(&(*class_node)[i]);
        if (node->value()->type() == pkm::ASTNodeType::FIELD)
        {
            auto* field_node = static_cast<ast::FieldNode*>(node->value().get());
            class_content->write(reinterpret_cast<char*>(&field_node->access_type), 1);

            uint16_t cp_size = getConstPoolSize(&const_pool_, cp::DataType(field_node->var_type));
            class_content->write(reinterpret_cast<char*>(&cp_size), sizeof(cp_size));

            cp_size = getConstPoolSize(&const_pool_, cp::StringType(field_node->name));
            class_content->write(reinterpret_cast<char*>(&cp_size), sizeof(cp_size));
        }
    }
}

void Translator::writeMethods(AST* class_node, std::stringstream* class_content, std::stringstream* instructions)
{
    uint8_t methods_num = 0;
    for (size_t i = 0; i < class_node->branches_num(); i++)
    {
        auto* node = static_cast<AST*>(&(*class_node)[i]);
        if (node->value()->type() == pkm::ASTNodeType::METHOD)
        {
            methods_num++;
        }
    }
    class_content->write(reinterpret_cast<char*>(&methods_num), 1);

    for (size_t i = 0; i < class_node->branches_num(); i++)
    {
        auto* node = static_cast<AST*>(&(*class_node)[i]);
        if (node->value()->type() == pkm::ASTNodeType::METHOD)
        {
            auto* method_node = static_cast<ast::MethodNode*>(node->value().get());
            class_content->write(reinterpret_cast<char*>(&method_node->access_type), 1);
            class_content->write(reinterpret_cast<char*>(&method_node->modifier), 1);

            uint16_t cp_size = getConstPoolSize(&const_pool_, cp::DataType(method_node->ret_type));
            class_content->write(reinterpret_cast<char*>(&cp_size), sizeof(cp_size));

            cp_size = getConstPoolSize(&const_pool_, cp::StringType(method_node->name));
            class_content->write(reinterpret_cast<char*>(&cp_size), sizeof(cp_size));

            locals_.clear();
            if (method_node->modifier == pkm::MethodType::INSTANCE)
            {
                locals_["this"] = std::make_pair(0, pkm::DataType(pkm::VariableType::REFERENCE,
                    static_cast<ast::ClassNode*>(class_node->value().get())->name));
            }
            writeMethodParams(node, class_content);

            auto* scope_node = static_cast<AST*>(&(*node)[node->branches_num() - 1]);
            uint32_t offset = writeInstructions(scope_node, instructions);
            class_content->write(reinterpret_cast<char*>(&offset), sizeof(offset));

            auto locals_num = static_cast<uint16_t>(locals_.size());
            class_content->write(reinterpret_cast<char*>(&locals_num), sizeof(locals_num));
        }
    }
}

void Translator::writeMethodParams(AST* method_node, std::stringstream* class_content)
{
    auto mps_num = static_cast<uint8_t>(method_node->branches_num() - 1);
    class_content->write(reinterpret_cast<char*>(&mps_num), 1);

    for (size_t i = 0; i < method_node->branches_num() - 1; i++)
    {
        auto* node = static_cast<AST*>(&(*method_node)[i]);
        auto* mp_node = static_cast<ast::MethodParameterNode*>(node->value().get());

        uint16_t cp_size = getConstPoolSize(&const_pool_, cp::DataType(mp_node->var_type));
        class_content->write(reinterpret_cast<char*>(&cp_size), sizeof(cp_size));

        auto locals_size = static_cast<uint16_t>(locals_.size());
        locals_[mp_node->name] = std::make_pair(locals_size, mp_node->var_type);
    }
}

void Translator::appendLocal(ast::VariableDeclarationNode* var_decl_node)
{
    if (!locals_.contains(var_decl_node->name))
    {
        auto locals_size = static_cast<uint16_t>(locals_.size());
        locals_[var_decl_node->name] = std::make_pair(locals_size, var_decl_node->var_type);
    }
}

uint32_t Translator::writeInstructions(AST* scope_node, std::stringstream* instructions)
{
    instructions->seekg(0, std::ios::end);
    auto offset = static_cast<uint32_t>(instructions->tellg());
    for (size_t i = 0; i < scope_node->branches_num(); i++)
    {
        auto* node = static_cast<AST*>(&(*scope_node)[i]);
        switch (node->value()->type())
        {
        case pkm::ASTNodeType::SCOPE:
            writeInstructions(node, instructions);
            break;
        case pkm::ASTNodeType::CONTROL:
            writeControl(node, instructions);
            break;
        case pkm::ASTNodeType::VAR_DECL:
            appendLocal(static_cast<ast::VariableDeclarationNode*>(node->value().get()));
            break;
        case pkm::ASTNodeType::OPERATION:
            writeOperation(node, instructions, true);
            break;
        case pkm::ASTNodeType::FUNCTION:
            writeFunction(node, instructions, true);
            break;
        default:
            writeObject(node, instructions);
            break;
        }
    }
    return offset;
}

void Translator::writeObject(AST* obj_node, std::stringstream* instructions)
{
    switch (obj_node->value()->type())
    {
    case pkm::ASTNodeType::OPERATION:
        writeOperation(obj_node, instructions);
        break;
    case pkm::ASTNodeType::FUNCTION:
        writeFunction(obj_node, instructions);
        break;
    case pkm::ASTNodeType::VARIABLE:
        writeVariable(obj_node, instructions, true);
        break;
    case pkm::ASTNodeType::CONVERSION:
        writeConversion(obj_node, instructions);
        break;
    case pkm::ASTNodeType::NUMBER:
        writeNumber(static_cast<ast::NumberNode*>(obj_node->value().get()), instructions);
        break;
    case pkm::ASTNodeType::STRING:
        writeString(static_cast<ast::StringNode*>(obj_node->value().get()), instructions);
        break;
    case pkm::ASTNodeType::SYMBOL:
        writeSymbol(static_cast<ast::SymbolNode*>(obj_node->value().get()), instructions);
        break;
    default:
        break;
    }
}

void Translator::writeOperation(AST* op_node, std::stringstream* instructions, bool in_scope)
{
    pkm::DataType op_data_type = static_cast<ast::OperationNode*>(op_node->value().get())->var_type;
    switch (static_cast<ast::OperationNode*>(op_node->value().get())->op_type)
    {
    case pkm::OperationType::RETURN:
    {
        uint32_t null = 0;
        uint8_t op_code = 0;
        if (op_node->branches_num() > 0)
        {
            writeObject(static_cast<AST*>(&(*op_node)[0]), instructions);

            switch (op_data_type.type)
            {
            case pkm::VariableType::CHAR:
            case pkm::VariableType::INT:
                op_code = static_cast<uint8_t>(Opcode::IRETURN);
                break;
            case pkm::VariableType::FLOAT:
                op_code = static_cast<uint8_t>(Opcode::FRETURN);
                break;
            case pkm::VariableType::REFERENCE:
                op_code = static_cast<uint8_t>(Opcode::ARETURN);
                break;
            default:
                break;
            }
        }
        else
        {
            op_code = static_cast<uint8_t>(Opcode::RETURN);
        }

        instructions->write(reinterpret_cast<char*>(&op_code), 1);
        instructions->write(reinterpret_cast<char*>(&null), 3);
        break;
    }
    case pkm::OperationType::ASSIGN:
    {
        auto* lhs = &(*op_node)[0];
        auto* rhs = &(*op_node)[1];

        writeObject(static_cast<AST*>(rhs), instructions);

        switch (lhs->value()->type())
        {
        case pkm::ASTNodeType::VAR_DECL:
        {
            auto* var_decl_node = static_cast<ast::VariableDeclarationNode*>(lhs->value().get());
            appendLocal(var_decl_node);
            writeStore(var_decl_node->name, instructions);
            break;
        }
        case pkm::ASTNodeType::VARIABLE:
        {
            writeVariable(static_cast<AST*>(lhs), instructions, false);
            break;
        }
        default:
            break;
        }
        break;
    }
    case pkm::OperationType::OR:
    case pkm::OperationType::AND:
    {
        AST ast(std::make_shared<ast::ControlNode>(ast::ControlNode(pkm::ControlType::IF)));
        ast.emplace_branch(std::move(*op_node));
        ast.emplace_branch(std::make_shared<ast::ScopeNode>(ast::ScopeNode()));
        ast[1].emplace_branch(std::make_shared<ast::NumberNode>(ast::NumberNode(1)));
        ast.emplace_branch(std::make_shared<ast::ControlNode>(ast::ControlNode(pkm::ControlType::ELSE)));
        ast[2].emplace_branch(std::make_shared<ast::ScopeNode>(ast::ScopeNode()));
        ast[2][0].emplace_branch(std::make_shared<ast::NumberNode>(ast::NumberNode(0)));

        writeControl(&ast, instructions);

        if (in_scope)
        {
            uint32_t null = 0;
            uint32_t op_code = static_cast<uint8_t>(Opcode::POP);
            instructions->write(reinterpret_cast<char*>(&op_code), 1);
            instructions->write(reinterpret_cast<char*>(&null), 3);
        }
        break;
    }
    case pkm::OperationType::EQ:
    case pkm::OperationType::NEQ:
    case pkm::OperationType::LEQ:
    case pkm::OperationType::GEQ:
    case pkm::OperationType::STL:
    case pkm::OperationType::STG:
    {
        auto* lhs = static_cast<AST*>(&(*op_node)[0]);
        auto* rhs = static_cast<AST*>(&(*op_node)[1]);
        writeObject(rhs, instructions);
        writeObject(lhs, instructions);

        auto op_type = static_cast<uint32_t>(static_cast<ast::OperationNode*>(op_node->value().get())->op_type);
        op_type -= static_cast<uint32_t>(pkm::OperationType::EQ);

        uint32_t null = 0;
        uint32_t op_code = 0;
        switch (op_data_type.type)
        {
        case pkm::VariableType::CHAR:
        case pkm::VariableType::INT:
            op_code = static_cast<uint8_t>(Opcode::IEQ) + op_type;
            break;
        case pkm::VariableType::FLOAT:
            op_code = static_cast<uint8_t>(Opcode::FEQ) + op_type;
            break;
        default:
            break;
        }

        instructions->write(reinterpret_cast<char*>(&op_code), 1);
        instructions->write(reinterpret_cast<char*>(&null), 3);

        if (in_scope)
        {
            op_code = static_cast<uint8_t>(Opcode::POP);
            instructions->write(reinterpret_cast<char*>(&op_code), 1);
            instructions->write(reinterpret_cast<char*>(&null), 3);
        }
        break;
    }
    case pkm::OperationType::SHL:
    case pkm::OperationType::SHR:
    {
        auto* lhs = static_cast<AST*>(&(*op_node)[0]);
        auto* rhs = static_cast<AST*>(&(*op_node)[1]);
        writeObject(rhs, instructions);
        writeObject(lhs, instructions);

        uint32_t null = 0;
        uint32_t op_code = 0;
        switch (static_cast<ast::OperationNode*>(op_node->value().get())->op_type)
        {
        case pkm::OperationType::SHL:
            op_code = static_cast<uint8_t>(Opcode::ISHL);
            break;
        case pkm::OperationType::SHR:
            op_code = static_cast<uint8_t>(Opcode::ISHR);
            break;
        default:
            break;
        }

        instructions->write(reinterpret_cast<char*>(&op_code), 1);
        instructions->write(reinterpret_cast<char*>(&null), 3);

        if (in_scope)
        {
            op_code = static_cast<uint8_t>(Opcode::POP);
            instructions->write(reinterpret_cast<char*>(&op_code), 1);
            instructions->write(reinterpret_cast<char*>(&null), 3);
        }
        break;
    }
    case pkm::OperationType::ADD:
    case pkm::OperationType::SUB:
    case pkm::OperationType::MUL:
    case pkm::OperationType::DIV:
    {
        if (op_node->branches_num() == 1)
        {
            auto* arg = static_cast<AST*>(&(*op_node)[0]);
            writeObject(arg, instructions);

            auto op_type = static_cast<uint32_t>(static_cast<ast::OperationNode*>(op_node->value().get())->op_type);
            if (op_type == static_cast<uint32_t>(pkm::OperationType::SUB))
            {
                uint32_t null = 0;
                uint32_t op_code = 0;
                switch (op_data_type.type)
                {
                case pkm::VariableType::CHAR:
                case pkm::VariableType::INT:
                    op_code = static_cast<uint8_t>(Opcode::INEG);
                    break;
                case pkm::VariableType::FLOAT:
                    op_code = static_cast<uint8_t>(Opcode::FNEG);
                    break;
                default:
                    break;
                }

                instructions->write(reinterpret_cast<char*>(&op_code), 1);
                instructions->write(reinterpret_cast<char*>(&null), 3);

                if (in_scope)
                {
                    op_code = static_cast<uint8_t>(Opcode::POP);
                    instructions->write(reinterpret_cast<char*>(&op_code), 1);
                    instructions->write(reinterpret_cast<char*>(&null), 3);
                }
            }
            break;
        }

        auto* lhs = static_cast<AST*>(&(*op_node)[0]);
        auto* rhs = static_cast<AST*>(&(*op_node)[1]);
        writeObject(rhs, instructions);
        writeObject(lhs, instructions);

        auto op_type = static_cast<uint32_t>(static_cast<ast::OperationNode*>(op_node->value().get())->op_type);
        op_type -= static_cast<uint32_t>(pkm::OperationType::ADD);

        uint32_t null = 0;
        uint32_t op_code = 0;
        switch (op_data_type.type)
        {
        case pkm::VariableType::CHAR:
        case pkm::VariableType::INT:
            op_code = static_cast<uint8_t>(Opcode::IADD) + op_type;
            break;
        case pkm::VariableType::FLOAT:
            op_code = static_cast<uint8_t>(Opcode::FADD) + op_type;
            break;
        default:
            break;
        }

        instructions->write(reinterpret_cast<char*>(&op_code), 1);
        instructions->write(reinterpret_cast<char*>(&null), 3);

        if (in_scope)
        {
            op_code = static_cast<uint8_t>(Opcode::POP);
            instructions->write(reinterpret_cast<char*>(&op_code), 1);
            instructions->write(reinterpret_cast<char*>(&null), 3);
        }
        break;
    }
    case pkm::OperationType::REM:
    {
        auto* lhs = static_cast<AST*>(&(*op_node)[0]);
        auto* rhs = static_cast<AST*>(&(*op_node)[1]);
        writeObject(rhs, instructions);
        writeObject(lhs, instructions);

        uint32_t null = 0;
        uint32_t op_code = 0;
        switch (op_data_type.type)
        {
        case pkm::VariableType::CHAR:
        case pkm::VariableType::INT:
            op_code = static_cast<uint8_t>(Opcode::IREM);
            break;
        case pkm::VariableType::FLOAT:
            op_code = static_cast<uint8_t>(Opcode::FREM);
            break;
        default:
            break;
        }

        instructions->write(reinterpret_cast<char*>(&op_code), 1);
        instructions->write(reinterpret_cast<char*>(&null), 3);

        if (in_scope)
        {
            op_code = static_cast<uint8_t>(Opcode::POP);
            instructions->write(reinterpret_cast<char*>(&op_code), 1);
            instructions->write(reinterpret_cast<char*>(&null), 3);
        }
        break;
    }
    case pkm::OperationType::NOT:
    {
        AST ast(std::make_shared<ast::ControlNode>(ast::ControlNode(pkm::ControlType::IF)));
        ast.emplace_branch(std::move(*static_cast<AST*>(&(*op_node)[0])));
        ast.emplace_branch(std::make_shared<ast::ScopeNode>(ast::ScopeNode()));
        ast[1].emplace_branch(std::make_shared<ast::NumberNode>(ast::NumberNode(0)));
        ast.emplace_branch(std::make_shared<ast::ControlNode>(ast::ControlNode(pkm::ControlType::ELSE)));
        ast[2].emplace_branch(std::make_shared<ast::ScopeNode>(ast::ScopeNode()));
        ast[2][0].emplace_branch(std::make_shared<ast::NumberNode>(ast::NumberNode(1)));

        writeControl(&ast, instructions);

        if (in_scope)
        {
            uint32_t null = 0;
            uint32_t op_code = static_cast<uint8_t>(Opcode::POP);
            instructions->write(reinterpret_cast<char*>(&op_code), 1);
            instructions->write(reinterpret_cast<char*>(&null), 3);
        }
        break;
    }
    case pkm::OperationType::SQR_BR:
    {
    }
    case pkm::OperationType::NEW:
    {
        uint8_t null = 0;
        auto op_code = static_cast<uint8_t>(Opcode::NEW);
        uint16_t cp_size = getConstPoolSize(&const_pool_, cp::DataType(op_data_type));

        instructions->write(reinterpret_cast<char*>(&op_code), 1);
        instructions->write(reinterpret_cast<char*>(&null), 1);
        instructions->write(reinterpret_cast<char*>(&cp_size), sizeof(cp_size));
        break;
    }
    }
}

void Translator::writeConversion(AST* conv_node, std::stringstream* instructions)
{
    auto* arg = static_cast<AST*>(&(*conv_node)[0]);
    writeObject(arg, instructions);

    auto from_type = static_cast<ast::ConversionNode*>(conv_node->value().get())->from;
    auto to_type = static_cast<ast::ConversionNode*>(conv_node->value().get())->to;

    uint32_t null = 0;
    uint32_t op_code = 0;
    if ((from_type.type == pkm::VariableType::INT) && (to_type.type == pkm::VariableType::CHAR))
    {
        op_code = static_cast<uint8_t>(Opcode::I2C);
    }
    else if ((from_type.type == pkm::VariableType::INT) && (to_type.type == pkm::VariableType::FLOAT))
    {
        op_code = static_cast<uint8_t>(Opcode::I2F);
    }
    else if ((from_type.type == pkm::VariableType::FLOAT) && (to_type.type == pkm::VariableType::INT))
    {
        op_code = static_cast<uint8_t>(Opcode::F2I);
    }
    else
    {
        return;
    }

    instructions->write(reinterpret_cast<char*>(&op_code), 1);
    instructions->write(reinterpret_cast<char*>(&null), 3);
}

void Translator::writeVariable(AST* var_node, std::stringstream* instructions, bool loading)
{
    auto* variable_node = static_cast<ast::VariableNode*>(var_node->value().get());

    if (variable_node->name_parts.size() == 1)
    {
        if (loading)
        {
            writeLoad(variable_node->name, instructions);
        }
        else
        {
            writeStore(variable_node->name, instructions);
        }
    }
    else
    {
        writeLoad(variable_node->name_parts[0].first, instructions);

        for (size_t i = 1; i < variable_node->name_parts.size(); i++)
        {
            uint16_t cp_size = getConstPoolSize(&const_pool_,
                cp::FieldType(variable_node->name_parts[i - 1].second.name, variable_node->name_parts[i].first));

            uint8_t null = 0;
            auto op_code = static_cast<uint8_t>(Opcode::GETFIELD);

            if ((i == variable_node->name_parts.size() - 1) && !loading)
            {
                op_code = static_cast<uint8_t>(Opcode::PUTFIELD);
            }

            instructions->write(reinterpret_cast<char*>(&op_code), 1);
            instructions->write(reinterpret_cast<char*>(&null), 1);
            instructions->write(reinterpret_cast<char*>(&cp_size), sizeof(cp_size));
        }
    }
}

void Translator::writeFunction(AST* func_node, std::stringstream* instructions, bool in_scope)
{
    for (size_t i = 0; i < func_node->branches_num(); i++)
    {
        writeObject(static_cast<AST*>(&(*func_node)[i]), instructions);
    }

    auto* function_node = static_cast<ast::FunctionNode*>(func_node->value().get());

    if (function_node->modifier == pkm::MethodType::INSTANCE)
    {
        writeLoad(function_node->name_parts[0].first, instructions);

        for (size_t i = 1; i < function_node->name_parts.size(); i++)
        {
            uint16_t cp_size = getConstPoolSize(&const_pool_,
                cp::FieldType(function_node->name_parts[i - 1].second.name, function_node->name_parts[i].first));

            uint8_t null = 0;
            auto op_code = static_cast<uint8_t>(Opcode::GETFIELD);

            if (i == function_node->name_parts.size() - 1)
            {
                op_code = static_cast<uint8_t>(Opcode::INVOKEINSTANCE);

                cp_size = getConstPoolSize(&const_pool_,
                    cp::MethodType(function_node->name_parts[i - 1].second.name, function_node->name_parts[i].first));
            }

            instructions->write(reinterpret_cast<char*>(&op_code), 1);
            instructions->write(reinterpret_cast<char*>(&null), 1);
            instructions->write(reinterpret_cast<char*>(&cp_size), sizeof(cp_size));
        }
    }
    else
    {
        uint16_t cp_size = getConstPoolSize(&const_pool_,
            cp::MethodType(function_node->name_parts[0].first, function_node->name_parts[1].first));

        uint8_t null = 0;
        auto op_code = static_cast<uint8_t>((function_node->modifier == pkm::MethodType::STATIC) ?
            Opcode::INVOKESTATIC : Opcode::INVOKENATIVE);

        instructions->write(reinterpret_cast<char*>(&op_code), 1);
        instructions->write(reinterpret_cast<char*>(&null), 1);
        instructions->write(reinterpret_cast<char*>(&cp_size), sizeof(cp_size));
    }

    if (in_scope && (function_node->ret_type.type != pkm::VariableType::VOID))
    {
        uint32_t null = 0;
        auto op_code = static_cast<uint8_t>(Opcode::POP);
        instructions->write(reinterpret_cast<char*>(&op_code), 1);
        instructions->write(reinterpret_cast<char*>(&null), 3);
    }
}

void Translator::writeNumber(ast::NumberNode* num_node, std::stringstream* instructions)
{
    uint8_t null = 0;
    uint16_t cp_size = 0;
    auto op_code = static_cast<uint8_t>(Opcode::LDC);

    switch (num_node->num_type)
    {
    case pkm::VariableType::INT:
        cp_size = getConstPoolSize(&const_pool_, cp::IntegerType(num_node->number.i));
        break;
    case pkm::VariableType::FLOAT:
        cp_size = getConstPoolSize(&const_pool_, cp::FloatType(num_node->number.f));
        break;
    default:
        break;
    }

    instructions->write(reinterpret_cast<char*>(&op_code), 1);
    instructions->write(reinterpret_cast<char*>(&null), 1);
    instructions->write(reinterpret_cast<char*>(&cp_size), sizeof(cp_size));
}

void Translator::writeString(ast::StringNode* str_node, std::stringstream* instructions)
{
    uint8_t null = 0;
    uint16_t cp_size = getConstPoolSize(&const_pool_, cp::StringType(str_node->value));
    auto op_code = static_cast<uint8_t>(Opcode::LDC);

    instructions->write(reinterpret_cast<char*>(&op_code), 1);
    instructions->write(reinterpret_cast<char*>(&null), 1);
    instructions->write(reinterpret_cast<char*>(&cp_size), sizeof(cp_size));
}

void Translator::writeSymbol(ast::SymbolNode* sym_node, std::stringstream* instructions)
{
    uint8_t null = 0;
    uint16_t cp_size = getConstPoolSize(&const_pool_, cp::SymbolType(sym_node->value));
    auto op_code = static_cast<uint8_t>(Opcode::LDC);

    instructions->write(reinterpret_cast<char*>(&op_code), 1);
    instructions->write(reinterpret_cast<char*>(&null), 1);
    instructions->write(reinterpret_cast<char*>(&cp_size), sizeof(cp_size));
}

void Translator::writeLoad(const std::string& name, std::stringstream* instructions)
{
    uint8_t null = 0;
    uint8_t op_code = 0;
    uint16_t index = locals_[name].first;

    switch (locals_[name].second.type)
    {
    case pkm::VariableType::CHAR:
    case pkm::VariableType::INT:
        op_code = static_cast<uint8_t>(Opcode::ILOAD);
        break;
    case pkm::VariableType::FLOAT:
        op_code = static_cast<uint8_t>(Opcode::FLOAD);
        break;
    case pkm::VariableType::REFERENCE:
        op_code = static_cast<uint8_t>(Opcode::ALOAD);
        break;
    default:
        break;
    }

    instructions->write(reinterpret_cast<char*>(&op_code), 1);
    instructions->write(reinterpret_cast<char*>(&null), 1);
    instructions->write(reinterpret_cast<char*>(&index), sizeof(index));
}

void Translator::writeStore(const std::string& name, std::stringstream* instructions)
{
    uint8_t null = 0;
    uint8_t op_code = 0;
    uint16_t index = locals_[name].first;

    switch (locals_[name].second.type)
    {
    case pkm::VariableType::CHAR:
    case pkm::VariableType::INT:
        op_code = static_cast<uint8_t>(Opcode::ISTORE);
        break;
    case pkm::VariableType::FLOAT:
        op_code = static_cast<uint8_t>(Opcode::FSTORE);
        break;
    case pkm::VariableType::REFERENCE:
        op_code = static_cast<uint8_t>(Opcode::ASTORE);
        break;
    default:
        break;
    }

    instructions->write(reinterpret_cast<char*>(&op_code), 1);
    instructions->write(reinterpret_cast<char*>(&null), 1);
    instructions->write(reinterpret_cast<char*>(&index), sizeof(index));
}

void Translator::writeControl(AST* control_node, std::stringstream* instructions)
{
    switch (static_cast<ast::ControlNode*>(control_node->value().get())->control_type)
    {
    case pkm::ControlType::IF:
        writeIfElifElse(control_node, instructions);
        break;
    case pkm::ControlType::ELSE:
    case pkm::ControlType::ELIF:
        break;
    case pkm::ControlType::WHILE:
        writeWhile(control_node, instructions);
        break;
    }
}

struct Condition
{
    enum Type
    {
        OR,
        AND,
        OBJ,
    };
    Type type = OBJ;
    size_t line = 0;
    AST* ast = nullptr;
};

void makeConditionTree(Tree<Condition>* cond_tree, AST* cond_node)
{
    if ((cond_node->value()->type() == pkm::ASTNodeType::OPERATION) &&
        ((static_cast<ast::OperationNode*>(cond_node->value().get())->op_type == pkm::OperationType::OR) ||
         (static_cast<ast::OperationNode*>(cond_node->value().get())->op_type == pkm::OperationType::AND)))
    {
        bool is_or = (static_cast<ast::OperationNode*>(cond_node->value().get())->op_type == pkm::OperationType::OR);
        Condition::Type cond_node_type = is_or ? Condition::OR : Condition::AND;
    
        if (cond_tree->value().type == Condition::OBJ)
        {
            cond_tree->value().type = cond_node_type;
        }
        else if (cond_tree->value().type != cond_node_type)
        {
            cond_tree->emplace_branch({cond_node_type, 0, nullptr});
            cond_tree = &(*cond_tree)[cond_tree->branches_num() - 1];
        }
        makeConditionTree(cond_tree, static_cast<AST*>(&(*cond_node)[0]));
        makeConditionTree(cond_tree, static_cast<AST*>(&(*cond_node)[1]));
    }
    else
    {
        cond_tree->emplace_branch({Condition::OBJ, 0, cond_node});
    }
}

struct CondCommand
{
    enum Type
    {
        IF,
        IFN,
        GOTO,
        OBJ,
        INST,
    };
    Type type = OBJ;
    size_t jump = 0;
    AST* ast = nullptr;
};

size_t setCommands(std::vector<CondCommand>* command_array, Tree<Condition>* cond_tree)
{
    size_t cmd_num = 0;
    switch (cond_tree->value().type)
    {
    case Condition::OR:
    {
        for (size_t i = 0; i < cond_tree->branches_num(); i++)
        {
            cmd_num += setCommands(command_array, &(*cond_tree)[i]);
            if ((*cond_tree)[i].value().type == Condition::OBJ)
            {
                CondCommand::Type type = (i + 1 == cond_tree->branches_num()) ? CondCommand::IFN : CondCommand::IF;
                command_array->emplace_back(CondCommand{type, 0, nullptr});
                cmd_num++;
            }
        }
        break;
    }
    case Condition::AND:
    {
        for (size_t i = 0; i < cond_tree->branches_num(); i++)
        {
            cmd_num += setCommands(command_array, &(*cond_tree)[i]);
            if ((*cond_tree)[i].value().type == Condition::OBJ)
            {
                command_array->emplace_back(CondCommand{CondCommand::IFN, 0, nullptr});
                cmd_num++;
            }
        }
        break;
    }
    case Condition::OBJ:
    {
        if (cond_tree->branches_num())
        {
            cond_tree = &(*cond_tree)[0];
        }
        cond_tree->value().line = command_array->size();
        command_array->emplace_back(CondCommand{CondCommand::OBJ, 0, cond_tree->value().ast});
        cmd_num++;
        break;
    }
    }
    return cmd_num;
}

size_t getJump(Tree<Condition>* cond_tree)
{
    if (cond_tree->value().type == Condition::OBJ)
    {
        return cond_tree->value().line;
    }
    return getJump(&(*cond_tree)[0]);
}

void setJumps(std::vector<CondCommand>* command_array, Tree<Condition>* cond_tree, size_t jump, CondCommand::Type type)
{
    if (jump)
    {
        for (size_t i = 0; i < cond_tree->branches_num(); i++)
        {
            setJumps(command_array, &(*cond_tree)[i], jump, type);
        }
    }

    if (((type == CondCommand::IFN) && (cond_tree->value().type == Condition::OR)) ||
        ((type == CondCommand::IF) && (cond_tree->value().type == Condition::AND)))
    {
        for (size_t i = cond_tree->branches_num() - 1; i > 0; i--)
        {
            setJumps(command_array, &(*cond_tree)[i - 1], getJump(&(*cond_tree)[i]), type);
        }
    }

    if (cond_tree->value().type == Condition::OBJ)
    {
        size_t line = cond_tree->value().line;
        if ((*command_array)[line + 1].type == type)
        {
            (*command_array)[line + 1].jump = jump;
        }
    }
}

void makeCommandArray(std::vector<CondCommand>* command_array, Tree<Condition>* cond_tree)
{
    size_t cmd_num = setCommands(command_array, cond_tree);
    if (cmd_num == 1)
    {
        command_array->emplace_back(CondCommand{CondCommand::IFN, 0, nullptr});
    }

    if (cond_tree->value().type == Condition::OR)
    {
        setJumps(command_array, cond_tree, 0, CondCommand::IFN);
        for (size_t i = 0; i < cond_tree->branches_num(); i++)
        {
            setJumps(command_array, &(*cond_tree)[i], 0, CondCommand::IF);
        }
    }
    if (cond_tree->value().type == Condition::AND)
    {
        setJumps(command_array, cond_tree, 0, CondCommand::IF);
        for (size_t i = 0; i < cond_tree->branches_num(); i++)
        {
            setJumps(command_array, &(*cond_tree)[i], 0, CondCommand::IFN);
        }
    }
}

using instructions_table = std::unordered_map<CondCommand*, std::stringstream>;

void updateJumpOffsets(instructions_table* inst_table, std::vector<CondCommand>* command_array, size_t initial_offset)
{
    for (auto& command : *command_array)
    {
        if ((command.type == CondCommand::IF) || (command.type == CondCommand::IFN) || (command.type == CondCommand::GOTO))
        {
            size_t offset = initial_offset;
            for (size_t i = 0; i < command.jump; i++)
            {
                switch ((*command_array)[i].type)
                {
                case CondCommand::IF:
                case CondCommand::IFN:
                case CondCommand::GOTO:
                {
                    offset += sizeof(int32_t);
                    break;
                }
                case CondCommand::OBJ:
                case CondCommand::INST:
                {
                    (*inst_table)[&(*command_array)[i]].seekg(0, std::ios::end);
                    offset += static_cast<size_t>((*inst_table)[&(*command_array)[i]].tellg());
                    break;
                }
                }
            }
            command.jump = offset;
        }
    }
}

void Translator::writeCommands(void* array, std::stringstream* instructions)
{
    auto* command_array = reinterpret_cast<std::vector<CondCommand>*>(array);

    instructions->seekg(0, std::ios::end);
    auto initial_offset = static_cast<size_t>(instructions->tellg());

    instructions_table inst_table;
    for (auto& command : *command_array)
    {
        if (command.type == CondCommand::OBJ)
        {
            writeObject(command.ast, &inst_table[&command]);
        }
        if (command.type == CondCommand::INST)
        {
            writeInstructions(command.ast, &inst_table[&command]);
        }
    }
    updateJumpOffsets(&inst_table, command_array, initial_offset);

    for (const auto& command : *command_array)
    {
        uint8_t null = 0;
        auto offset = static_cast<uint16_t>(command.jump);

        switch (command.type)
        {
        case CondCommand::IF:
        case CondCommand::IFN:
        {
            auto op_code = static_cast<uint32_t>(command.type) - static_cast<uint32_t>(CondCommand::IF) +
                static_cast<uint32_t>(Opcode::IF);

            instructions->write(reinterpret_cast<char*>(&op_code), 1);
            instructions->write(reinterpret_cast<char*>(&null), 1);
            instructions->write(reinterpret_cast<char*>(&offset), sizeof(offset));
            break;
        }
        case CondCommand::GOTO:
        {
            auto op_code = static_cast<uint8_t>(Opcode::GOTO);

            instructions->write(reinterpret_cast<char*>(&op_code), 1);
            instructions->write(reinterpret_cast<char*>(&null), 1);
            instructions->write(reinterpret_cast<char*>(&offset), sizeof(offset));
            break;
        }
        case CondCommand::OBJ:
            writeObject(command.ast, instructions);
            break;
        case CondCommand::INST:
            writeInstructions(command.ast, instructions);
            break;
        }
    }
}

void setOffsets(std::vector<CondCommand>* command_array, size_t true_offset, size_t false_offset)
{
    for (auto& command : *command_array)
    {
        if ((command.type == CondCommand::IF) && (command.jump == 0))
        {
            command.jump = true_offset;
        }
        if ((command.type == CondCommand::IFN) && (command.jump == 0))
        {
            command.jump = false_offset;
        }
    }
}

void setOffsets(std::vector<CondCommand>* command_array, size_t goto_offset)
{
    for (auto& command : *command_array)
    {
        if ((command.type == CondCommand::GOTO) && (command.jump == 0))
        {
            command.jump = goto_offset;
        }
    }
}

void makeCommandArrayIfElifElse(std::vector<CondCommand>* command_array, AST* control_node)
{
    auto* cond_node = static_cast<AST*>(&(*control_node)[0]);
    auto* if_scope = static_cast<AST*>(&(*control_node)[1]);
    AST* else_scope = (control_node->branches_num() == 3) ? static_cast<AST*>(&(*control_node)[2]) : nullptr;

    Tree<Condition> cond_tree;
    makeConditionTree(&cond_tree, cond_node);
    makeCommandArray(command_array, &cond_tree);

    size_t true_offset = command_array->size();
    command_array->emplace_back(CondCommand{CondCommand::INST, 0, if_scope});

    if (else_scope)
    {
        setOffsets(command_array, true_offset, true_offset + 2);
        command_array->emplace_back(CondCommand{CondCommand::GOTO, 0, nullptr});

        if (static_cast<ast::ControlNode*>(else_scope->value().get())->control_type == pkm::ControlType::ELIF)
        {
            makeCommandArrayIfElifElse(command_array, else_scope);
        }
        else
        {
            command_array->emplace_back(CondCommand{CondCommand::INST, 0, static_cast<AST*>(&(*else_scope)[0])});
        }
        return;
    }
    setOffsets(command_array, true_offset, true_offset + 1);
}

void Translator::writeIfElifElse(AST* if_node, std::stringstream* instructions)
{
    std::vector<CondCommand> command_array;
    makeCommandArrayIfElifElse(&command_array, if_node);
    setOffsets(&command_array, command_array.size());

    writeCommands(&command_array, instructions);
}

void Translator::writeWhile(AST* while_node, std::stringstream* instructions)
{
    auto* cond_node = static_cast<AST*>(&(*while_node)[0]);
    auto* scope_node = static_cast<AST*>(&(*while_node)[1]);

    Tree<Condition> cond_tree;
    makeConditionTree(&cond_tree, cond_node);

    std::vector<CondCommand> command_array;
    makeCommandArray(&command_array, &cond_tree);

    size_t true_offset = command_array.size();
    command_array.emplace_back(CondCommand{CondCommand::INST, 0, scope_node});

    size_t false_offset = true_offset + 2;
    command_array.emplace_back(CondCommand{CondCommand::GOTO, 0, nullptr});

    setOffsets(&command_array, true_offset, false_offset);
    writeCommands(&command_array, instructions);
}