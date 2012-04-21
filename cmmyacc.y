%{
#define YYSTYPE syntax_tree_node*
#include "syntax_tree_node.h"
#include "lex.yy.c"

extern unsigned char is_error_happened;

void resume_from_error(char const *str);
syntax_tree_node *syntax_tree_root = 0;
%}

%error-verbose

%token INT FLOAT ID
%token SEMI COMMA
%token ASSIGNOP RELOP PLUS MINUS STAR DIV AND OR
%token DOT NOT
%token TYPE
%token LP RP LB RB LC RC
%token STRUCT RETURN IF ELSE WHILE

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%nonassoc LOWER_THAN_error
%nonassoc error

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right UMINUS NOT
%left DOT LB RB LP RP

%%

Program :   ExtDefList { syntax_tree_root = create_syntax_tree_node("Program", Program_SYNTAX, 1, $1); }
        ;

ExtDefList  :   ExtDef  ExtDefList { $$ = create_syntax_tree_node("ExtDefList", ExtDefList_SYNTAX, 2, $1, $2);  }
            |   /* empty */ { $$ = create_syntax_tree_node("ExtDefList", ExtDefList_SYNTAX, 0); }
            ;

ExtDef  :   Specifier ExtDecList SEMI { $$ = create_syntax_tree_node("ExtDef", ExtDef_SYNTAX, 3, $1, $2, $3);  }
        |   Specifier SEMI { $$ = create_syntax_tree_node("ExtDef", ExtDef_SYNTAX, 2, $1, $2);  }
        |   Specifier FunDec CompSt { $$ = create_syntax_tree_node("ExtDef", ExtDef_SYNTAX, 3, $1, $2, $3);  }
        |   error SEMI { resume_from_error("SEMI"); }
        ;

ExtDecList  :   VarDec { $$ = create_syntax_tree_node("ExtDecList", ExtDecList_SYNTAX, 1, $1);  }
            |   VarDec COMMA ExtDecList { $$ = create_syntax_tree_node("ExtDecList", ExtDecList_SYNTAX, 3, $1, $2, $3);  }
            ;

Specifier   :   TYPE { $$ = create_syntax_tree_node("Specifier", Specifier_SYNTAX, 1, $1);  }
            |   StructSpecifier { $$ = create_syntax_tree_node("Specifier", Specifier_SYNTAX, 1, $1);  }
            ;

StructSpecifier :   STRUCT OptTag LC DefList RC { $$ = create_syntax_tree_node("StructSpecifier", StructSpecifier_SYNTAX, 5, $1, $2, $3, $4, $5);  }
                |   STRUCT Tag { $$ = create_syntax_tree_node("StructSpecifier", StructSpecifier_SYNTAX, 2, $1, $2);  }
                ;

OptTag  :   ID { $$ = create_syntax_tree_node("OptTag", OptTag_SYNTAX, 1, $1);  }
        |   /* empty */ { $$ = create_syntax_tree_node("OptTag", OptTag_SYNTAX, 0);  }
        ;

Tag :   ID { $$ = create_syntax_tree_node("Tag", Tag_SYNTAX, 1, $1);  }
    ;

VarDec  :   ID { $$ = create_syntax_tree_node("VarDec", VarDec_SYNTAX, 1, $1);  }
        |   VarDec LB INT RB { $$ = create_syntax_tree_node("VarDec", VarDec_SYNTAX, 4, $1, $2, $3, $4);  }
        ;

FunDec  :   ID LP VarList RP { $$ = create_syntax_tree_node("FunDec", FunDec_SYNTAX, 4, $1, $2, $3, $4);  }
        |   ID LP RP { $$ = create_syntax_tree_node("FunDec", FunDec_SYNTAX, 3, $1, $2, $3);  }
        ;

VarList :   ParamDec COMMA VarList { $$ = create_syntax_tree_node("VarList", VarList_SYNTAX, 3, $1, $2, $3);  }
        |   ParamDec { $$ = create_syntax_tree_node("VarList", VarList_SYNTAX, 1, $1);  }
        ;

ParamDec    :   Specifier VarDec { $$ = create_syntax_tree_node("ParamDec", ParamDec_SYNTAX, 2, $1, $2);  }
            ;

CompSt  :   LC  DefList StmtList RC { $$ = create_syntax_tree_node("CompSt", CompSt_SYNTAX, 4, $1, $2, $3, $4);  }
        |   error RC %prec LOWER_THAN_error { resume_from_error("RC"); }
        ;

StmtList    :   Stmt StmtList { $$ = create_syntax_tree_node("StmtList", StmtList_SYNTAX, 2, $1, $2);  }
            |   /* empty */ { $$ = create_syntax_tree_node("StmtList", StmtList_SYNTAX, 0);  }
            ;

