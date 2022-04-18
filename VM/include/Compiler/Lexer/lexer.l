%option c++
%option yylineno
%option yyclass="Lexer"
%option noyywrap

%{ 
#include "Compiler/Lexer/Lexer.h"
#include "parser.hh"

#define YY_USER_ACTION setLocation();
%}

WS           [ \t\n\v]+
DIGIT        [0-9]
DIGIT1       [1-9]
WORD         [_a-zA-Z]+{DIGIT}*
WRONGNUMBER  0+{DIGIT}+
INTNUMBER    {DIGIT1}{DIGIT}*|0
FLOATNUMBER  {INTNUMBER}"."{WRONGNUMBER}
QUOTES       ["]
QUOTE        [']

%%

"boolean"                   { return yy::parser::token_type::BOOLEAN; }
"byte"                      { return yy::parser::token_type::BYTE;    }
"char"                      { return yy::parser::token_type::CHAR;    }
"class"                     { return yy::parser::token_type::CLASS;   }
"double"                    { return yy::parser::token_type::DOUBLE;  }
"else"                      { return yy::parser::token_type::ELSE;    }
"false"                     { return yy::parser::token_type::FALSE;   }
"float"                     { return yy::parser::token_type::FLOAT;   }
"for"                       { return yy::parser::token_type::FOR;     }
"if"                        { return yy::parser::token_type::IF;      }
"int"                       { return yy::parser::token_type::INT;     }
"long"                      { return yy::parser::token_type::LONG;    }
"native"                    { return yy::parser::token_type::NATIVE;  }
"new"                       { return yy::parser::token_type::NEW;     }
"null"                      { return yy::parser::token_type::NUL;     }
"private"                   { return yy::parser::token_type::PRIVATE; }
"public"                    { return yy::parser::token_type::PUBLIC;  }
"return"                    { return yy::parser::token_type::RETURN;  }
"short"                     { return yy::parser::token_type::SHORT;   }
"static"                    { return yy::parser::token_type::STATIC;  }
"this"                      { return yy::parser::token_type::THIS;    }
"true"                      { return yy::parser::token_type::TRUE;    }
"void"                      { return yy::parser::token_type::VOID;    }
"while"                     { return yy::parser::token_type::WHILE;   }

"{"		                    { return yy::parser::token_type::OCBRACKET; }
"}"		                    { return yy::parser::token_type::CCBRACKET; }
"("		                    { return yy::parser::token_type::ORBRACKET; }
")"		                    { return yy::parser::token_type::CRBRACKET; }
"["		                    { return yy::parser::token_type::OSBRACKET; }
"]"		                    { return yy::parser::token_type::CSBRACKET; }
";"                         { return yy::parser::token_type::SCOLON;    }
"."                         { return yy::parser::token_type::DOT;       }
","                         { return yy::parser::token_type::COMMA;     }

"||"                        { return yy::parser::token_type::OR;  }
"&&"                        { return yy::parser::token_type::AND; }
"=="                        { return yy::parser::token_type::EQ;  }
"!="                        { return yy::parser::token_type::NEQ; }
"<="                        { return yy::parser::token_type::LEQ; }
">="                        { return yy::parser::token_type::GEQ; }
"<"                         { return yy::parser::token_type::STL; }
">"                         { return yy::parser::token_type::STG; }

"+"                         { return yy::parser::token_type::ADD;   }
"-"                         { return yy::parser::token_type::SUB;   }
"*"                         { return yy::parser::token_type::MUL;   }
"/"                         { return yy::parser::token_type::DIV;   }
"="                         { return yy::parser::token_type::ASSOP; }

"//".*
{WS}
{WORD}                      { return yy::parser::token_type::WORD;        }
{INTNUMBER}                 { return yy::parser::token_type::INTNUMBER;   }
{FLOATNUMBER}               { return yy::parser::token_type::FLOATNUMBER; }
{QUOTES}.*{QUOTES}          { return yy::parser::token_type::STRING;      }
{QUOTE}.{QUOTE}             { return yy::parser::token_type::SYMBOL;      }
{DIGIT}+{WORD}              { return yy::parser::token_type::ERROR;       }
{WRONGNUMBER}               { return yy::parser::token_type::ERROR;       }
.                           { return yy::parser::token_type::ERROR;       }

%%