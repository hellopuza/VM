#include "Compiler/Analyzer/Analyzer.h"
#include "Compiler/AST/ASTMaker.h"

#include <filesystem>

using ast::AST;

#define CHECK_ERROR(cond, err, node, action)         \
    if (cond) {                                      \
        error_.type = CompilationError::Type::err;   \
        error_.location = (node)->value()->location; \
        action;                                      \
    } //

#define RETURN_ERROR(action)                         \
    if (error_.type != CompilationError::Type::OK) { \
        action;                                      \
    } //

Analyzer::Analyzer()
{
    for (const auto & name : pkm::strings::RESERVED_NAMES)
    {
        reserved_names_.emplace(name);
    }
}

Analyzer::Analyzer(const char* lib_folder)
{
    AST lib_ast;
    for (const auto& entry : std::filesystem::directory_iterator(lib_folder))
    {
        std::ifstream file(entry.path().string());
        ASTMaker ast_maker(&file);
        ast_maker.make(&lib_ast);
    }

    for (size_t i = 0; i < lib_ast.branches_num(); i++)
    {
        loadClass(static_cast<AST*>(&lib_ast[i]));
    }

    for (const auto & name : pkm::strings::RESERVED_NAMES)
    {
        reserved_names_.emplace(name);
    }
}

void Analyzer::analyze(AST* class_tree)
{
    class_name_ = static_cast<ast::ClassNode*>(class_tree->value().get())->name;
    for (size_t i = 0; i < class_tree->branches_num(); i++)
    {
        auto* node = static_cast<AST*>(&(*class_tree)[i]);
        if (node->value()->type() == pkm::ASTNodeType::METHOD)
        {
            analyzeMethod(node);
            RETURN_ERROR(return);
        }
        else
        {
            analyzeField(node);
            RETURN_ERROR(return);
        }
    }
}

void Analyzer::loadClass(const AST* class_tree)
{
    std::string class_name = static_cast<ast::ClassNode*>(class_tree->value().get())->name;

    CHECK_ERROR((reserved_names_.contains(class_name)), RESERVED_NAME, class_tree, return);
    CHECK_ERROR((classes_.contains(class_name)), OVERRIDING_CLASS, class_tree, return);

    for (size_t i = 0; i < class_tree->branches_num(); i++)
    {
        const auto* node = static_cast<const AST*>(&(*class_tree)[i]);
        if (node->value()->type() == pkm::ASTNodeType::FIELD)
        {
            auto* field_node = static_cast<ast::FieldNode*>(node->value().get());

            CHECK_ERROR((reserved_names_.contains(field_node->name)), RESERVED_NAME, node, return);
            CHECK_ERROR((classes_[class_name].fields.contains(field_node->name)), OVERRIDING_FIELD, node, return);

            classes_[class_name].fields[field_node->name].access_type = field_node->access_type;
            classes_[class_name].fields[field_node->name].var_type = field_node->var_type;
        }
        else
        {
            auto* method_node = static_cast<ast::MethodNode*>(node->value().get());

            CHECK_ERROR((reserved_names_.contains(method_node->name)), RESERVED_NAME, node, return);
            CHECK_ERROR((classes_[class_name].methods.contains(method_node->name)), OVERRIDING_METHOD, node, return);

            classes_[class_name].methods[method_node->name].access_type = method_node->access_type;
            classes_[class_name].methods[method_node->name].modifier = method_node->modifier;
            classes_[class_name].methods[method_node->name].ret_type = method_node->ret_type;

            for (size_t j = 0; j < node->branches_num() - 1; j++)
            {
                const auto* mnode = static_cast<const AST*>(&(*node)[j]);
                auto* par_node = static_cast<ast::MethodParameterNode*>(mnode->value().get());
                classes_[class_name].methods[method_node->name].met_params.push_back(par_node->var_type);
            }
        }
    }
}

CompilationError Analyzer::getError() const
{
    return error_;
}

