%{
#include <stdio.h>
#include <malloc.h>
#include "cmmyacc.tab.h"

extern unsigned char is_error_happened;

extern struct st_node *create_token_node(char *name);
%}

%option yylineno

INT 429496729[0-5]|42949672[0-8][0-9]|4294967[0-1][0-9]{2}|429496[0-6][0-9]{3}|42949[0-5][0-9]{4}|4294[0-8][0-9]{5}|429[0-3][0-9]{6}|42[0-8][0-9]{7}|4[0-1][0-9]{8}|[1-3][0-9]{9}|[1-9][0-9]{0,8}|0|0[xX][0-9a-fA-F]{1,8}|0+[1-3][0-7]{10}|0+[0-7]{1,10}
FLOAT   [0-9]*\.[0-9]+|[0-9]+\.|[0-9]*\.[0-9]+[eE][+-]?[0-9]+|[0-9]+(\.[0-9]*)?[eE][+-]?[0-9]+
ID  [a-zA-Z][_0-9a-zA-Z]{0,31}  
SEMI    ;
COMMA   ,
ASSIGNOP    =
RELOP   >|<|>=|<=|==|!=
PLUS    \+
MINUS   -
STAR    \*
DIV \/
AND &&
OR  \|\|
DOT \.
NOT !
TYPE    int|float
LP  \(
RP  \)
LB  \[
RB  \]
LC  \{
RC  \}
STRUCT  struct
RETURN  return
IF  if
ELSE    else
WHILE   while

BLANK   [ \t\n\r]

%%

{BLANK}    { }

"//"    {
    char c = input();
    while (c != '\n' && c != EOF) c = input();
}

"/*"    {
    int nesting_comment_lineno = 0;
    char c;
    int state = 0;
    while (state != 3) {
        c = input();
        if (c == EOF) {
            nesting_comment_lineno = 0;
            fprintf(stdout, "Error type 3 at line %d: no matching \"*/\"\n", yylineno);
            break;
        }
        if (state == 0) {
            if (c == '*') {
                state = 2;   
            }
            else if (c == '/') {
                state = 1;
            }
            else {
                state = 0;
            }
        }


        if (state == 1) {
            if (c == '*') {
                nesting_comment_lineno = yylineno;
                state = 0;   
            }
            else if (c == '/') {
                state = 1;
            }
            else {
                state = 0;
            }
        }

        if (state == 2) {
            if (c == '*') {
                state = 2;   
            }
            else if (c == '/') {
                state = 3;
            }
            else {
                state = 0;
            }
        }
    }
    if (nesting_comment_lineno) {
        is_error_happened = 1;
        fprintf(stderr, "Error type 3 at line %d: nesting comments\n", nesting_comment_lineno);
    }
}

{INT} { 
#ifdef PRINT_TOKEN
	fprintf(stdout, "INT\n");
#endif
    yylval = create_token_node("INT");
    yylval->value.int_val = atoi_ext(yytext);
	return INT;
}

{FLOAT} { 
#ifdef PRINT_TOKEN
	fprintf(stdout, "FLOAT\n");
#endif
    yylval = create_token_node("FLOAT");
    yylval->value.float_val = atof(yytext);
    return FLOAT;
}

{SEMI} { 
#ifdef PRINT_TOKEN
	fprintf(stdout, "SEMI\n");
#endif
    yylval = create_token_node("SEMI");
	return SEMI;
}

{COMMA}   { 
#ifdef PRINT_TOKEN
	fprintf(stdout, "COMMA\n");
#endif
    yylval = create_token_node("COMMA");
	return COMMA;
}

{ASSIGNOP}    { 
#ifdef PRINT_TOKEN
	fprintf(stdout, "ASSIGNOP\n");
#endif
    yylval = create_token_node("ASSIGNOP");
	return ASSIGNOP;
}

{RELOP}   { 
#ifdef PRINT_TOKEN
	fprintf(stdout, "RELOP\n");
#endif
    yylval = create_token_node("RELOP");
	return RELOP;
}

{PLUS}    { 
#ifdef PRINT_TOKEN
	fprintf(stdout, "PLUS\n");
#endif
    yylval = create_token_node("PLUS");
	return PLUS;
}

{MINUS}   { 
#ifdef PRINT_TOKEN
	fprintf(stdout, "MINUS\n");
#endif
    yylval = create_token_node("MINUS");
	return MINUS;
}

{STAR}    { 
#ifdef PRINT_TOKEN
	fprintf(stdout, "STAR\n");
#endif
    yylval = create_token_node("STAR");
	return STAR;
}

{DIV} { 
#ifdef PRINT_TOKEN
	fprintf(stdout, "DIV\n");
#endif
    yylval = create_token_node("DIV");
	return DIV;
}

