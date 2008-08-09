#include "libxml/parser.h"
#include "stdio.h"

#include "osm05.h"

char string1[]="user";
char string2[]="timestamp";

/**
 * startDocumentDebug:
 * @ctxt:  An XML parser context
 *
 * called when the document start being processed.
 */
static void
startDocumentDebug(void *ctx )
{
    fprintf(stdout, "SAX.startDocument()\n");
}

/**
 * endDocumentDebug:
 * @ctxt:  An XML parser context
 *
 * called when the document end has been detected.
 */
static void
endDocumentDebug(void *ctx)
{
    fprintf(stdout, "SAX.endDocument()\n");
}/*
 * SAX2 specific callbacks
 */
/**
 * startElementNsDebug:
 * @ctxt:  An XML parser context
 * @name:  The element name
 *
 * called when an opening tag has been processed.
 */
static void
startElementDebug(void *ctx,
                    const xmlChar *localname,
		    const xmlChar **attributes)
{
    fprintf(stdout, "SAX.startElement(%s", (char *) localname);
    while (*attributes != NULL) {
        if(strncmp((char *) *(attributes), string1, 4) == 0) {
            attributes+=2;
            continue;}
        if(strncmp((char *) *(attributes), string2, 9) == 0) {
            attributes+=2;
            continue;}
		fprintf(stdout, ", %s:", (char *) *(attributes++));
		fprintf(stdout, "%s", (char *) *(attributes++));
	}
    fprintf(stdout, ")\n");
}

/**
 * endElementDebug:
 * @ctxt:  An XML parser context
 * @name:  The element name
 *
 * called when the end of an element has been detected.
 */
static void
endElementDebug(void *ctx ,
                  const xmlChar *localname)
{
    fprintf(stdout, "SAX.endElement(%s)\n", (char *) localname);
}


xmlSAXHandler debugSAX2HandlerStruct = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    startDocumentDebug,
    endDocumentDebug,
    startElementDebug,
    endElementDebug,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

xmlSAXHandlerPtr debugSAX2Handler = &debugSAX2HandlerStruct;

int main(int argc, char **argv) {
    
    xmlInitParser();
    
    xmlParserCtxtPtr ctxt;
    int res;
    
    res = xmlSAXUserParseFile(debugSAX2Handler, NULL,
			                          "map.osm");
    
    xmlCleanupParser();
    xmlMemoryDump();

    return(0);
}
