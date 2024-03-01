all: lexer parser
	g++ -std=c++11  parser.tab.c lex.yy.c   ./front/SemanticAnalyzer.cpp ./main.cpp ./back/*.cpp  -o kompilator

lexer: 
	flex ./front/lexer.l

parser:
	bison -d ./front/parser.y

clean:
	rm  parser.tab.c lex.yy.c parser.tab.h 

clean_all: clean
	rm kompilator

