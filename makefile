test: test.c
	g++ test.c -o test

test.c: tb test.tb
	./tb < test.tb > test.c

all: tb

tb.tab.c tb.tab.h: tb.y
	bison -d tb.y

lex.yy.c: tb.l tb.tab.h
	flex tb.l

tb: lex.yy.c tb.tab.c tb.tab.h
	gcc -o tb tb.tab.c lex.yy.c

clean:
	rm tb tb.tab.c tb.tab.h lex.yy.c test test.c
