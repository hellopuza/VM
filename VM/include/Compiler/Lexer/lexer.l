%option c++
%option yylineno
%option yyclass="Lexer"
%option noyywrap

%{ 
#include "Compiler/Lexer/Lexer.h"
#include "parser.hh"

#define YY_USER_ACTION setLocation();
%}

WS           [ \t\n\v\r]+
DIGIT        [0-9]
DIGIT1       [1-9]
DOT          [.]
WORD         [_a-zA-Z]+{DIGIT}*
INTNUMBER    {DIGIT1}{DIGIT}*|0
FLOATNUMBER  {INTNUMBER}{DOT}{DIGIT}+
QUOTES       ["]
QUOTE        [']
STRING       {QUOTES}.*{QUOTES}
SYMBOL       {QUOTE}.{QUOTE}
W_DOT        {WORD}{DOT}
WORD_DOT     {W_DOT}+{WORD}

%%

"boolean"                   { return yy::parser::token_type::BOOLEAN;  }
"byte"                      { return yy::parser::token_type::BYTE;     }
"char"                      { return yy::parser::token_type::CHAR;     }
"class"                     { return yy::parser::token_type::CLASS;    }
"double"                    { return yy::parser::token_type::DOUBLE;   }
"elif"                      { return yy::parser::token_type::ELIF;     }
"else"                      { return yy::parser::token_type::ELSE;     }
"false"                     { return yy::parser::token_type::FALSE;    }
"float"                     { return yy::parser::token_type::FLOAT;    }
"if"                        { return yy::parser::token_type::IF;       }
"int"                       { return yy::parser::token_type::INT;      }
"instance"                  { return yy::parser::token_type::INSTANCE; }
"long"                      { return yy::parser::token_type::LONG;     }
"native"                    { return yy::parser::token_type::NATIVE;   }
"new"                       { return yy::parser::token_type::NEW;      }
"null"                      { return yy::parser::token_type::NUL;      }
"private"                   { return yy::parser::token_type::PRIVATE;  }
"public"                    { return yy::parser::token_type::PUBLIC;   }
"return"                    { return yy::parser::token_type::RETURN;   }
"short"                     { return yy::parser::token_type::SHORT;    }
"static"                    { return yy::parser::token_type::STATIC;   }
"this"                      { return yy::parser::token_type::THIS;     }
"true"                      { return yy::parser::token_type::TRUE;     }
"void"                      { return yy::parser::token_type::VOID;     }
"while"                     { return yy::parser::token_type::WHILE;    }

"{"		                    { return yy::parser::token_type::OCB;    }
"}"		                    { return yy::parser::token_type::CCB;    }
"("		                    { return yy::parser::token_type::ORB;    }
")"		                    { return yy::parser::token_type::CRB;    }
"["		                    { return yy::parser::token_type::OSB;    }
"]"		                    { return yy::parser::token_type::CSB;    }
";"                         { return yy::parser::token_type::SCOLON; }
","                         { return yy::parser::token_type::COMMA;  }

"="                         { return yy::parser::token_type::ASSIGN; }
"||"                        { return yy::parser::token_type::OR;     }
"&&"                        { return yy::parser::token_type::AND;    }
"=="                        { return yy::parser::token_type::EQ;     }
"!="                        { return yy::parser::token_type::NEQ;    }
"<="                        { return yy::parser::token_type::LEQ;    }
">="                        { return yy::parser::token_type::GEQ;    }
"<"                         { return yy::parser::token_type::STL;    }
">"                         { return yy::parser::token_type::STG;    }
"<<"                        { return yy::parser::token_type::SHL;    }
">>"                        { return yy::parser::token_type::SHR;    }
"+"                         { return yy::parser::token_type::ADD;    }
"-"                         { return yy::parser::token_type::SUB;    }
"*"                         { return yy::parser::token_type::MUL;    }
"/"                         { return yy::parser::token_type::DIV;    }
"%"                         { return yy::parser::token_type::REM;    }
"!"                         { return yy::parser::token_type::NOT;    }

"//".*
{WS}
{WORD}                      { return yy::parser::token_type::WORD;        }
{WORD_DOT}                  { return yy::parser::token_type::WORD_DOT;    }
{INTNUMBER}                 { return yy::parser::token_type::INTNUMBER;   }
{FLOATNUMBER}               { return yy::parser::token_type::FLOATNUMBER; }
{STRING}                    { return yy::parser::token_type::STRING;      }
{SYMBOL}                    { return yy::parser::token_type::SYMBOL;      }

%%