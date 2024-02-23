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
%token <name>     VAR_NAME    "var_name"

%token            KW_FORWARD  "forward"
%token            KW_BACKWARD "backward"
%token            KW_UP       "up"
%token            KW_DOWN     "down"
%token            KW_REPEAT   "repeat"
%token            KW_PRINT    "print"
%token            KW_RIGHT    "right"
%token            KW_LEFT     "left"
%token            KW_HOME     "home"
%token            KW_SET      "set"
%token            KW_PROC     "proc"
%token            KW_CALL     "call"

%token            KW_SIN      "sin"
%token            KW_COS      "cos"
%token            KW_TAN      "tan"
%token            KW_SQRT     "sqrt"
%token            KW_RANDOM   "random"

%token            KW_COLOR    "color"
%token            KW_RED      "red"
%token            KW_GREEN    "green"
%token            KW_BLUE     "blue"
%token            KW_CYAN     "cyan"
%token            KW_MAGENTA  "magenta"
%token            KW_YELLOW   "yellow"
%token            KW_BLACK    "black"
%token            KW_GRAY     "gray"
%token            KW_WHITE    "white"


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
  |  KW_RED         {}
  |  KW_GREEN       {}
  |  KW_BLUE        {}
  |  KW_CYAN        {}
  |  KW_MAGENTA     {}
  |  KW_YELLOW      {}
  |  KW_BLACK       {}
  |  KW_GRAY        {}
  |  KW_WHITE       {}

%%

void yyerror(struct ast *ret, const char *msg) {
  (void) ret;
  fprintf(stderr, "%s\n", msg);
}
