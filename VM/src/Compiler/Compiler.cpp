#include "Compiler/AST/ASTMaker.h"
#include "Compiler/Compiler.h"
#include "Compiler/Translator/Translator.h"

#include <cstring>
#include <fstream>

Compiler::Compiler(const char* lib_folder) : anal_(lib_folder) {}

void Compiler::load(const std::string& input_name)
{
    current_filename_ = input_name;

    ast::AST ast;
    std::ifstream file(input_name);
    if (file.is_open())
    {
        loadFile(input_name, &file);

        ASTMaker ast_maker(&file);
        ast_maker.make(&ast);

        if (ast_maker.getError().type != CompilationError::Type::OK)
        {
            error_ = ast_maker.getError();
            return;
        }
    }
    else
    {
        error_.type = CompilationError::Type::FILE_NOT_FOUND;
        return;
    }

    for (size_t i = 0; i < ast.branches_num(); i++)
    {
        classes_.emplace_back(std::make_pair(current_filename_, std::move(*static_cast<ast::AST*>(&ast[i]))));
    }
}

void Compiler::compile(const std::string& code_ext)
{
    for (const auto& [filename, class_ast] : classes_)
    {
        anal_.loadClass(&class_ast);
        if (anal_.getError().type != CompilationError::Type::OK)
        {
            error_ = anal_.getError();
            return;
        }
    }

    for (auto& [filename, class_ast] : classes_)
    {
        anal_.analyze(&class_ast);
        if (anal_.getError().type != CompilationError::Type::OK)
        {
            error_ = anal_.getError();
            return;
        }
    }

    for (auto& [filename, class_ast] : classes_)
    {
        std::ofstream file(static_cast<ast::ClassNode*>(class_ast.value().get())->name + code_ext);

        Translator trans(&class_ast);
        trans.translate(&file);
    }
}

void Compiler::printErrors(std::ostream& os)
{
    os << "\tCompilation error occured!\n";
    os << "Error: " << getErrorString() << "\n";
    os << "File: " << current_filename_ << ", line: " << error_.location.line << "\n";
    os << programs_[current_filename_][error_.location.line - 1] << "\n";
}

CompilationError Compiler::getError() const
{
    return error_;
}

void Compiler::loadFile(const std::string& input_name, std::ifstream* file)
{
    while (*file)
    {
        std::string line;
        std::getline(*file, line);
        programs_[input_name].push_back(line);
    }
}

