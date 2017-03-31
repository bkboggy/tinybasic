%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "tb.h"
#include "tbc.h"

/* Prototypes. */
int yylex(void);

/* Symbol table. */
symTable symb[26];
%}

%union {
    char var;       /* Variable. */
    int num;        /* Integer constant. */
    float dec;      /* Decimal constant. */
    nodeType *nPtr; /* Node pointer. */
};

%token <num> INTEGER
%token <var> VARIABLE
%token <dec> DECIMAL

%token PRINT IF GOTO INPUT LET END THEN CR

%left LT LE GT GE EQ NE 
%left '+' '-'
%left '*' '/'

%type <nPtr> line statement expression term factor number var 
%type <nPtr> relop var_list expr_list

%%

program:
    block                               { fprintf(stdout, "\treturn 0;\n}\n"); }
    ;

block:
    block line                          {}
    | line                              {}
    ;

line:
    INTEGER statement CR                { ex($2,$1); line = $1; freeNode($2); }
    | statement CR                      { ex($1,++line); freeNode($1); }
    | CR                                {}
    ;



statement:
    PRINT expr_list                     { $$ = opr(PRINT, 1, $2); }
    | IF expression relop expression THEN statement { $$ = opr(IF, 4, $2, $3, $4, $6); }
    | GOTO expression                   { $$ = opr(GOTO, 1, $2); }
    | INPUT var_list                    { $$ = opr(INPUT, 1, $2); }
    | LET var '=' expression            { $$ = opr(LET, 2, $2, $4); }
    | END                               { $$ = opr(END, 0);  }
    ;

expr_list:
    expr_list ','  expression           { $$ = opr('l', 2, $1, $3); }
    | expression                        { $$ = $1; }
    ;

var_list:
    var_list ','  var                   { $$ = opr('v', 2, $1, $3); }
    | var                               { $$ = $1; }
    ;

expression:
    expression '+' term                 { $$ = opr('+', 2, $1, $3); }
    | expression '-' term               { $$ = opr('-', 2, $1, $3); }
    | term                              { $$ = $1; }
    ;

term:
    term '*' factor                     { $$ = opr('*', 2, $1, $3); }
    | term '/' factor                   { $$ = opr('/', 2, $1, $3); }
    | factor                            { $$ = $1; }
    ;

factor:
    var                                 { $$ = $1; }
    | number                            { $$ = $1; }
    | '(' expression ')'                { $$ = opr('(', 1, $2); }
    ;

number:
    INTEGER                             { $$ = integer($1); }
    | DECIMAL                           { $$ = decimal($1); }
    ;

var:
    VARIABLE                            { $$ = variable($1); }
    ;

relop:
    LT                                  { $$ = opr(LT, 0); }
    | LE                                { $$ = opr(LE, 0); }
    | GT                                { $$ = opr(GT, 0); }
    | GE                                { $$ = opr(GE, 0); }
    | EQ                                { $$ = opr(EQ, 0); }
    | NE                                { $$ = opr(NE, 0); }
    ;

%%

int main(void) {
    yyparse();
    return 0;
}
