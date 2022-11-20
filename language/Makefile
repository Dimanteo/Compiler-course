CC = g++

calc : lex.yy.c grammar.tab.c
	$(CC) -o $@ $^ -lfl

grammar.tab.c : grammar.y
	bison -d $^

lex.yy.c : tokenizer.lex
	lex $^