Stmt    :   Exp SEMI { $$ = create_syntax_tree_node("Stmt", Stmt_SYNTAX, 2, $1, $2);  }
        |   CompSt { $$ = create_syntax_tree_node("Stmt", Stmt_SYNTAX, 1, $1);  }
        |   RETURN Exp SEMI { $$ = create_syntax_tree_node("Stmt", Stmt_SYNTAX, 3, $1, $2, $3);  }
        |   IF LP Exp RP Stmt %prec LOWER_THAN_ELSE { $$ = create_syntax_tree_node("Stmt", Stmt_SYNTAX, 5, $1, $2, $3, $4, $5);  }
        |   IF LP Exp RP Stmt ELSE Stmt { $$ = create_syntax_tree_node("Stmt", Stmt_SYNTAX, 7, $1, $2, $3, $4, $5, $6, $7);  }
        |   WHILE LP Exp RP Stmt { $$ = create_syntax_tree_node("Stmt", Stmt_SYNTAX, 5, $1, $2, $3, $4, $5);  }
        |   error SEMI { resume_from_error("SEMI"); }
        ;

DefList :   Def DefList { $$ = create_syntax_tree_node("DefList", DefList_SYNTAX, 2, $1, $2);  }
        |   %prec LOWER_THAN_error /* empty */ { $$ = create_syntax_tree_node("DefList", DefList_SYNTAX, 0);  }
        ;

Def :   Specifier DecList SEMI { $$ = create_syntax_tree_node("Def", Def_SYNTAX, 3, $1, $2, $3);  }
    |   error SEMI { resume_from_error("SEMI"); }
    ;

DecList :   Dec { $$ = create_syntax_tree_node("DecList", DecList_SYNTAX, 1, $1);  }
        |   Dec COMMA DecList { $$ = create_syntax_tree_node("DecList", DecList_SYNTAX, 3, $1, $2, $3);  }
        ;

Dec :   VarDec { $$ = create_syntax_tree_node("Dec", Dec_SYNTAX, 1, $1);  }
    |   VarDec ASSIGNOP Exp { $$ = create_syntax_tree_node("Dec", Dec_SYNTAX, 3, $1, $2, $3);  }
    ;

Exp :   Exp ASSIGNOP Exp { $$ = create_syntax_tree_node("Exp", Exp_SYNTAX, 3, $1, $2, $3);  }
    |   Exp AND Exp { $$ = create_syntax_tree_node("Exp", Exp_SYNTAX, 3, $1, $2, $3);  }
    |   Exp OR Exp { $$ = create_syntax_tree_node("Exp", Exp_SYNTAX, 3, $1, $2, $3);  }
    |   Exp RELOP Exp { $$ = create_syntax_tree_node("Exp", Exp_SYNTAX, 3, $1, $2, $3);  }
    |   Exp PLUS Exp { $$ = create_syntax_tree_node("Exp", Exp_SYNTAX, 3, $1, $2, $3);  }
    |   Exp MINUS Exp { $$ = create_syntax_tree_node("Exp", Exp_SYNTAX, 3, $1, $2, $3);  }
    |   Exp STAR Exp { $$ = create_syntax_tree_node("Exp", Exp_SYNTAX, 3, $1, $2, $3);  }
    |   Exp DIV Exp { $$ = create_syntax_tree_node("Exp", Exp_SYNTAX, 3, $1, $2, $3);  }
    |   LP Exp RP { $$ = create_syntax_tree_node("Exp", Exp_SYNTAX, 3, $1, $2, $3);  }
    |   MINUS Exp %prec UMINUS { $$ = create_syntax_tree_node("Exp", Exp_SYNTAX, 2, $1, $2);  }
    |   NOT Exp { $$ = create_syntax_tree_node("Exp", Exp_SYNTAX, 2, $1, $2);  }
    |   ID LP Args RP { $$ = create_syntax_tree_node("Exp", Exp_SYNTAX, 4, $1, $2, $3, $4);  }
    |   ID LP RP { $$ = create_syntax_tree_node("Exp", Exp_SYNTAX, 3, $1, $2, $3);  }
    |   Exp LB Exp RB { $$ = create_syntax_tree_node("Exp", Exp_SYNTAX, 4, $1, $2, $3, $4);  }
    |   Exp DOT ID { $$ = create_syntax_tree_node("Exp", Exp_SYNTAX, 3, $1, $2, $3);  }
    |   ID { $$ = create_syntax_tree_node("Exp", Exp_SYNTAX, 1, $1);  }
    |   INT { $$ = create_syntax_tree_node("Exp", Exp_SYNTAX, 1, $1);  }
    |   FLOAT { $$ = create_syntax_tree_node("Exp", Exp_SYNTAX, 1, $1);  }
    ;

Args    :   Exp COMMA Args { $$ = create_syntax_tree_node("Args", Args_SYNTAX, 3, $1, $2, $3); }
        |   Exp { $$ = create_syntax_tree_node("Args", Args_SYNTAX, 1, $1);  }
        ;

%%

int yyerror(char const *str) {
    is_error_happened = 1;
    fprintf(stderr, "Error type 2 at line %d: %s\n", yylineno, str); 
}

void resume_from_error(char const *str) {
#ifdef PRINT_ERROK
    fprintf(stderr, "Resume from error at line %d: %s\n", yylineno, str);
#endif
}
