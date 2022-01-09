%option noyywrap

%{
#include <stdio.h>

#define YY_DECL int yylex()

#include "exp_engine.h"
#include "expressions.tab.h"

int lookup_word();

%}

word [a-zA-Z_][a-zA-Z0-9_]*
multiple_words "unsigned short"|"unsigned int"|"unsigned long"

%%

[ \t]	; // ignore all whitespace
-?[0-9]+\.[0-9]+ 	{ yylval.val.type = EXPRESSION_RESULT_FLOAT; yylval.val.as_float = atof(yytext); return T_PRIMITIVE_VALUE; }
-?[0-9]+		    { yylval.val.type = EXPRESSION_RESULT_INT32; yylval.val.as_int = atoi(yytext); return T_PRIMITIVE_VALUE; }
0x[0-9a-fA-F]+		{ yylval.val.type = EXPRESSION_RESULT_INT32; sscanf(yytext + 2, "%x", &yylval.val.as_int); return T_PRIMITIVE_VALUE; }
"&"		            { return T_AMPERSAND; }
{multiple_words}	{ return lookup_word(); }
{word}		        { return lookup_word(); }
"+"		            {return T_PLUS;}
"-"		            {return T_MINUS;}
"*"		            {return T_STAR;}
"/"		            {return T_SLASH;}
"("		            {return T_LEFT;}
")"		            {return T_RIGHT;}

%%

int lookup_word() {
    // it it a type?
    {
        enum expression_result_type_t type = expression_string_get_type(yytext);
        if (type != EXPRESSION_RESULT_UNKNOWN) {
            yylval.type.type = type;
            return T_PRIMITIVE_TYPE;
        }
    }

    struct expression_result_t result;
    struct lookup_t lookup;
    lookup.symbol_name = yytext;
    enum expression_result_type_t t = debug_lookup_symbol(&lookup, &result);

    switch (t)
    {
        case EXPRESSION_RESULT_UNKNOWN:
        {
            strcpy(yylval.errval, "<unknown>");
            return T_ERROR;
        }
        case EXPRESSION_RESULT_ERROR:
        {
            strcpy(yylval.errval, result.as_error);
            return T_ERROR;
        }
        case EXPRESSION_RESULT_POINTER:
        {
            yylval.val = result;
            return T_POINTER;
        }
        default:
        {
            yylval.val = result;
            return T_PRIMITIVE_VALUE;
        }
    }

    return T_STRING;
}