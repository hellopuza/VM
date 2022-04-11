#include "Compiler/Lexer/Lexer.h"

void Lexer::setLocation()
{
    location.begin.line = location.end.line = lineno();
    int end_col = location.end.column;

    for (size_t i = 0; yytext[i]; ++i)
    {
        switch (yytext[i])
        {
        case ' ':
            location.begin.column++;
            location.end.column++;
            break;
        case '\n':
        case '\v':
        case '\r':
            location.begin.column = location.end.column = 1;
            location.end.line = lineno();
            break;
        case '\t':
            location.begin.column += 4;
            location.end.column += 4;
            break;
        default: 
            location.begin.column = end_col;
            location.end.column = location.begin.column + YYLeng();
        }
    }
}

yy::location Lexer::getLocation() const
{
    return location;
}