const char* Compiler::getErrorString() const
{
    const char* errstr;
    switch (error_.type)
    {
    case CompilationError::Type::OK:
        errstr = "Ok";
        break;
    case CompilationError::Type::EXPECTED_OCB:
        errstr = "expected {";
        break;
    case CompilationError::Type::EXPECTED_CCB:
        errstr = "expected }";
        break;
    case CompilationError::Type::EXPECTED_ORB:
        errstr = "expected (";
        break;
    case CompilationError::Type::EXPECTED_CRB:
        errstr = "expected )";
        break;
    case CompilationError::Type::EXPECTED_CSB:
        errstr = "expected ]";
        break;
    case CompilationError::Type::EXPECTED_COLON:
        errstr = "expected ;";
        break;
    case CompilationError::Type::EXPECTED_CLASS:
        errstr = "expected class";
        break;
    case CompilationError::Type::EXPECTED_CLASS_NAME:
        errstr = "expected class name";
        break;
    case CompilationError::Type::EXPECTED_EXPRESSION:
        errstr = "expected class expression";
        break;
    case CompilationError::Type::EXPECTED_FIELD_NAME:
        errstr = "expected field name";
        break;
    case CompilationError::Type::EXPECTED_FIELD_TYPE:
        errstr = "expected field type";
        break;
    case CompilationError::Type::EXPECTED_METHOD_NAME:
        errstr = "expected method name";
        break;
    case CompilationError::Type::EXPECTED_METHOD_PARAMETER_NAME:
        errstr = "expected method parameter name";
        break;
    case CompilationError::Type::EXPECTED_METHOD_RETURN_TYPE:
        errstr = "expected method return type";
        break;
    case CompilationError::Type::EXPECTED_PRIMARY_EXPRESSION:
        errstr = "expected primary-expression";
        break;
    case CompilationError::Type::FILE_NOT_FOUND:
        errstr = "file not found";
        break;
    case CompilationError::Type::INCORRECT_ASSIGNMENT_LHS:
        errstr = "incorrect assignment lhs";
        break;
    case CompilationError::Type::INCORRECT_ASSIGNMENT_TYPE:
        errstr = "incorrect assignment type";
        break;
    case CompilationError::Type::INCORRECT_CONDITION_TYPE:
        errstr = "incorrect condition type";
        break;
    case CompilationError::Type::INCORRECT_FUNCTION_PARAMETERS_NUMBER:
        errstr = "incorrect function parameters number";
        break;
    case CompilationError::Type::INCORRECT_OPERAND:
        errstr = "incorrect operand";
        break;
    case CompilationError::Type::INCORRECT_OPERAND_TYPE:
        errstr = "incorrect operand type";
        break;
    case CompilationError::Type::INCORRECT_PARAMETER_TYPE:
        errstr = "incorrect parameter type";
        break;
    case CompilationError::Type::INCORRECT_RETURN_CONVERSION_TYPE:
        errstr = "incorrect return conversion type";
        break;
    case CompilationError::Type::INSTANCE_CLASS_STATIC_INVOCATION:
        errstr = "invocation static method by class instance";
        break;
    case CompilationError::Type::INSTANCE_INSIDE_STATIC_INVOCATION:
        errstr = "using class instance inside static invokation";
        break;
    case CompilationError::Type::OVERRIDING_CLASS:
        errstr = "class overriding";
        break;
    case CompilationError::Type::OVERRIDING_FIELD:
        errstr = "field overriding";
        break;
    case CompilationError::Type::OVERRIDING_METHOD:
        errstr = "method overriding";
        break;
    case CompilationError::Type::OVERRIDING_VARIABLE:
        errstr = "variable overriding";
        break;
    case CompilationError::Type::OVERRIDING_VARIABLE_FIELD:
        errstr = "variable overrides field";
        break;
    case CompilationError::Type::PRIMITIVE_TYPE_FIELD:
        errstr = "primitive type does not have fields";
        break;
    case CompilationError::Type::PRIVATE_FIELD:
        errstr = "private field access";
        break;
    case CompilationError::Type::PRIVATE_METHOD:
        errstr = "private method access";
        break;
    case CompilationError::Type::REQUIRED_CLASS_INSTANCE_FIELD_ACCESS:
        errstr = "class instance required to field access";
        break;
    case CompilationError::Type::REQUIRED_CLASS_INSTANCE_METHOD_ACCESS:
        errstr = "class instance required to method access";
        break;
    case CompilationError::Type::RESERVED_NAME:
        errstr = "reserved name";
        break;
    case CompilationError::Type::UNIDENTIFIED_FIELD:
        errstr = "unidentified field";
        break;
    case CompilationError::Type::UNIDENTIFIED_FIELD_TYPE:
        errstr = "unidentified field type";
        break;
    case CompilationError::Type::UNIDENTIFIED_FUNCTION:
        errstr = "unidentified function";
        break;
    case CompilationError::Type::UNIDENTIFIED_METHOD:
        errstr = "unidentified method";
        break;
    case CompilationError::Type::UNIDENTIFIED_METHOD_PARAMETER_TYPE:
        errstr = "unidentified method parameter type";
        break;
    case CompilationError::Type::UNIDENTIFIED_METHOD_RETURN_TYPE:
        errstr = "unidentified method return type";
        break;
    case CompilationError::Type::UNIDENTIFIED_TYPE:
        errstr = "unidentified type";
        break;
    case CompilationError::Type::UNIDENTIFIED_VARIABLE:
        errstr = "unidentified variable";
        break;
    case CompilationError::Type::UNIDENTIFIED_VARIABLE_TYPE:
        errstr = "unidentified variable type";
        break;
    }

    return errstr;
}