/*
 * Simplified CSS lexer
 */
%option reentrant 
%option noyywrap
%option prefix="CSS_"
%option bison-bridge
%option bison-locations
%option yylineno
/* see: http://www.phpcompiler.org/doc/reentrantparser.html */
%{
#include "csscontext.h"
#include "cssparser.tab.h"
#define YY_EXTRA_TYPE CSSParserContext*
#define YY_USER_ACTION yylloc->first_line = yylineno;
%}
%x COMMENT

DIGIT       [0-9]
HEX         [0-9a-fA-F]
ID          [a-zA-Z][a-zA-Z0-9-]*
VALUE       [0-9]+
COLOR       \#({HEX}{3}|{HEX}{6}|{HEX}{8})

%%
\/\/.*$                 { /* skip */ }
[ \t]                   { /* skip */ }
"\n"                    { yyextra->lineno++; }

"{"                     { return T_O_BRACE; }
"}"                     { return T_C_BRACE; }
"."                     { return T_DOT; }
":"                     { return T_COLON; }
";"                     { return T_SEMICOLON; }
"*"                     { return T_STAR; }
"#"                     { return T_ID_SELECTOR; }
{ID}                    { yylval->string = yytext; return T_IDENTIFIER; }
{VALUE}                 { yylval->number = atoi(yytext); return T_VALUE; }
{COLOR}                 { yylval->string = yytext; return T_COLOR; }
\"([^"\n]|\\\")*\"      { yylval->string = yytext; return T_STRING; }
.                       { return T_UNKNOWN; }
%%


void init_scanner(CSSParserContext *ctx)
{
    yylex_init(&ctx->scanner);
    yyset_extra(ctx, ctx->scanner);
}

void destroy_scanner(CSSParserContext *ctx)
{
    yylex_destroy(ctx->scanner);
    ctx->scanner = NULL;
}

#if 0
int main(int argc, char * argv[])
{
    yyscan_t scanner;
    int tok;
    CSS_data data = {
        .line = 0,
    };

    yylex_init(&scanner);
    yyset_extra(&data, scanner);

    while ((tok = yylex(scanner)) > 0) {
        if (tok == T_UNKNOWN)
            fprintf(stderr, "Unknown element: '%s'\n", yyget_text(scanner));
        printf("%d: tok=%d  yytext=%s\n",
               yyget_extra(scanner)->line,
               tok,
               yyget_text(scanner));
    }

    yylex_destroy(scanner);

    return 0;
}
#endif

