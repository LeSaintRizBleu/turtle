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

%%

{COMMENT}             { yylval.name = strdup(yytext); return COMMENT; }
{DOUBLE}              { yylval.value = strtod(yytext, NULL); return VALUE; }
forward|fw            { return KW_FORWARD; }
backward|bw           { return KW_BACKWARD; }
red                   { return KW_RED; }
green                 { return KW_GREEN;}
blue                  { return KW_BLUE;}
cyan                  { return KW_CYAN;}
magenta               { return KW_MAGENTA;}
yellow                { return KW_YELLOW;}
black                 { return KW_BLACK;}
gray                  { return KW_GRAY;}
white                 { return KW_WHITE;}

[\n\t ]*              /* whitespace */
.                     { fprintf(stderr, "Unknown token: '%s'\n", yytext); exit(EXIT_FAILURE); }

%%