{AND} { 
#ifdef PRINT_TOKEN
	fprintf(stdout, "AND\n");
#endif
    yylval = create_token_node("AND");
	return AND;
}

{OR}  { 
#ifdef PRINT_TOKEN
	fprintf(stdout, "OR\n");
#endif
    yylval = create_token_node("OR");
	return OR;
}

{DOT} { 
#ifdef PRINT_TOKEN
	fprintf(stdout, "DOT\n");
#endif
    yylval = create_token_node("DOT");
	return DOT;
}

{NOT} { 
#ifdef PRINT_TOKEN
	fprintf(stdout, "NOT\n");
#endif
    yylval = create_token_node("NOT");
	return NOT;
}

{TYPE}    { 
#ifdef PRINT_TOKEN
	fprintf(stdout, "TYPE\n");
#endif
    yylval = create_token_node("TYPE");
    if (strcmp(yytext, "int") == 0)
        yylval->value.type_val = Int;
    else if (strcmp(yytext, "float") == 0)
        yylval->value.type_val = Float;
	return TYPE;
}

{LP}  { 
#ifdef PRINT_TOKEN
	fprintf(stdout, "LP\n");
#endif
    yylval = create_token_node("LP");
	return LP;
}

{RP}  { 
#ifdef PRINT_TOKEN
	fprintf(stdout, "RP\n");
#endif
    yylval = create_token_node("RP");
	return RP;
}

{LB}  { 
#ifdef PRINT_TOKEN
	fprintf(stdout, "LB\n");
#endif
    yylval = create_token_node("LB");
	return LB;
}

{RB}  { 
#ifdef PRINT_TOKEN
	fprintf(stdout, "RB\n");
#endif
    yylval = create_token_node("RB");
	return RB;
}

{LC}  { 
#ifdef PRINT_TOKEN
	fprintf(stdout, "LC\n");
#endif
    yylval = create_token_node("LC");
	return LC;
}

{RC}  { 
#ifdef PRINT_TOKEN
	fprintf(stdout, "RC\n");
#endif
    yylval = create_token_node("RC");
	return RC;
}

{STRUCT}  { 
#ifdef PRINT_TOKEN
	fprintf(stdout, "STRUCT\n");
#endif
    yylval = create_token_node("STRUCT");
	return STRUCT;
}

{RETURN}  { 
#ifdef PRINT_TOKEN
	fprintf(stdout, "RETURN\n");
#endif
    yylval = create_token_node("RETURN");
	return RETURN;
}

{IF}  { 
#ifdef PRINT_TOKEN
	fprintf(stdout, "IF\n");
#endif
    yylval = create_token_node("IF");
	return IF;
}

{ELSE}    { 
#ifdef PRINT_TOKEN
	fprintf(stdout, "ELSE\n");
#endif
    yylval = create_token_node("ELSE");
	return ELSE;
}

{WHILE}   { 
#ifdef PRINT_TOKEN
	fprintf(stdout, "WHILE\n");
#endif
    yylval = create_token_node("WHILE");
	return WHILE;
}

{ID}  { 
#ifdef PRINT_TOKEN
	fprintf(stdout, "ID\n");
#endif
	yylval = create_token_node("ID");
    yylval->value.str_val = (char*)malloc(yyleng + 1);
    strcpy(yylval->value.str_val, yytext);
	return ID;
}

.  {
    is_error_happened = 1;
    fprintf(stderr, "Error type 1 at line %d: Mysterious character '%s'\n", yylineno, yytext); 
}

%%

int atoi_ext(char *str) {
    int val = 0;
    int radix;

    if (*str == '0' && (str[1] == 'x' || str[1] == 'X')) {
        radix = 16;
        str += 2;
    }
    else if (*str == '0') {
        radix = 8;
        ++ str;
    }
    else
        radix = 10;

    while (*str) {
        if (radix == 16) {
            if (*str >= '0' && *str <= '9')
                val = (val << 4) + *str - '0';
            else if (*str >= 'A' && *str <= 'F')
                val = (val << 4) + *str - 'A' + 10;
            else
                val = (val << 4) + *str - 'a';
        }
        else if (radix == 8) {
            val = (val << 3) + *str - '0';
        }
        else
            val = val * 10 + *str - '0';
        ++ str;
    }

    return val;
}

struct st_node *create_token_node(char *name) {
    syntax_tree_node *p_node = (struct st_node*)malloc(sizeof(struct st_node));
    p_node->is_token = TRUE;
    p_node->name = name;
    p_node->lchild = 0;
    p_node->next_sibling = 0;
    p_node->lineno = yylineno;
    return p_node;
}
