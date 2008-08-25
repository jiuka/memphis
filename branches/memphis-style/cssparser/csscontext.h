#ifndef CSS_CONTEXT_H
#define CSS_CONTEXT_H

#include <glib.h>

typedef struct CSSClass {
    gchar *name;
    GHashTable *attributes;
} CSSClass;

CSSClass* css_class_new(const gchar *name);
void css_class_free(CSSClass *cls);

typedef struct CSSParserContext {
    void *scanner;
    unsigned int lineno;

    GHashTable *classes;
} CSSParserContext;

void init_context(CSSParserContext *ctx);
void destroy_context(CSSParserContext *ctx);

#endif /* CSS_CONTEXT_H */

