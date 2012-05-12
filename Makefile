parser: main.o cmmyacc.tab.o syntax_tree.o symbol_table.o semantic_analysis.o type.o translate.o inter_code.o
	gcc main.o cmmyacc.tab.o syntax_tree.o symbol_table.o semantic_analysis.o type.o translate.o inter_code.o -lfl -ly -g -o parser 

test: test_inter_code
	./test_inter_code test.ir

test_inter_code: test_inter_code.c inter_code.o common.h inter_code.h
	gcc -g test_inter_code.c inter_code.o -o test_inter_code

main.o: main.c symbol_table.h syntax_tree_node.h common.h semantic_analysis.h translate.h inter_code.h
	gcc -g -c main.c

cmmyacc.tab.o: cmmyacc.tab.c
	gcc -g -c cmmyacc.tab.c

syntax_tree.o: syntax_tree.c syntax_tree_node.h
	gcc -g -c syntax_tree.c

symbol_table.o: symbol_table.c symbol_table.h common.h
	gcc -g -c symbol_table.c

semantic_analysis.o: semantic_analysis.c symbol_table.h syntax_tree_node.h common.h semantic_analysis.h
	gcc -g -c semantic_analysis.c

type.o: type.c common.h
	gcc -g -c type.c

translate.o: translate.c symbol_table.h syntax_tree_node.h common.h translate.h inter_code.h
	gcc -g -c translate.c

inter_code.o: inter_code.c inter_code.h
	gcc -g -c inter_code.c

lex.yy.c: cmmlex.l
	flex cmmlex.l

cmmyacc.tab.c: cmmyacc.y lex.yy.c syntax_tree_node.h
	bison -dv cmmyacc.y

test1: parser test1.cmm
	./parser test1.cmm test1.ir

test2: parser test2.cmm
	./parser test2.cmm test2.ir

clean:
	rm lex.yy.c cmmyacc.tab.c cmmyacc.tab.h parser cmmyacc.output *.o test_inter_code *.ir
