/*
 * Simplified CSS Parser
 */
%pure-parser
%name-prefix="CSS_"
%locations
%defines
%error-verbose
%parse-param { CSSParserContext *context }
%lex-param { void *scanner }

%union {
    char *string;
    int number;
    unsigned int color;
}

%{
#include "csscontext.h"
#include <stdio.h>

int CSS_lex(YYSTYPE* lvalp, YYLTYPE* llocp, void *scanner);

void CSS_error(YYLTYPE* locp, CSSParserContext *ctx, const char *err);

#define scanner (context->scanner)
%}

%token T_O_BRACE
%token T_C_BRACE
%token T_DOT
%token T_COLON
%token T_SEMICOLON
%token T_STAR
%token <string> T_IDENTIFIER
%token <number> T_VALUE
%token <string> T_STRING
%token <string> T_COLOR
%token T_ID_SELECTOR
%token T_UNKNOWN

%start input

%%

input:      /* empty */
        |   input rule
        ;

rule:       T_DOT T_IDENTIFIER T_O_BRACE block T_C_BRACE
                                        { printf("class: %s\n", $2); }
        |   T_ID_SELECTOR T_IDENTIFIER T_O_BRACE block T_C_BRACE
                                        { printf("id: %s\n", $2); }
        |   T_STAR T_O_BRACE block T_C_BRACE
                                        { printf("any\n"); }
        ;

block:      /* empty */
        |   attribute
        |   attribute T_SEMICOLON block
        ;

attribute:  T_IDENTIFIER T_COLON value  { printf("prop: %s\n", $1); }
        ;

value:      T_IDENTIFIER                { printf("ident: %s\n", $1); }
        |   T_VALUE                     { printf("value: %d\n", $1); }
        |   T_STRING                    { printf("strin: %s\n", $1); }
        |   T_COLOR                     { printf("color: %s\n", $1); }
        ;

%%


void CSS_error(YYLTYPE* locp, CSSParserContext *ctx, const char *err)
{
    fprintf (stderr, "%d: %s\n", locp->first_line, err);
}

extern void init_scanner(CSSParserContext *ctx);
extern void destroy_scanner(CSSParserContext *ctx);

void init_context(CSSParserContext *ctx)
{
    g_return_if_fail(ctx != NULL);

    init_scanner(ctx);

    ctx->classes = g_hash_table_new(g_str_hash, g_str_equal);
}

void destroy_context(CSSParserContext *ctx)
{
    destroy_scanner(ctx);
}

int main(int argc, char *argv[])
{
    CSSParserContext context;

    init_context(&context);

    if (!CSS_parse(&context)) {
        fprintf(stdout, "finished parsing\n");
    }

    destroy_context(&context);

    return 0;
}

CSSClass* css_class_new(const gchar *name)
{
    CSSClass *cls = g_new(CSSClass, 1);
    cls->name = g_strdup(name);
    cls->attributes = g_hash_table_new(g_str_hash, g_str_equal);
    return cls;
}

void css_class_free(CSSClass *cls)
{
    g_free(cls->name);
    g_hash_table_destroy(cls->attributes);
    g_free(cls);
}


