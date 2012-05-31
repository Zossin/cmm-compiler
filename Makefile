cc: main.o cmmyacc.tab.o syntax_tree.o symbol_table.o semantic_analysis.o type.o translate.o inter_code.o var_table.o
	gcc main.o cmmyacc.tab.o syntax_tree.o symbol_table.o semantic_analysis.o type.o translate.o inter_code.o var_table.o -lfl -ly -g -o cc 

#test: test_inter_code
#	./test_inter_code test.s

#test_inter_code: test_inter_code.c inter_code.o common.h inter_code.h
#	gcc -g test_inter_code.c inter_code.o -o test_inter_code

main.o: main.c symbol_table.h syntax_tree_node.h common.h semantic_analysis.h translate.h inter_code.h
	gcc -g -c main.c

cmmyacc.tab.o: cmmyacc.tab.c
	gcc -g -c cmmyacc.tab.c

syntax_tree.o: syntax_tree.c syntax_tree_node.h
	gcc -g -c syntax_tree.c

symbol_table.o: symbol_table.c symbol_table.h common.h
	gcc -g -c symbol_table.c

var_table.o: var_table.c common.h var_table.h
	gcc -g -c var_table.c

semantic_analysis.o: semantic_analysis.c symbol_table.h syntax_tree_node.h common.h semantic_analysis.h
	gcc -g -c semantic_analysis.c

type.o: type.c common.h
	gcc -g -c type.c

translate.o: translate.c symbol_table.h syntax_tree_node.h common.h translate.h inter_code.h
	gcc -g -c translate.c

inter_code.o: inter_code.c inter_code.h common.h
	gcc -g -c inter_code.c

gen_code.o: gen_code.c inter_code.h gen_code.h var_table.h common.h
	gcc -g -c gen_code.c

lex.yy.c: cmmlex.l
	flex cmmlex.l

cmmyacc.tab.c: cmmyacc.y lex.yy.c syntax_tree_node.h
	bison -dv cmmyacc.y

test1: cc test1.cmm
	./cc test1.cmm test1.s

test2: cc test2.cmm
	./cc test2.cmm test2.s

test3: cc test3.cmm
	./cc test3.cmm test3.s

test4: cc test4.cmm
	./cc test4.cmm test4.s

test5: cc test5.cmm
	./cc test5.cmm test5.s

test6: cc test6.cmm
	./cc test6.cmm test6.s

test7: cc test7.cmm
	./cc test7.cmm test7.s

test8: cc test8.cmm
	./cc test8.cmm test8.s

clean:
	rm lex.yy.c cmmyacc.tab.c cmmyacc.tab.h cc cmmyacc.output *.o *.s