void Analyzer::analyzeField(AST* field_node)
{
    pkm::DataType field_type = static_cast<ast::FieldNode*>(field_node->value().get())->var_type;
    CHECK_ERROR(((field_type.type == pkm::VariableType::REFERENCE) && !classes_.contains(field_type.name)),
        UNIDENTIFIED_FIELD_TYPE, field_node, return);
}

void setLastReturn(AST* scope_node)
{
    bool was_return = false;
    for (size_t i = 0; i < scope_node->branches_num(); i++)
    {
        auto* node = static_cast<AST*>(&(*scope_node)[i]);
        if (node->value()->type() == pkm::ASTNodeType::OPERATION)
        {
            auto* op_node = static_cast<ast::OperationNode*>(node->value().get());
            if (op_node->op_type == pkm::OperationType::RETURN)
            {
                was_return = true;
            }
        }
    }

    if (!was_return)
    {
        scope_node->emplace_branch(std::make_shared<ast::OperationNode>(ast::OperationNode(pkm::OperationType::RETURN)));
    }
}

void Analyzer::analyzeMethod(AST* met_node)
{
    pkm::DataType met_ret_type = static_cast<ast::MethodNode*>(met_node->value().get())->ret_type;
    CHECK_ERROR(((met_ret_type.type == pkm::VariableType::REFERENCE) && !classes_.contains(met_ret_type.name)),
        UNIDENTIFIED_METHOD_RETURN_TYPE, met_node, return);

    locals_.clear();
    method_name_ = static_cast<ast::MethodNode*>(met_node->value().get())->name;

    for (size_t i = 0; i < met_node->branches_num() - 1; i++)
    {
        auto* node = static_cast<AST*>(&(*met_node)[i]);
        auto* mp_node = static_cast<ast::MethodParameterNode*>(node->value().get());
        pkm::DataType mp_type = mp_node->var_type;

        CHECK_ERROR(((mp_type.type == pkm::VariableType::REFERENCE) && !classes_.contains(mp_type.name)),
            UNIDENTIFIED_METHOD_PARAMETER_TYPE, node, return);

        CHECK_ERROR((reserved_names_.contains(mp_node->name)), RESERVED_NAME, node, return);
        CHECK_ERROR((locals_.contains(mp_node->name)), EXPECTED_METHOD_PARAMETER_NAME, node, return);

        locals_[mp_node->name] = mp_node->var_type;
    }

    auto* scope_node = static_cast<AST*>(&(*met_node)[met_node->branches_num() - 1]);
    analyzeScope(scope_node);
    RETURN_ERROR(return);

    setLastReturn(scope_node);
}

void Analyzer::analyzeScope(AST* scope_node)
{
    for (size_t i = 0; i < scope_node->branches_num(); i++)
    {
        auto* node = static_cast<AST*>(&(*scope_node)[i]);
        switch (node->value()->type())
        {
        case pkm::ASTNodeType::SCOPE:
            analyzeScope(node);
            break;
        case pkm::ASTNodeType::CONTROL:
            analyzeControl(node);
            break;
        case pkm::ASTNodeType::VAR_DECL:
            analyzeVariableDeclaration(node);
            break;
        case pkm::ASTNodeType::OPERATION:
            analyzeOperation(node);
            break;
        case pkm::ASTNodeType::FUNCTION:
            analyzeFunction(node);
            break;
        case pkm::ASTNodeType::VARIABLE:
            analyzeVariable(node);
            break;
        case pkm::ASTNodeType::NUMBER:
        case pkm::ASTNodeType::STRING:
        case pkm::ASTNodeType::SYMBOL:
        case pkm::ASTNodeType::TYPE:
        default:
            break;
        }
        RETURN_ERROR(return);
    }
}

