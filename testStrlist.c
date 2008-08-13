#define DEBUG

#include "strlist.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global Vars
int         debug;
strList     *l;
strList     *strings;
char        *dest;

int main(int argc, char **argv) {
    
    strings = NULL;
    
    STRLIST_GET(strings,"b",dest);
    STRLIST_GET(strings,"a",dest);
    STRLIST_GET(strings,"c",dest);
    STRLIST_GET(strings,"d",dest);
    STRLIST_GET(strings,"e",dest);
    
    l = strings;
	while(l) {
	   printf("%s\n",l->str);
	   l = l->next;
	}
	printf("----------\n");
	
    STRLIST_GET(strings,"a",dest);
    STRLIST_GET(strings,"c",dest);
    STRLIST_GET(strings,"d",dest);
    
    l = strings;
	while(l) {
	   printf("%s\n",l->str);
	   l = l->next;
	}
	printf("----------\n");
	
	return(0);
}