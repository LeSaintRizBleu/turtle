%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "turtle-ast.h"
#include "turtle-parser.h"
%}

%option warn 8bit nodefault noyywrap

DOUBLE (?:0|[1-9]+[0-9]*)?\.?[0-9]*(?:[eE][+-]?)?[0-9]*
COMMENT \#.*$
VAR_NAME [A-Z][A-Z0-9]*

%%

forward|fw            { return KW_FORWARD; }
backward|bw           { return KW_BACKWARD; }
right|rt              { return KW_RIGHT;}
left|lt               { return KW_LEFT;}
up                    { return KW_UP; }
down                  { return KW_DOWN; }
position|pos          { return KW_POSITION; }
heading|hd            { return KW_HEADING; }
color                 { return KW_COLOR; }
home                  { return KW_HOME; }
repeat                { return KW_REPEAT; }
set                   { return KW_SET; }
proc                  { return KW_PROC; }
call                  { return KW_CALL; }
print                 { return KW_PRINT; }

red                   { return KW_RED; }
green                 { return KW_GREEN;}
blue                  { return KW_BLUE;}
cyan                  { return KW_CYAN;}
magenta               { return KW_MAGENTA;}
yellow                { return KW_YELLOW;}
gray                  { return KW_GRAY;}
black                 { return KW_BLACK;}
white                 { return KW_WHITE;}

sin                   { return KW_SIN; }
cos                   { return KW_COS; }
tan                   { return KW_TAN; }
sqrt                  { return KW_SQRT; }
random                { return KW_RANDOM; }

PI                    { return KW_PI; }
SQRT2                 { return KW_SQRT2; }
SQRT3                 { return KW_SQRT3; }

\-                    { return '-'; }
\+                    { return '+'; }
\/                    { return '/'; }
\*                    { return '*'; }
\^                    { return '^'; }
"("                   { return '('; }
")"                   { return ')'; }
"{"                   { return '{'; }
"}"                   { return '}'; }
","                   { return ','; }

{COMMENT}             { yylval.name = strdup(yytext); return COMMENT; }
{DOUBLE}              { yylval.value = strtod(yytext, NULL); return VALUE; }
{VAR_NAME}            { yylval.name = strdup(yytext); return NAME; }

[\n\t ]*              /* whitespace */
.                     { fprintf(stderr, "Unknown token: '%s'\n", yytext); exit(EXIT_FAILURE); }

%%
