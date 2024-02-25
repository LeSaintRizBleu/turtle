%{
#include <stdio.h>
#include <math.h>

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
%token            KW_POSITION "position"
%token            KW_HEADING  "heading"
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
%token            KW_PI       "pi"
%token            KW_SQRT2    "sqrt2"
%token            KW_SQRT3    "sqrt3"
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

%type <node> unit cmds cmd expr

%left '+' '-'
%left '*' '/'
%left UNARY_MINUS
%left '^'

%%

unit:
    cmds                                { $$ = $1; ret->unit = $$; }
;

cmds:
    cmd cmds                            { $1->next = $2; $$ = $1; }
  | /* empty */                         { $$ = NULL; }
;

cmd:
    KW_FORWARD expr                     { /* TODO */ }
  | KW_BACKWARD expr                    { /* TODO */ }
  | KW_POSITION expr ',' expr           { /* TODO */ }
  | KW_HEADING expr                     { /* TODO */ }
  | KW_RIGHT expr                       { /* TODO */ }
  | KW_LEFT expr                        { /* TODO */ }
  | KW_PRINT expr                       { /* TODO */ }

  
  | KW_UP                               { /* TODO */ }
  | KW_DOWN                             { /* TODO */ }
  | KW_HOME                             { /* TODO */ }

  | KW_SET NAME expr                    { /* TODO */ }
  | KW_CALL NAME                        { /* TODO */ }
  | KW_PROC NAME '{' cmds '}'           { /* TODO */ }
  | KW_REPEAT expr '{' cmds '}'         { /* TODO */ }
  
  | KW_COLOR expr ',' expr ',' expr     { /* TODO */ }
  | KW_COLOR KW_RED                     { /* TODO */ }
  | KW_COLOR KW_GREEN                   { /* TODO */ }
  | KW_COLOR KW_BLUE                    { /* TODO */ }
  | KW_COLOR KW_CYAN                    { /* TODO */ }
  | KW_COLOR KW_MAGENTA                 { /* TODO */ }
  | KW_COLOR KW_YELLOW                  { /* TODO */ }
  | KW_COLOR KW_BLACK                   { /* TODO */ }
  | KW_COLOR KW_GRAY                    { /* TODO */ }
  | KW_COLOR KW_WHITE                   { /* TODO */ }
;

expr:
    VALUE                               { $$ = make_expr_value($1); }
  | NAME                                { $$ = make_expr_name($1); }
  | COMMENT                             { /* nothing */ }
  | '-' expr %prec UNARY_MINUS          { $$ = make_expr_value(-make_double($2)); }
  | '(' expr ')'                        { $$ = $2; }
  | expr '+' expr                       { $$ = make_expr_value(make_double($1) + make_double($3)); }
  | expr '-' expr                       { $$ = make_expr_value(make_double($1) - make_double($3)); }
  | expr '*' expr                       { $$ = make_expr_value(make_double($1) * make_double($3)); }
  | expr '/' expr                       { $$ = make_expr_value(make_double($1) / make_double($3)); }
  | expr '^' expr                       { $$ = make_expr_value(pow(make_double($1), make_double($3))); }
  | KW_SIN '(' expr ')'                 { $$ = make_expr_value(sin(make_double($3))); }
  | KW_COS '(' expr ')'                 { $$ = make_expr_value(cos(make_double($3))); }
  | KW_TAN '(' expr ')'                 { $$ = make_expr_value(tan(make_double($3))); }
  | KW_SQRT '(' expr ')'                { $$ = make_expr_value(sqrt(make_double($3))); }
  | KW_RANDOM '(' expr ',' expr ')'     { $$ = make_expr_value(generate_random_number(make_double($3), make_double($5))); }
  | KW_PI                               { $$ = make_expr_value(3.14159265358979323846); }
  | KW_SQRT2                            { $$ = make_expr_value(1.41421356237309504880); }
  | KW_SQRT3                            { $$ = make_expr_value(1.7320508075688772935); }
;

%%

void yyerror(struct ast *ret, const char *msg) {
  (void) ret;
  fprintf(stderr, "%s\n", msg);
}