void Analyzer::analyzeControl(AST* control_node)
{
    switch (static_cast<ast::ControlNode*>(control_node->value().get())->control_type)
    {
    case pkm::ControlType::IF:
    case pkm::ControlType::ELIF:
    case pkm::ControlType::WHILE:
    {
        auto* cond_node = static_cast<AST*>(&(*control_node)[0]);
        pkm::DataType cond_type = analyzeObject(cond_node);
        RETURN_ERROR(return);

        CHECK_ERROR((cond_type.type == pkm::VariableType::REFERENCE), INCORRECT_CONDITION_TYPE, cond_node, return);

        if (cond_type.type != pkm::VariableType::INT)
        {
            AST conv_node(std::make_shared<ast::ConversionNode>(ast::ConversionNode(cond_type, pkm::DataType(pkm::VariableType::INT))));
            conv_node.emplace_branch(std::move(*cond_node));
            *cond_node = conv_node;
        }

        analyzeScope(static_cast<AST*>(&(*control_node)[1]));
        RETURN_ERROR(return);

        if (control_node->branches_num() > 2)
        {
            analyzeControl(static_cast<AST*>(&(*control_node)[2]));
            RETURN_ERROR(return);
        }
        break;
    }
    case pkm::ControlType::ELSE:
    {
        analyzeScope(static_cast<AST*>(&(*control_node)[0]));
        RETURN_ERROR(return);
        break;
    }
    }
}

pkm::DataType Analyzer::analyzeObject(AST* obj_node)
{
    switch (obj_node->value()->type())
    {
    case pkm::ASTNodeType::OPERATION:
        return analyzeOperation(obj_node);
    case pkm::ASTNodeType::FUNCTION:
        return analyzeFunction(obj_node);
    case pkm::ASTNodeType::VARIABLE:
        return analyzeVariable(obj_node);
    case pkm::ASTNodeType::NUMBER:
        return pkm::DataType(static_cast<ast::NumberNode*>(obj_node->value().get())->num_type);
    case pkm::ASTNodeType::STRING:
        return pkm::DataType(pkm::VariableType::REFERENCE, "String");
    case pkm::ASTNodeType::SYMBOL:
        return pkm::DataType(pkm::VariableType::CHAR);
    default:
        CHECK_ERROR(true, INCORRECT_OPERAND, obj_node, break);
    }
    return pkm::DataType();
}

