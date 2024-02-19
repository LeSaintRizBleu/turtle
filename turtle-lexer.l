%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "turtle-ast.h"
#include "turtle-parser.h"
%}

%option warn 8bit nodefault noyywrap

DOUBLE (?:0|[1-9]+[0-9]*)?\.?[0-9]*(?:[eE][+-]?)?[0-9]*

%%

{DOUBLE}              { yylval.value = strtod(yytext, NULL); return VALUE; }       
#.*$                  /* Comment */

[\n\t ]*              /* whitespace */
.                     { fprintf(stderr, "Unknown token: '%s'\n", yytext); exit(EXIT_FAILURE); }

%%
