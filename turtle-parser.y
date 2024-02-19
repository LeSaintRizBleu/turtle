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

%token            KW_COLOR    

%token            KW_RED
%token            KW_GREEN
%token            KW_BLUE
%token            KW_CYAN
%token            KW_MAGENTA
%token            KW_YELLOW
%token            KW_BLACK
%token            KW_GRAY
%token            KW_WHITE


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
    KW_BACKWARD expr  { /* TODO */ }
    KW_COLOR color    { /* TODO */ }
;

expr:
    VALUE             { $$ = make_expr_value($1); }
    /* TODO: add identifier */
;

color:
     expr expr expr {}
  |  KW_RED   {}
  |  KW_GREEN  {}
  |  KW_BLUE  {}
  |  KW_CYAN  {}
  |  KW_MAGENTA  {}
  |  KW_YELLOW  {}
  |  KW_BLACK  {}
  |  KW_GRAY  {}
  |  KW_WHITE  {}

%%

void yyerror(struct ast *ret, const char *msg) {
  (void) ret;
  fprintf(stderr, "%s\n", msg);
}