pkm::DataType Analyzer::analyzeOperation(AST* op_node)
{
    switch (static_cast<ast::OperationNode*>(op_node->value().get())->op_type)
    {
    case pkm::OperationType::RETURN:
    {
        pkm::DataType ret_type;
        if (op_node->branches_num() > 0)
        {
            ret_type = analyzeObject(static_cast<AST*>(&(*op_node)[0]));
            RETURN_ERROR(return ret_type);
        }

        pkm::DataType met_ret_type = classes_[class_name_].methods[method_name_].ret_type;
        if ((ret_type.type == pkm::VariableType::REFERENCE) || (met_ret_type.type == pkm::VariableType::REFERENCE) ||
            (ret_type.type == pkm::VariableType::VOID))
        {
            CHECK_ERROR((ret_type.name != met_ret_type.name), INCORRECT_RETURN_CONVERSION_TYPE, op_node, return ret_type);
        }

        if (ret_type.name != met_ret_type.name)
        {
            AST conv_node(std::make_shared<ast::ConversionNode>(ast::ConversionNode(ret_type, met_ret_type)));
            conv_node.emplace_branch(std::move((*op_node)[0]));
            (*op_node)[0] = conv_node;
        }
        static_cast<ast::OperationNode*>(op_node->value().get())->var_type = ret_type;

        return ret_type;
    }
    case pkm::OperationType::ASSIGN:
    {
        auto* lhs = &(*op_node)[0];
        auto* rhs = &(*op_node)[1];

        pkm::DataType rhs_type = analyzeObject(static_cast<AST*>(rhs));
        pkm::DataType lhs_type;
        RETURN_ERROR(return pkm::DataType());

        switch (lhs->value()->type())
        {
        case pkm::ASTNodeType::VAR_DECL:
        {
            lhs_type = analyzeVariableDeclaration(static_cast<AST*>(lhs));
            break;
        }
        case pkm::ASTNodeType::VARIABLE:
        {
            lhs_type = analyzeVariable(static_cast<AST*>(lhs));
            break;
        }
        default:
            CHECK_ERROR(1, INCORRECT_ASSIGNMENT_LHS, op_node, return pkm::DataType());
        }
        RETURN_ERROR(return pkm::DataType());

        if ((lhs_type.type == pkm::VariableType::REFERENCE) || (rhs_type.type == pkm::VariableType::REFERENCE))
        {
            CHECK_ERROR((lhs_type.name != rhs_type.name), INCORRECT_ASSIGNMENT_TYPE, op_node, return pkm::DataType());
        }

        if (lhs_type.name != rhs_type.name)
        {
            AST conv_node(std::make_shared<ast::ConversionNode>(ast::ConversionNode(rhs_type, lhs_type)));
            conv_node.emplace_branch(std::move(*rhs));
            *rhs = conv_node;
        }
        static_cast<ast::OperationNode*>(op_node->value().get())->var_type = lhs_type;

        return lhs_type;
    }
    case pkm::OperationType::OR:
    case pkm::OperationType::AND:
    {
        auto* lhs = &(*op_node)[0];
        auto* rhs = &(*op_node)[1];

        pkm::DataType lhs_type = analyzeObject(static_cast<AST*>(lhs));
        RETURN_ERROR(return pkm::DataType());

        pkm::DataType rhs_type = analyzeObject(static_cast<AST*>(rhs));
        RETURN_ERROR(return pkm::DataType());

        CHECK_ERROR((lhs_type.type == pkm::VariableType::REFERENCE), INCORRECT_OPERAND_TYPE, lhs, return pkm::DataType());
        CHECK_ERROR((rhs_type.type == pkm::VariableType::REFERENCE), INCORRECT_OPERAND_TYPE, rhs, return pkm::DataType());

        if (lhs_type.type != pkm::VariableType::INT)
        {
            AST conv_node(std::make_shared<ast::ConversionNode>(ast::ConversionNode(lhs_type, pkm::DataType(pkm::VariableType::INT))));
            conv_node.emplace_branch(std::move(*lhs));
            *lhs = conv_node;
        }
        if (rhs_type.type != pkm::VariableType::INT)
        {
            AST conv_node(std::make_shared<ast::ConversionNode>(ast::ConversionNode(rhs_type, pkm::DataType(pkm::VariableType::INT))));
            conv_node.emplace_branch(std::move(*rhs));
            *rhs = conv_node;
        }
        static_cast<ast::OperationNode*>(op_node->value().get())->var_type = pkm::DataType(pkm::VariableType::INT);

        return pkm::DataType(pkm::VariableType::INT);
    }
    case pkm::OperationType::EQ:
    case pkm::OperationType::NEQ:
    case pkm::OperationType::LEQ:
    case pkm::OperationType::GEQ:
    case pkm::OperationType::STL:
    case pkm::OperationType::STG:
    {
        auto* lhs = &(*op_node)[0];
        auto* rhs = &(*op_node)[1];

        pkm::DataType lhs_type = analyzeObject(static_cast<AST*>(lhs));
        RETURN_ERROR(return pkm::DataType());

        pkm::DataType rhs_type = analyzeObject(static_cast<AST*>(rhs));
        RETURN_ERROR(return pkm::DataType());

        CHECK_ERROR((lhs_type.type == pkm::VariableType::REFERENCE), INCORRECT_OPERAND_TYPE, lhs, return pkm::DataType());
        CHECK_ERROR((rhs_type.type == pkm::VariableType::REFERENCE), INCORRECT_OPERAND_TYPE, rhs, return pkm::DataType());

        pkm::DataType op_type(pkm::VariableType::INT);
        if (lhs_type.name != rhs_type.name)
        {
            if ((lhs_type.type != pkm::VariableType::FLOAT) && (rhs_type.type == pkm::VariableType::FLOAT))
            {
                AST conv_node(std::make_shared<ast::ConversionNode>(ast::ConversionNode(lhs_type, rhs_type)));
                conv_node.emplace_branch(std::move(*lhs));
                *lhs = conv_node;
                op_type = pkm::DataType(pkm::VariableType::FLOAT);
            }
            if ((rhs_type.type != pkm::VariableType::FLOAT) && (lhs_type.type == pkm::VariableType::FLOAT))
            {
                AST conv_node(std::make_shared<ast::ConversionNode>(ast::ConversionNode(rhs_type, lhs_type)));
                conv_node.emplace_branch(std::move(*rhs));
                *rhs = conv_node;
                op_type = pkm::DataType(pkm::VariableType::FLOAT);
            }
        }
        if (lhs_type.type == pkm::VariableType::FLOAT)
        {
            op_type = pkm::DataType(pkm::VariableType::FLOAT);
        }
        static_cast<ast::OperationNode*>(op_node->value().get())->var_type = op_type;

        return pkm::DataType(pkm::VariableType::INT);
    }
    case pkm::OperationType::SHL:
    case pkm::OperationType::SHR:
    {
        auto* lhs = &(*op_node)[0];
        auto* rhs = &(*op_node)[1];

        pkm::DataType lhs_type = analyzeObject(static_cast<AST*>(lhs));
        RETURN_ERROR(return pkm::DataType());

        pkm::DataType rhs_type = analyzeObject(static_cast<AST*>(rhs));
        RETURN_ERROR(return pkm::DataType());

        CHECK_ERROR(((lhs_type.type != pkm::VariableType::INT) && (lhs_type.type != pkm::VariableType::CHAR)),
            INCORRECT_OPERAND_TYPE, lhs, return pkm::DataType());
        
        CHECK_ERROR(((rhs_type.type != pkm::VariableType::INT) && (rhs_type.type != pkm::VariableType::CHAR)),
            INCORRECT_OPERAND_TYPE, rhs, return pkm::DataType());

        static_cast<ast::OperationNode*>(op_node->value().get())->var_type = pkm::DataType(pkm::VariableType::INT);

        return pkm::DataType(pkm::VariableType::INT);
    }
    case pkm::OperationType::ADD:
    case pkm::OperationType::SUB:
    case pkm::OperationType::MUL:
    case pkm::OperationType::DIV:
    case pkm::OperationType::REM:
    {
        if (op_node->branches_num() == 1)
        {
            auto* rhs = &(*op_node)[0];
            pkm::DataType rhs_type = analyzeObject(static_cast<AST*>(rhs));
            RETURN_ERROR(return pkm::DataType());

            CHECK_ERROR((rhs_type.type == pkm::VariableType::REFERENCE), INCORRECT_OPERAND_TYPE, rhs, return pkm::DataType());
            static_cast<ast::OperationNode*>(op_node->value().get())->var_type = rhs_type;

            return rhs_type;
        }

        auto* lhs = &(*op_node)[0];
        auto* rhs = &(*op_node)[1];

        pkm::DataType lhs_type = analyzeObject(static_cast<AST*>(lhs));
        RETURN_ERROR(return pkm::DataType());

        pkm::DataType rhs_type = analyzeObject(static_cast<AST*>(rhs));
        RETURN_ERROR(return pkm::DataType());

        CHECK_ERROR((lhs_type.type == pkm::VariableType::REFERENCE), INCORRECT_OPERAND_TYPE, lhs, return pkm::DataType());
        CHECK_ERROR((rhs_type.type == pkm::VariableType::REFERENCE), INCORRECT_OPERAND_TYPE, rhs, return pkm::DataType());

        pkm::DataType ret_type(pkm::VariableType::INT);
        if (lhs_type.name != rhs_type.name)
        {
            if ((lhs_type.type != pkm::VariableType::FLOAT) && (rhs_type.type == pkm::VariableType::FLOAT))
            {
                ret_type = pkm::DataType(pkm::VariableType::FLOAT);
                AST conv_node(std::make_shared<ast::ConversionNode>(ast::ConversionNode(lhs_type, rhs_type)));
                conv_node.emplace_branch(std::move(*lhs));
                *lhs = conv_node;
            }
            if ((rhs_type.type != pkm::VariableType::FLOAT) && (lhs_type.type == pkm::VariableType::FLOAT))
            {
                ret_type = pkm::DataType(pkm::VariableType::FLOAT);
                AST conv_node(std::make_shared<ast::ConversionNode>(ast::ConversionNode(rhs_type, lhs_type)));
                conv_node.emplace_branch(std::move(*rhs));
                *rhs = conv_node;
            }
        }
        if (lhs_type.type == pkm::VariableType::FLOAT)
        {
            ret_type = pkm::DataType(pkm::VariableType::FLOAT);
        }
        static_cast<ast::OperationNode*>(op_node->value().get())->var_type = ret_type;

        return ret_type;
    }
    case pkm::OperationType::NOT:
    {
        auto* rhs = &(*op_node)[0];
        pkm::DataType rhs_type = analyzeObject(static_cast<AST*>(rhs));
        RETURN_ERROR(return pkm::DataType());

        CHECK_ERROR((rhs_type.type == pkm::VariableType::REFERENCE), INCORRECT_OPERAND_TYPE, rhs, return pkm::DataType());

        if (rhs_type.type != pkm::VariableType::INT)
        {
            AST conv_node(std::make_shared<ast::ConversionNode>(ast::ConversionNode(rhs_type, pkm::DataType(pkm::VariableType::INT))));
            conv_node.emplace_branch(std::move(*rhs));
            *rhs = conv_node;
        }
        static_cast<ast::OperationNode*>(op_node->value().get())->var_type = pkm::DataType(pkm::VariableType::INT);

        return pkm::DataType(pkm::VariableType::INT);
    }
    case pkm::OperationType::SQR_BR:
        break;
    case pkm::OperationType::NEW:
    {
        auto* rhs = &(*op_node)[0];
        pkm::DataType data_type = static_cast<ast::TypeNode*>(rhs->value().get())->data_type;
        CHECK_ERROR(((data_type.type == pkm::VariableType::REFERENCE) && (!classes_.contains(data_type.name))),
            UNIDENTIFIED_TYPE, rhs, return pkm::DataType());

        static_cast<ast::OperationNode*>(op_node->value().get())->var_type = data_type;

        return data_type;
    }
    }
    return pkm::DataType();
}

