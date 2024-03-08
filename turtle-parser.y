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
    KW_FORWARD expr                     { $$ = make_cmd_simple(CMD_FORWARD, 1, (struct ast_node*[]){$2, NULL, NULL}); }
  | KW_BACKWARD expr                    { $$ = make_cmd_simple(CMD_BACKWARD, 1, (struct ast_node*[]){$2, NULL, NULL}); }
  | KW_POSITION expr ',' expr           { $$ = make_cmd_simple(CMD_POSITION, 2, (struct ast_node*[]){$2, $4, NULL}); }
  | KW_HEADING expr                     { $$ = make_cmd_simple(CMD_HEADING, 1, (struct ast_node*[]){$2, NULL, NULL}); }
  | KW_RIGHT expr                       { $$ = make_cmd_simple(CMD_RIGHT, 1, (struct ast_node*[]){$2, NULL, NULL}); }
  | KW_LEFT expr                        { $$ = make_cmd_simple(CMD_LEFT, 1, (struct ast_node*[]){$2, NULL, NULL}); }
  | KW_PRINT expr                       { $$ = make_cmd_simple(CMD_PRINT, 1, (struct ast_node*[]){$2, NULL, NULL}); }

  | KW_UP                               { $$ = make_cmd_simple(CMD_UP, 0, (struct ast_node*[]){NULL, NULL, NULL}); }
  | KW_DOWN                             { $$ = make_cmd_simple(CMD_DOWN, 0, (struct ast_node*[]){NULL, NULL, NULL}); }
  | KW_HOME                             { $$ = make_cmd_simple(CMD_HOME, 0, (struct ast_node*[]){NULL, NULL, NULL}); }

  | KW_SET NAME expr                    { $$ = make_cmd_set($2, $3); }
  | KW_CALL NAME                        { $$ = make_cmd_call($2); }
  | KW_PROC NAME  cmd                   { $$ = make_cmd_proc($2, $3); }
  | KW_REPEAT expr cmd                  { $$ = make_cmd_repeat($2, $3); }
  | '{' cmds '}'                        { $$ = make_cmd_block($2); }

  | KW_COLOR expr ',' expr ',' expr     { $$ = make_cmd_simple(CMD_COLOR, 3, (struct ast_node*[]){$2, $4, $6}); }
  | KW_COLOR KW_RED                     { $$ = make_cmd_simple(CMD_COLOR, 3, (struct ast_node*[]){
                                            make_expr_value(1.0),
                                            make_expr_value(0.0),
                                            make_expr_value(0.0)});}
  | KW_COLOR KW_GREEN                   { $$ = make_cmd_simple(CMD_COLOR, 3, (struct ast_node*[]){
                                            make_expr_value(0.0),
                                            make_expr_value(1.0),
                                            make_expr_value(0.0)});}
  | KW_COLOR KW_BLUE                    { $$ = make_cmd_simple(CMD_COLOR, 3, (struct ast_node*[]){
                                            make_expr_value(0.0),
                                            make_expr_value(0.0),
                                            make_expr_value(1.0)});}
  | KW_COLOR KW_CYAN                    { $$ = make_cmd_simple(CMD_COLOR, 3, (struct ast_node*[]){
                                            make_expr_value(0.0),
                                            make_expr_value(1.0),
                                            make_expr_value(1.0)});}
  | KW_COLOR KW_MAGENTA                 { $$ = make_cmd_simple(CMD_COLOR, 3, (struct ast_node*[]){
                                            make_expr_value(1.0),
                                            make_expr_value(0.0),
                                            make_expr_value(1.0)});}
  | KW_COLOR KW_YELLOW                  { $$ = make_cmd_simple(CMD_COLOR, 3, (struct ast_node*[]){
                                            make_expr_value(1.0),
                                            make_expr_value(1.0),
                                            make_expr_value(0.0)});}
  | KW_COLOR KW_BLACK                   { $$ = make_cmd_simple(CMD_COLOR, 3, (struct ast_node*[]){
                                            make_expr_value(0.0),
                                            make_expr_value(0.0),
                                            make_expr_value(0.0)});}
  | KW_COLOR KW_GRAY                    { $$ = make_cmd_simple(CMD_COLOR, 3, (struct ast_node*[]){
                                            make_expr_value(0.5),
                                            make_expr_value(0.5),
                                            make_expr_value(0.5)});}
  | KW_COLOR KW_WHITE                   { $$ = make_cmd_simple(CMD_COLOR, 3, (struct ast_node*[]){
                                            make_expr_value(1.0),
                                            make_expr_value(1.0),
                                            make_expr_value(1.0)});}
;

expr:
    VALUE                               { $$ = make_expr_value($1); }
  | NAME                                { $$ = make_expr_name($1); }
  | '-' expr %prec UNARY_MINUS          { $$ = make_expr_unop('-', $2); }
  | '(' expr ')'                        { $$ = make_expr_block($2); }
  | expr '+' expr                       { $$ = make_expr_binop('+', $1, $3); }
  | expr '-' expr                       { $$ = make_expr_binop('-', $1, $3); }
  | expr '*' expr                       { $$ = make_expr_binop('*', $1, $3); }
  | expr '/' expr                       { $$ = make_expr_binop('/', $1, $3); }
  | expr '^' expr                       { $$ = make_expr_binop('^', $1, $3); }
  | KW_SIN '(' expr ')'                 { $$ = make_expr_func(FUNC_SIN, 1, (struct ast_node*[]){$3, NULL, NULL}); }
  | KW_COS '(' expr ')'                 { $$ = make_expr_func(FUNC_COS, 1, (struct ast_node*[]){$3, NULL, NULL}); }
  | KW_TAN '(' expr ')'                 { $$ = make_expr_func(FUNC_TAN, 1, (struct ast_node*[]){$3, NULL, NULL}); }
  | KW_SQRT '(' expr ')'                { $$ = make_expr_func(FUNC_SQRT, 1, (struct ast_node*[]){$3, NULL, NULL}); }
  | KW_RANDOM '(' expr ',' expr ')'     { $$ = make_expr_func(FUNC_RANDOM, 2, (struct ast_node*[]){$3, $5, NULL}); }
  | KW_PI                               { $$ = make_expr_value(3.14159265358979323846); }
  | KW_SQRT2                            { $$ = make_expr_value(1.41421356237309504880); }
  | KW_SQRT3                            { $$ = make_expr_value(1.7320508075688772935); }
;

%%

void yyerror(struct ast *ret, const char *msg) {
  (void) ret;
  fprintf(stderr, "%s\n", msg);
}
