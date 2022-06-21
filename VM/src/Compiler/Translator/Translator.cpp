#include "Compiler/Translator/Translator.h"
#include "Opcodes.h"

#include <unordered_map>

Translator::Translator(AST* class_tree) : class_tree_(class_tree) {}

void Translator::translate(std::ofstream* output_file)
{
    auto* class_node = class_tree_;
    std::string class_name = static_cast<ClassNode*>(class_node->value().get())->name;
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
        output_file->write(reinterpret_cast<char*>(&type), sizeof(type));
        switch (type)
        {
        case static_cast<uint8_t>(AbstractType::Type::INTEGER):
        {
            auto value = static_cast<IntegerType*>(elem.first)->value;
            output_file->write(reinterpret_cast<char*>(&value), sizeof(value));
            break;
        }
        case static_cast<uint8_t>(AbstractType::Type::FLOAT):
        {
            auto value = static_cast<FloatType*>(elem.first)->value;
            output_file->write(reinterpret_cast<char*>(&value), sizeof(value));
            break;
        }
        case static_cast<uint8_t>(AbstractType::Type::STRING):
        {
            auto value = static_cast<StringType*>(elem.first)->value;
            output_file->write(reinterpret_cast<const char*>(value.c_str()), static_cast<std::streamsize>(value.length() + 1));
            break;
        }
        case static_cast<uint8_t>(AbstractType::Type::FUNCTION):
        {
            auto value = static_cast<FunctionType*>(elem.first)->value;
            output_file->write(reinterpret_cast<const char*>(value.c_str()), static_cast<std::streamsize>(value.length() + 1));
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
        if ((*class_node)[i].value()->type() == ASTNodeType::FIELD)
        {
            fields_num++;
        }
    }
    class_content->write(reinterpret_cast<char*>(&fields_num), sizeof(fields_num));

    for (size_t i = 0; i < class_node->branches_num(); i++)
    {
        if ((*class_node)[i].value()->type() == ASTNodeType::FIELD)
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
        if ((*class_node)[i].value()->type() == ASTNodeType::METHOD)
        {
            methods_num++;
        }
    }
    class_content->write(reinterpret_cast<char*>(&methods_num), sizeof(methods_num));

    for (size_t i = 0; i < class_node->branches_num(); i++)
    {
        if ((*class_node)[i].value()->type() == ASTNodeType::METHOD)
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

            auto locals_num = static_cast<uint16_t>(locals_.size());
            class_content->write(reinterpret_cast<char*>(&locals_num), sizeof(locals_num));

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
        if ((*method_node)[i].value()->type() == ASTNodeType::MET_PAR)
        {
            mps_num++;
        }
    }
    class_content->write(reinterpret_cast<char*>(&mps_num), sizeof(mps_num));

    for (size_t i = 0; i < method_node->branches_num(); i++)
    {
        if ((*method_node)[i].value()->type() == ASTNodeType::MET_PAR)
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
        switch ((*scope_node)[i].value()->type())
        {
        case ASTNodeType::SCOPE:
            writeInstructions(static_cast<AST*>(&(*scope_node)[i]), instructions);
            break;
        case ASTNodeType::OPERATION:
            writeOperation(static_cast<AST*>(&(*scope_node)[i]), instructions);
            break;
        case ASTNodeType::CONTROL:
            writeControl(static_cast<AST*>(&(*scope_node)[i]), instructions);
            break;
        case ASTNodeType::FUNCTION:
            writeFunction(static_cast<AST*>(&(*scope_node)[i]), instructions);
            break;
        case ASTNodeType::VAR_DECL:
            appendLocal(static_cast<VariableDeclarationNode*>((*scope_node)[i].value().get()));
            break;
        case ASTNodeType::VARIABLE:
            writeLoad(static_cast<VariableNode*>((*scope_node)[i].value().get())->name, instructions);
            break;
        case ASTNodeType::NUMBER:
            writeNumber(static_cast<NumberNode*>((*scope_node)[i].value().get()), instructions);
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
    case ASTNodeType::OPERATION:
        return writeOperation(obj_node, instructions);
    case ASTNodeType::FUNCTION:
        return writeFunction(obj_node, instructions);
    case ASTNodeType::VARIABLE:
        return writeLoad(static_cast<VariableNode*>(obj_node->value().get())->name, instructions);
    case ASTNodeType::NUMBER:
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
    case OperationType::RETURN:
    {
        uint32_t null = 0;
        uint8_t op_code = 0;
        VariableType ret_type = VariableType::VOID;
        if (op_node->branches_num() > 0)
        {
            ret_type = writeObject(static_cast<AST*>(&(*op_node)[0]), instructions);

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
        }
        else
        {
            op_code = static_cast<uint8_t>(Opcode::RETURN);
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
        case ASTNodeType::VAR_DECL:
        {
            auto* var_decl_node = static_cast<VariableDeclarationNode*>(lhs->value().get());
            appendLocal(var_decl_node);
            writeStore(var_decl_node->name, instructions);
            return ret_type;
        }
        case ASTNodeType::VARIABLE:
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
    case OperationType::OR:
    case OperationType::AND:
    {
        AST ast(std::make_shared<ControlNode>(ControlNode(ControlType::IF)));
        ast.emplace_branch(std::move(*op_node));
        ast.emplace_branch(std::make_shared<ScopeNode>(ScopeNode()));
        ast[1].emplace_branch(std::make_shared<NumberNode>(NumberNode(1)));
        ast.emplace_branch(std::make_shared<ControlNode>(ControlNode(ControlType::ELSE)));
        ast[2].emplace_branch(std::make_shared<ScopeNode>(ScopeNode()));
        ast[2][0].emplace_branch(std::make_shared<NumberNode>(NumberNode(0)));

        writeControl(&ast, instructions);
        return VariableType::INT;
    }
    case OperationType::EQ:
    case OperationType::NEQ:
    case OperationType::LEQ:
    case OperationType::GEQ:
    case OperationType::STL:
    case OperationType::STG:
    {
        auto* lhs = static_cast<AST*>(&(*op_node)[0]);
        auto* rhs = static_cast<AST*>(&(*op_node)[1]);
        VariableType ret_type = writeObject(rhs, instructions);
        writeObject(lhs, instructions);

        auto op_type = static_cast<uint32_t>(static_cast<OperationNode*>(op_node->value().get())->op_type);
        op_type -= static_cast<uint32_t>(OperationType::EQ);

        uint32_t null = 0;
        uint32_t op_code = 0;
        switch (ret_type)
        {
        case VariableType::INT:
            op_code = static_cast<uint8_t>(Opcode::IEQ) + op_type;
            break;
        case VariableType::LONG:
            op_code = static_cast<uint8_t>(Opcode::LEQ) + op_type;
            break;
        case VariableType::FLOAT:
            op_code = static_cast<uint8_t>(Opcode::FEQ) + op_type;
            break;
        case VariableType::DOUBLE:
            op_code = static_cast<uint8_t>(Opcode::DADD) + op_type;
            break;
        default:
            break;
        }

        instructions->write(reinterpret_cast<char*>(&op_code), 1);
        instructions->write(reinterpret_cast<char*>(&null), 3);
        return VariableType::INT;
    }
    case OperationType::SHL:
    case OperationType::SHR:
    {
        auto* lhs = static_cast<AST*>(&(*op_node)[0]);
        auto* rhs = static_cast<AST*>(&(*op_node)[1]);
        VariableType ret_type = writeObject(rhs, instructions);
        writeObject(lhs, instructions);

        auto var_type = static_cast<uint32_t>(ret_type);
        var_type -= static_cast<uint32_t>(VariableType::INT);

        uint32_t null = 0;
        uint32_t op_code = 0;
        switch (static_cast<OperationNode*>(op_node->value().get())->op_type)
        {
        case OperationType::SHL:
            op_code = static_cast<uint8_t>(Opcode::ISHL) + var_type;
            break;
        case OperationType::SHR:
            op_code = static_cast<uint8_t>(Opcode::ISHR) + var_type;
            break;
        default:
            break;
        }

        instructions->write(reinterpret_cast<char*>(&op_code), 1);
        instructions->write(reinterpret_cast<char*>(&null), 3);
        return VariableType::INT;
    }
    case OperationType::ADD:
    case OperationType::SUB:
    case OperationType::MUL:
    case OperationType::DIV:
    {
        if (op_node->branches_num() == 1)
        {
            auto* arg = static_cast<AST*>(&(*op_node)[0]);
            VariableType ret_type = writeObject(arg, instructions);
            auto op_type = static_cast<uint32_t>(static_cast<OperationNode*>(op_node->value().get())->op_type);
            if (op_type == static_cast<uint32_t>(OperationType::SUB))
            {
                uint32_t null = 0;
                auto op_code = static_cast<uint32_t>(Opcode::INEG) + static_cast<uint32_t>(ret_type) -
                    static_cast<uint32_t>(VariableType::INT);

                instructions->write(reinterpret_cast<char*>(&op_code), 1);
                instructions->write(reinterpret_cast<char*>(&null), 3);
            }
            return ret_type;
        }

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
    case OperationType::REM:
    {
        auto* lhs = static_cast<AST*>(&(*op_node)[0]);
        auto* rhs = static_cast<AST*>(&(*op_node)[1]);
        VariableType ret_type = writeObject(rhs, instructions);
        writeObject(lhs, instructions);

        uint32_t null = 0;
        uint32_t op_code = 0;
        switch (ret_type)
        {
        case VariableType::INT:
            op_code = static_cast<uint8_t>(Opcode::IREM);
            break;
        case VariableType::LONG:
            op_code = static_cast<uint8_t>(Opcode::LREM);
            break;
        case VariableType::FLOAT:
            op_code = static_cast<uint8_t>(Opcode::FREM);
            break;
        case VariableType::DOUBLE:
            op_code = static_cast<uint8_t>(Opcode::DREM);
            break;
        default:
            break;
        }

        instructions->write(reinterpret_cast<char*>(&op_code), 1);
        instructions->write(reinterpret_cast<char*>(&null), 3);
        return ret_type;
    }
    case OperationType::NOT:
    {
        AST ast(std::make_shared<ControlNode>(ControlNode(ControlType::IF)));
        ast.emplace_branch(std::move(*static_cast<AST*>(&(*op_node)[0])));
        ast.emplace_branch(std::make_shared<ScopeNode>(ScopeNode()));
        ast[1].emplace_branch(std::make_shared<NumberNode>(NumberNode(0)));
        ast.emplace_branch(std::make_shared<ControlNode>(ControlNode(ControlType::ELSE)));
        ast[2].emplace_branch(std::make_shared<ScopeNode>(ScopeNode()));
        ast[2][0].emplace_branch(std::make_shared<NumberNode>(NumberNode(1)));

        writeControl(&ast, instructions);
        return VariableType::INT;
    }
    case OperationType::SQR_BR:
    case OperationType::NEW:
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

    if (!const_pool_.contains(std::make_unique<FunctionType>(FunctionType(func_name))))
    {
        const_pool_[std::make_unique<FunctionType>(FunctionType(func_name))] = cp_size;
    }
    else
    {
        cp_size = const_pool_[std::make_unique<FunctionType>(FunctionType(func_name))];
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

void Translator::writeControl(AST* control_node, std::stringstream* instructions)
{
    switch (static_cast<ControlNode*>(control_node->value().get())->control_type)
    {
    case ControlType::IF:
        writeIfElifElse(control_node, instructions);
        break;
    case ControlType::ELSE:
    case ControlType::ELIF:
        break;
    case ControlType::WHILE:
        writeWhile(control_node, instructions);
        break;
    default:
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
    if ((cond_node->value()->type() == ASTNodeType::OPERATION) &&
        ((static_cast<OperationNode*>(cond_node->value().get())->op_type == OperationType::OR) ||
         (static_cast<OperationNode*>(cond_node->value().get())->op_type == OperationType::AND)))
    {
        bool is_or = (static_cast<OperationNode*>(cond_node->value().get())->op_type == OperationType::OR);
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
    default:
        break;
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
                default:
                    break;
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
        default:
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

        if (static_cast<ControlNode*>(else_scope->value().get())->control_type == ControlType::ELIF)
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