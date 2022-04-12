#include "Compiler/AST/ASTMaker.h"

ASTMaker::ASTMaker(std::ifstream* file) : lexer_(new Lexer(file))
{
    while (*file)
    {
        std::string line;
        std::getline(*file, line);
        program_.push_back(line);
    }
    file->clear();
    file->seekg(0);
}

void ASTMaker::make(AST* ast)
{
    ast_ = ast;
    yy::parser parser(this);
    parser.parse();
}

yy::parser::token_type ASTMaker::yylex(yy::parser::semantic_type *yylval, yy::parser::location_type* location)
{
    *location = lexer_->getLocation();

    auto tt = static_cast<yy::parser::token_type>(lexer_->yylex());

    switch (tt)
    {
    case yy::parser::token_type::CLASS:
        yylval->build<NodeType>() = NodeType::CLASS;
        break;
    case yy::parser::token_type::PRIVATE:
        yylval->build<AccessType>() = AccessType::PRIVATE;
        break;
    case yy::parser::token_type::PUBLIC:
        yylval->build<AccessType>() = AccessType::PUBLIC;
        break;
    case yy::parser::token_type::NSTATIC:
        yylval->build<MethodType>() = MethodType::NONSTATIC;
        break;
    case yy::parser::token_type::STATIC:
        yylval->build<MethodType>() = MethodType::STATIC;
        break;
    case yy::parser::token_type::NATIVE:
        yylval->build<MethodType>() = MethodType::NATIVE;
        break;
    case yy::parser::token_type::VOID:
        yylval->build<VariableType>() = VariableType::VOID;
        break;
    case yy::parser::token_type::BOOLEAN:
        yylval->build<VariableType>() = VariableType::BOOLEAN;
        break;
    case yy::parser::token_type::BYTE:
        yylval->build<VariableType>() = VariableType::BYTE;
        break;
    case yy::parser::token_type::CHAR:
        yylval->build<VariableType>() = VariableType::CHAR;
        break;
    case yy::parser::token_type::SHORT:
        yylval->build<VariableType>() = VariableType::SHORT;
        break;
    case yy::parser::token_type::INT:
        yylval->build<VariableType>() = VariableType::INT;
        break;
    case yy::parser::token_type::LONG:
        yylval->build<VariableType>() = VariableType::LONG;
        break;
    case yy::parser::token_type::FLOAT:
        yylval->build<VariableType>() = VariableType::FLOAT;
        break;
    case yy::parser::token_type::DOUBLE:
        yylval->build<VariableType>() = VariableType::DOUBLE;
        break;
    case yy::parser::token_type::OR:
        yylval->build<OperationType>() = OperationType::OR;
        break;
    case yy::parser::token_type::AND:
        yylval->build<OperationType>() = OperationType::AND;
        break;
    case yy::parser::token_type::EQ:
        yylval->build<OperationType>() = OperationType::EQ;
        break;
    case yy::parser::token_type::NEQ:
        yylval->build<OperationType>() = OperationType::NEQ;
        break;
    case yy::parser::token_type::LEQ:
        yylval->build<OperationType>() = OperationType::LEQ;
        break;
    case yy::parser::token_type::GEQ:
        yylval->build<OperationType>() = OperationType::GEQ;
        break;
    case yy::parser::token_type::STL:
        yylval->build<OperationType>() = OperationType::STL;
        break;
    case yy::parser::token_type::STG:
        yylval->build<OperationType>() = OperationType::STG;
        break;
    case yy::parser::token_type::ADD:
        yylval->build<OperationType>() = OperationType::ADD;
        break;
    case yy::parser::token_type::SUB:
        yylval->build<OperationType>() = OperationType::SUB;
        break;
    case yy::parser::token_type::MUL:
        yylval->build<OperationType>() = OperationType::MUL;
        break;
    case yy::parser::token_type::DIV:
        yylval->build<OperationType>() = OperationType::DIV;
        break;
    case yy::parser::token_type::DOT:
        yylval->build<OperationType>() = OperationType::DOT;
        break;
    case yy::parser::token_type::COMMA:
        yylval->build<OperationType>() = OperationType::COMMA;
        break;
    case yy::parser::token_type::ASSOP:
        yylval->build<OperationType>() = OperationType::ASSOP;
        break;
    case yy::parser::token_type::NEW:
        yylval->build<OperationType>() = OperationType::NEW;
        break;
    case yy::parser::token_type::RETURN:
        yylval->build<OperationType>() = OperationType::RETURN;
        break;
    case yy::parser::token_type::IF:
        yylval->build<ControlType>() = ControlType::IF;
        break;
    case yy::parser::token_type::ELSE:
        yylval->build<ControlType>() = ControlType::ELSE;
        break;
    case yy::parser::token_type::FOR:
        yylval->build<ControlType>() = ControlType::FOR;
        break;
    case yy::parser::token_type::WHILE:
        yylval->build<ControlType>() = ControlType::WHILE;
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
    case yy::parser::token_type::OCBRACKET:
    case yy::parser::token_type::CCBRACKET:
    case yy::parser::token_type::ORBRACKET:
    case yy::parser::token_type::CRBRACKET:
    case yy::parser::token_type::OSBRACKET:
    case yy::parser::token_type::CSBRACKET:
    case yy::parser::token_type::SCOLON:
    case yy::parser::token_type::ERROR:
        break;
    }

    return tt;
}

int ASTMaker::lineno()
{
    return lexer_->lineno();
}

AST* ASTMaker::ast()
{
    return ast_;
}