void split(const std::string& str, std::vector<std::string>* tokens)
{
    size_t prev_pos = 0;
    size_t dot_pos = 0;
    do {
        dot_pos = str.find('.', prev_pos);
        tokens->push_back(str.substr(prev_pos, dot_pos - prev_pos));
        prev_pos = dot_pos + 1;
    } while (dot_pos != std::string::npos);
}

pkm::DataType Analyzer::analyzeFunction(AST* func_node)
{
    auto* function_node = static_cast<ast::FunctionNode*>(func_node->value().get());
    std::string func_name = function_node->name;

    std::vector<std::string> tokens;
    split(func_name, &tokens);

    pkm::DataType class_type;
    if (tokens.size() == 1)
    {
        CHECK_ERROR((!classes_[class_name_].methods.contains(tokens[0])), UNIDENTIFIED_FUNCTION, func_node, return pkm::DataType());
        class_type = pkm::DataType(pkm::VariableType::REFERENCE, class_name_);

        CHECK_ERROR(((classes_[class_name_].methods[method_name_].modifier == pkm::MethodType::STATIC) &&
            (classes_[class_name_].methods[tokens[0]].modifier == pkm::MethodType::INSTANCE)),
            INSTANCE_INSIDE_STATIC_INVOCATION, func_node, return pkm::DataType());

        function_node->modifier = classes_[class_name_].methods[tokens[0]].modifier;

        std::string first_token = (function_node->modifier == pkm::MethodType::INSTANCE) ? "this" : class_name_;
        function_node->name_parts.emplace_back(std::make_pair(first_token, class_type));
        function_node->name_parts.emplace_back(std::make_pair(tokens[0], pkm::DataType()));
    }
    else if (classes_.contains(tokens[0]))
    {
        CHECK_ERROR((tokens.size() != 2), REQUIRED_CLASS_INSTANCE_FIELD_ACCESS, func_node, return pkm::DataType());
        class_type = pkm::DataType(pkm::VariableType::REFERENCE, tokens[0]);

        CHECK_ERROR((!classes_[class_type.name].methods.contains(tokens.back())),
            UNIDENTIFIED_METHOD, func_node, return pkm::DataType());

        CHECK_ERROR((classes_[class_type.name].methods[tokens.back()].access_type == pkm::AccessType::PRIVATE),
            PRIVATE_METHOD, func_node, return pkm::DataType());

        CHECK_ERROR((classes_[class_type.name].methods[tokens.back()].modifier == pkm::MethodType::INSTANCE),
            REQUIRED_CLASS_INSTANCE_METHOD_ACCESS, func_node, return pkm::DataType());

        function_node->modifier = classes_[class_type.name].methods[tokens.back()].modifier;
        function_node->name_parts.emplace_back(std::make_pair(class_type.name, class_type));
        function_node->name_parts.emplace_back(std::make_pair(tokens.back(), pkm::DataType()));
    }
    else
    {
        CHECK_ERROR(((classes_[class_name_].methods[method_name_].modifier == pkm::MethodType::STATIC) &&
            (!locals_.contains(tokens[0]))), UNIDENTIFIED_VARIABLE, func_node, return pkm::DataType());

        CHECK_ERROR(((classes_[class_name_].methods[method_name_].modifier == pkm::MethodType::INSTANCE) &&
            (!locals_.contains(tokens[0])) && (!classes_[class_name_].fields.contains(tokens[0]))),
            UNIDENTIFIED_VARIABLE, func_node, return pkm::DataType());

        if (locals_.contains(tokens[0]))
        {
            class_type = locals_[tokens[0]];
        }
        else
        {
            class_type = classes_[class_name_].fields[tokens[0]].var_type;
        }
        CHECK_ERROR((class_type.type != pkm::VariableType::REFERENCE), PRIMITIVE_TYPE_FIELD, func_node, return pkm::DataType());
        function_node->name_parts.emplace_back(std::make_pair(tokens[0], class_type));

        for (size_t i = 1; i < tokens.size() - 1; i++)
        {
            CHECK_ERROR((!classes_.contains(class_type.name)), UNIDENTIFIED_VARIABLE_TYPE, func_node, return pkm::DataType());
            CHECK_ERROR((!classes_[class_type.name].fields.contains(tokens[i])), UNIDENTIFIED_FIELD, func_node, return pkm::DataType());
            CHECK_ERROR((classes_[class_type.name].fields[tokens[i]].access_type == pkm::AccessType::PRIVATE),
                PRIVATE_FIELD, func_node, return pkm::DataType());

            class_type = classes_[class_type.name].fields[tokens[i]].var_type;
            function_node->name_parts.emplace_back(std::make_pair(tokens[i], class_type));
        }

        CHECK_ERROR((!classes_[class_type.name].methods.contains(tokens.back())),
            UNIDENTIFIED_METHOD, func_node, return pkm::DataType());

        CHECK_ERROR((classes_[class_type.name].methods[tokens.back()].access_type == pkm::AccessType::PRIVATE),
            PRIVATE_METHOD, func_node, return pkm::DataType());

        CHECK_ERROR((classes_[class_type.name].methods[tokens.back()].modifier != pkm::MethodType::INSTANCE),
            INSTANCE_CLASS_STATIC_INVOCATION, func_node, return pkm::DataType());

        function_node->modifier = pkm::MethodType::INSTANCE;
        function_node->name_parts.emplace_back(std::make_pair(tokens.back(), pkm::DataType()));
    }

    CHECK_ERROR((classes_[class_type.name].methods[tokens.back()].met_params.size() != func_node->branches_num()),
        INCORRECT_FUNCTION_PARAMETERS_NUMBER, func_node, return pkm::DataType());

    for (size_t i = 0; i < func_node->branches_num(); i++)
    {
        auto* func_par_node = static_cast<AST*>(&(*func_node)[i]);
        pkm::DataType func_par_type = analyzeObject(func_par_node);
        RETURN_ERROR(return pkm::DataType());

        pkm::DataType met_par_type = classes_[class_type.name].methods[tokens.back()].met_params[i];

        if ((met_par_type.type == pkm::VariableType::REFERENCE) || (func_par_type.type == pkm::VariableType::REFERENCE))
        {
            CHECK_ERROR((met_par_type.name != func_par_type.name), INCORRECT_PARAMETER_TYPE, func_par_node, return pkm::DataType());
        }

        if (met_par_type.name != func_par_type.name)
        {
            AST conv_node(std::make_shared<ast::ConversionNode>(ast::ConversionNode(func_par_type, met_par_type)));
            conv_node.emplace_branch(std::move(*func_par_node));
            *func_par_node = conv_node;
        }
    }

    pkm::DataType met_type = classes_[class_type.name].methods[tokens.back()].ret_type;
    function_node->ret_type = met_type;
    return met_type;
}

