parser: main.c cmmyacc.tab.c lex.yy.c syntax_tree.c syntax_tree_node.h
	gcc main.c cmmyacc.tab.c syntax_tree.c -lfl -ly -o parser 

lex.yy.c: cmmlex.l
	flex cmmlex.l

cmmyacc.tab.c: cmmyacc.y
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
	rm lex.yy.c cmmyacc.tab.c cmmyacc.tab.h parser cmmyacc.output
