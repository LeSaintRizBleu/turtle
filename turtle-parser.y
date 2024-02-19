%{
#include <stdio.h>

#include "turtle-ast.h"

int yylex();
void yyerror(struct ast *ret, const char *);

%}

%debug
%defines

%define parse.error verbose

%parse-param { struct ast *ret }

%union {
  double value;
  char *name;
  struct ast_node *node;
}

%token <value>    VALUE       "value"
%token <name>     NAME        "name"
%token <name>     COMMENT     "comment"

%token            KW_FORWARD  "forward"

%token            KW_BACKWARD "backward"


/* TODO: add other tokens */

%type <node> unit cmds cmd expr color

%%

unit:
    cmds              { $$ = $1; ret->unit = $$; }
;

cmds:
    cmd cmds          { $1->next = $2; $$ = $1; }
  | /* empty */       { $$ = NULL; }
;

cmd:
    KW_FORWARD expr   { /* TODO */ }
    KW_BACKWARD expr   { /* TODO */ }
    KW_COLOR color   { /* TODO */ }
;

expr:
    VALUE             { $$ = make_expr_value($1); }
    /* TODO: add identifier */
;

color:
     expr expr expr {}
   | KW_RED

%%

void yyerror(struct ast *ret, const char *msg) {
  (void) ret;
  fprintf(stderr, "%s\n", msg);
}
