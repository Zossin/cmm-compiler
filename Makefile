parser: main.o cmmyacc.tab.o syntax_tree.o symbol_table.o
	gcc main.o cmmyacc.tab.o syntax_tree.o symbol_table.o -lfl -ly -o parser 

main.o: main.c symbol_table.h syntax_tree_node.h common.h
	gcc -c main.c

cmmyacc.tab.o: cmmyacc.tab.c
	gcc -c cmmyacc.tab.c

syntax_tree.o: syntax_tree.c syntax_tree_node.h
	gcc -c syntax_tree.c

symbol_table.o: symbol_table.c symbol_table.h syntax_tree_node.h common.h
	gcc -c symbol_table.c

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

clean:
	rm lex.yy.c cmmyacc.tab.c cmmyacc.tab.h parser cmmyacc.output *.o
