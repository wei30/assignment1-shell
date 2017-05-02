make all: myshell

lex.yy.c: shell.l
	flex shell.l

myshell: lex.yy.c argshell.c
	cc -o myshell argshell.c lex.yy.c -lfl

clean: 
	rm -rf *o myshell
