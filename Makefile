CXX=g++
LDFLAGS=$(shell llvm-config --libs core native --cxxflags --ldflags)
CFLAGS=-std=gnu++11 -g -Wall -Wextra

OBJS=main.o parsing.o lexing.o _parser.o _lexer.o llvm_codegen.o
PROG=comp

.PHONY: all
all: $(PROG)

-include $(OBJS:.o=.d)

$(PROG): $(OBJS)
	$(CXX) -o $(PROG) $(OBJS) $(LDFLAGS) -ldl -lpthread -lncurses

_lexer.cpp: lexer.l yyparse.h lexing.h
	flex -o _lexer.cpp lexer.l

_parser.cpp yyparse.h: parser.y
	bison parser.y -o _parser.cpp --defines=yyparse.h

%.o: %.cpp
	$(CXX) -c $(CFLAGS) $*.cpp -o $*.o
	$(CXX) -MM $(CFLAGS) $*.cpp > $*.d

clean:
	rm -f $(PROG) *.o *.d _parser.cpp _lexer.cpp yyparse.h parser.output *.d

