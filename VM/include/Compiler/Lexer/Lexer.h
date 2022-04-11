#ifndef COMPILER_LEXER_LEXER_H
#define COMPILER_LEXER_LEXER_H

#if !defined(yyFlexLexerOnce)
#include "FlexLexer.h"
#endif

#include "location.hh"

class Lexer : public yyFlexLexer
{
public:
    Lexer() = default;

    void setLocation();
    yy::location getLocation() const;
    int yylex() override;

private:
    yy::location location;
};

#endif // COMPILER_LEXER_LEXER_H