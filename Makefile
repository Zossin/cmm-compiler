parser: main.o cmmyacc.tab.o syntax_tree.o symbol_table.o semantic_analysis.o type.o translate.o inter_code.o
	gcc main.o cmmyacc.tab.o syntax_tree.o symbol_table.o semantic_analysis.o type.o translate.o inter_code.o -lfl -ly -g -o parser 

test: test_inter_code
	./test_inter_code

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
	./parser test1.cmm

test2: parser test2.cmm
	./parser test2.cmm

test3: parser test3.cmm
	./parser test3.cmm

test4: parser test4.cmm
	./parser test4.cmm

test5: parser test5.cmm
	./parser test5.cmm

test6: parser test6.cmm
	./parser test6.cmm

test7: parser test7.cmm
	./parser test7.cmm

test8: parser test8.cmm
	./parser test8.cmm

test9: parser test9.cmm
	./parser test9.cmm

test10: parser test10.cmm
	./parser test10.cmm

test11: parser test11.cmm
	./parser test11.cmm

test12: parser test12.cmm
	./parser test12.cmm

test13: parser test13.cmm
	./parser test13.cmm

clean:
	rm lex.yy.c cmmyacc.tab.c cmmyacc.tab.h parser cmmyacc.output *.o test_inter_code
