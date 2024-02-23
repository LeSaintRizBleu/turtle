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
color                 { return KW_COLOR; }
up                    { return KW_UP; }
down                  { return KW_DOWN; }
repeat                { return KW_REPEAT; }
print                 { return KW_PRINT; }
right|rt              { return KW_RIGHT;}
left|lt               { return KW_LEFT;}
home                  { return KW_HOME; }
set                   { return KW_SET; }
proc                  { return KW_PROC; }
call                  { return KW_CALL; }

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
{VAR_NAME}            { yylval.name = strdup(yytext); return VAR_NAME; }

[\n\t ]*              /* whitespace */
.                     { fprintf(stderr, "Unknown token: '%s'\n", yytext); exit(EXIT_FAILURE); }

%%
