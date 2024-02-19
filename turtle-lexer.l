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

{COMMENT}             { yylval.name = strdup(yytext); return COMMENT};
{DOUBLE}              { yylval.value = strtod(yytext, NULL); return VALUE; }
forward|fw            { return KW_FORWARD; }
backward|bw            { return KW_BACKWARD; }

[\n\t ]*              /* whitespace */
.                     { fprintf(stderr, "Unknown token: '%s'\n", yytext); exit(EXIT_FAILURE); }

%%