pkm::DataType Analyzer::analyzeVariableDeclaration(AST* var_decl_node)
{
    pkm::DataType var_type = static_cast<ast::VariableDeclarationNode*>(var_decl_node->value().get())->var_type;
    std::string var_name = static_cast<ast::VariableDeclarationNode*>(var_decl_node->value().get())->name;

    CHECK_ERROR(((var_type.type == pkm::VariableType::REFERENCE) && (!classes_.contains(var_type.name))),
        UNIDENTIFIED_TYPE, var_decl_node, return pkm::DataType());

    CHECK_ERROR((reserved_names_.contains(var_name)), RESERVED_NAME, var_decl_node, return pkm::DataType());
    CHECK_ERROR((locals_.contains(var_name)), OVERRIDING_VARIABLE, var_decl_node, return pkm::DataType());
    CHECK_ERROR(((classes_[class_name_].methods[method_name_].modifier == pkm::MethodType::INSTANCE) &&
        (classes_[class_name_].fields.contains(var_name))), OVERRIDING_VARIABLE_FIELD, var_decl_node, return pkm::DataType());

    locals_[var_name] = var_type;

    return var_type;
}

pkm::DataType Analyzer::analyzeVariable(AST* var_node)
{
    auto* variable_node = static_cast<ast::VariableNode*>(var_node->value().get());
    std::string var_name = variable_node->name;

    std::vector<std::string> tokens;
    split(var_name, &tokens);

    CHECK_ERROR(((classes_[class_name_].methods[method_name_].modifier == pkm::MethodType::STATIC) &&
        (!locals_.contains(tokens[0]))), UNIDENTIFIED_VARIABLE, var_node, return pkm::DataType());

    CHECK_ERROR(((classes_[class_name_].methods[method_name_].modifier == pkm::MethodType::INSTANCE) &&
        (!locals_.contains(tokens[0])) && (!classes_[class_name_].fields.contains(tokens[0]))),
        UNIDENTIFIED_VARIABLE, var_node, return pkm::DataType());

    pkm::DataType var_type;
    if (locals_.contains(tokens[0]))
    {
        var_type = locals_[tokens[0]];
    }
    else
    {
        var_type = classes_[class_name_].fields[tokens[0]].var_type;
        variable_node->name_parts.emplace_back(std::make_pair("this", pkm::DataType(pkm::VariableType::REFERENCE, class_name_)));
    }
    variable_node->name_parts.emplace_back(std::make_pair(tokens[0], var_type));

    if (var_type.type != pkm::VariableType::REFERENCE)
    {
        CHECK_ERROR((tokens.size() > 1), PRIMITIVE_TYPE_FIELD, var_node, return pkm::DataType());
        variable_node->var_type = var_type;
        return var_type;
    }

    for (size_t i = 1; i < tokens.size(); i++)
    {
        CHECK_ERROR((!classes_.contains(var_type.name)), UNIDENTIFIED_VARIABLE_TYPE, var_node, return pkm::DataType());
        CHECK_ERROR((!classes_[var_type.name].fields.contains(tokens[i])), UNIDENTIFIED_FIELD, var_node, return pkm::DataType());
        CHECK_ERROR((classes_[var_type.name].fields[tokens[i]].access_type == pkm::AccessType::PRIVATE),
            PRIVATE_FIELD, var_node, return pkm::DataType());

        var_type = classes_[var_type.name].fields[tokens[i]].var_type;
        variable_node->name_parts.emplace_back(std::make_pair(tokens[i], var_type));
    }

    variable_node->var_type = var_type;
    return var_type;
}