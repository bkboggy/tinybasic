%{
#include <stdlib.h>
#include "tb.h"
#include "tb.tab.h"

void yyerror(char *);
%}

%%

"LET"                   { return LET; }
"IF"                    { return IF; }
"THEN"                  { return THEN; }
"PRINT"                 { return PRINT; }
"GOTO"                  { return GOTO; }
"INPUT"                 { return INPUT; }
"END"                   { return END; }
"<"                     { return LT; }            
"<="                    { return LE; }
">"                     { return GT; }
">="                    { return GE; }
"=="                    { return EQ; }
"!="                    { return NE; }
\r?\n\r?                { return CR; }          
[A-Z]                   { yylval.var = yytext[0]; return VARIABLE; }
[-+]?(([1-9][0-9]*)|0)  { yylval.num = atoi(yytext); return INTEGER; }
[-+]?[0-9]*\.[0-9]+     { yylval.dec = atof(yytext); return DECIMAL; }
[-+*/=(),]              { return *yytext; }
[ \t]+                  { /* Ignore whitespace. */ } 
.                       { 
                            yyerror("[Parsing Error] Unknown syntax."); 
                            fprintf(stdout, "%s\n", yytext); 
                        }

%%

int yywrap(void) {
    return 1;
}
