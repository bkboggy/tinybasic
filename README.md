# Modified Tiny BASIC compiler.

Uses Flex and Bison to translate modified tiny basic grammar into C code, which is then passed onto a C compiler to be compiled.

## Modified Tiny BASIC Grammar:

program ::= block

block ::= block line | 
		line

line ::= INTEGER statement CR | 
		statement CR

statement ::= PRINT expr-list |
              	IF expression relop expression THEN statement |
              	GOTO expression |
              	INPUT var-list |
              	LET var = expression |
              	END

expr-list ::= expr-list , expression | 
		expression

var-list ::= var-list , var | 
		var

expression ::= expression + term |
		expression - term |
		term

term ::= term * factor |
		term / factor |
		factor

factor ::= var | 
	number | 
	(expression)

number ::= INTEGER | DECIMAL

var ::= A | B | C .... | Y | Z

relop ::= < | <= | > | >= | == | !=
