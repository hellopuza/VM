#include "Compiler/AST/ASTMaker.h"

ASTMaker::ASTMaker(std::ifstream* file) : lexer_(new Lexer(file))
{
    file->clear();
    file->seekg(0);
}

void ASTMaker::make(ast::AST* ast)
{
    ast_ = ast;
    yy::parser parser(this);
    parser.parse();
}

yy::parser::token_type ASTMaker::yylex(yy::parser::semantic_type *yylval, yy::parser::location_type* location) const
{
    *location = lexer_->getLocation();

    auto tt = static_cast<yy::parser::token_type>(lexer_->yylex());

    switch (tt)
    {
    case yy::parser::token_type::CLASS:
        yylval->build<pkm::ASTNodeType>() = pkm::ASTNodeType::CLASS;
        break;
    case yy::parser::token_type::PRIVATE:
        yylval->build<pkm::AccessType>() = pkm::AccessType::PRIVATE;
        break;
    case yy::parser::token_type::PUBLIC:
        yylval->build<pkm::AccessType>() = pkm::AccessType::PUBLIC;
        break;
    case yy::parser::token_type::INSTANCE:
        yylval->build<pkm::MethodType>() = pkm::MethodType::INSTANCE;
        break;
    case yy::parser::token_type::STATIC:
        yylval->build<pkm::MethodType>() = pkm::MethodType::STATIC;
        break;
    case yy::parser::token_type::NATIVE:
        yylval->build<pkm::MethodType>() = pkm::MethodType::NATIVE;
        break;
    case yy::parser::token_type::VOID:
        yylval->build<pkm::VariableType>() = pkm::VariableType::VOID;
        break;
    case yy::parser::token_type::CHAR:
        yylval->build<pkm::VariableType>() = pkm::VariableType::CHAR;
        break;
    case yy::parser::token_type::INT:
        yylval->build<pkm::VariableType>() = pkm::VariableType::INT;
        break;
    case yy::parser::token_type::FLOAT:
        yylval->build<pkm::VariableType>() = pkm::VariableType::FLOAT;
        break;
    case yy::parser::token_type::RETURN:
        yylval->build<pkm::OperationType>() = pkm::OperationType::RETURN;
        break;
    case yy::parser::token_type::ASSIGN:
        yylval->build<pkm::OperationType>() = pkm::OperationType::ASSIGN;
        break;
    case yy::parser::token_type::OR:
        yylval->build<pkm::OperationType>() = pkm::OperationType::OR;
        break;
    case yy::parser::token_type::AND:
        yylval->build<pkm::OperationType>() = pkm::OperationType::AND;
        break;
    case yy::parser::token_type::EQ:
        yylval->build<pkm::OperationType>() = pkm::OperationType::EQ;
        break;
    case yy::parser::token_type::NEQ:
        yylval->build<pkm::OperationType>() = pkm::OperationType::NEQ;
        break;
    case yy::parser::token_type::LEQ:
        yylval->build<pkm::OperationType>() = pkm::OperationType::LEQ;
        break;
    case yy::parser::token_type::GEQ:
        yylval->build<pkm::OperationType>() = pkm::OperationType::GEQ;
        break;
    case yy::parser::token_type::STL:
        yylval->build<pkm::OperationType>() = pkm::OperationType::STL;
        break;
    case yy::parser::token_type::STG:
        yylval->build<pkm::OperationType>() = pkm::OperationType::STG;
        break;
    case yy::parser::token_type::SHL:
        yylval->build<pkm::OperationType>() = pkm::OperationType::SHL;
        break;
    case yy::parser::token_type::SHR:
        yylval->build<pkm::OperationType>() = pkm::OperationType::SHR;
        break;
    case yy::parser::token_type::ADD:
        yylval->build<pkm::OperationType>() = pkm::OperationType::ADD;
        break;
    case yy::parser::token_type::SUB:
        yylval->build<pkm::OperationType>() = pkm::OperationType::SUB;
        break;
    case yy::parser::token_type::MUL:
        yylval->build<pkm::OperationType>() = pkm::OperationType::MUL;
        break;
    case yy::parser::token_type::DIV:
        yylval->build<pkm::OperationType>() = pkm::OperationType::DIV;
        break;
    case yy::parser::token_type::REM:
        yylval->build<pkm::OperationType>() = pkm::OperationType::REM;
        break;
    case yy::parser::token_type::NOT:
        yylval->build<pkm::OperationType>() = pkm::OperationType::NOT;
        break;
    case yy::parser::token_type::NEW:
        yylval->build<pkm::OperationType>() = pkm::OperationType::NEW;
        break;
    case yy::parser::token_type::IF:
        yylval->build<pkm::ControlType>() = pkm::ControlType::IF;
        break;
    case yy::parser::token_type::ELSE:
        yylval->build<pkm::ControlType>() = pkm::ControlType::ELSE;
        break;
    case yy::parser::token_type::ELIF:
        yylval->build<pkm::ControlType>() = pkm::ControlType::ELIF;
        break;
    case yy::parser::token_type::WHILE:
        yylval->build<pkm::ControlType>() = pkm::ControlType::WHILE;
        break;
    case yy::parser::token_type::NUL:
        yylval->build<std::string>() = "null";
        break;
    case yy::parser::token_type::THIS:
        yylval->build<std::string>() = "this";
        break;
    case yy::parser::token_type::FALSE:
        yylval->build<bool>() = false;
        break;
    case yy::parser::token_type::TRUE:
        yylval->build<bool>() = true;
        break;
    case yy::parser::token_type::WORD:
    case yy::parser::token_type::WORD_DOT:
        yylval->build<std::string>() = lexer_->YYText();
        break;
    case yy::parser::token_type::INTNUMBER:
        yylval->build<int32_t>() = std::stoi(lexer_->YYText());
        break;
    case yy::parser::token_type::FLOATNUMBER:
        yylval->build<float>() = std::stof(lexer_->YYText());
        break;
    case yy::parser::token_type::STRING:
        yylval->build<std::string>() = lexer_->YYText();
        break;
    case yy::parser::token_type::SYMBOL:
        yylval->build<char>() = lexer_->YYText()[1];
        break;
    case yy::parser::token_type::OCB:
    case yy::parser::token_type::CCB:
    case yy::parser::token_type::ORB:
    case yy::parser::token_type::CRB:
    case yy::parser::token_type::OSB:
    case yy::parser::token_type::CSB:
    case yy::parser::token_type::SCOLON:
    case yy::parser::token_type::COMMA:
    default:
        break;
    }

    return tt;
}

void ASTMaker::pushError(CompilationError::Type type, const yy::location& location)
{
    error_.type = type;
    error_.location.begin = location.begin.column;
    error_.location.end = location.end.column;
    error_.location.line = location.begin.line;
}

CompilationError ASTMaker::getError() const
{
    return error_;
}

ast::AST* ASTMaker::ast()
{
    return ast_;
}