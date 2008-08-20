#define DEBUG


#include "ruleset.h"
#include "list.h"
#include "strlist.h"

#include <stdio.h>

// Global Vars
int         debug;
strList     *keyStrings;
strList     *valStrings;

int main(int argc, char **argv) {
	cfgRules   *rules;
	cfgRule    *rule;
	cfgDraw    *draw;
	int        i=0;
	char       **ref;
	
	debug = 2;
	
	rules = (cfgRules *) rulesetRead("rule.xml");
	
	fprintf(stdout,"------------------------\n");
	rule = rules->rule;
	while(rule) {
	   i++;
       int h;
       for(h=0;h<rule->d;h++){
            fprintf(stdout,"-");
       }
       
	   fprintf(stdout,"rule %i %i\n",(rule->type&WAY),(rule->type&NODE));
	   
	   ref = rule->key;
	   while (*ref != NULL) {
            fprintf(stdout," K: %s\n", *(ref));
            ref++;
       }
	   ref = rule->value;
	   while (*ref != NULL) {
            fprintf(stdout," V: %s\n", *(ref));
            ref++;
       }
       
	   draw = rule->draw;
	   while (draw) {
            fprintf(stdout," D: %i\n", draw->type);
            draw = draw->next;
       }
	   
	   
	   if(rule->nsub) {
        for(h=0;h<rule->d;h++){
            fprintf(stdout,"-");
        }
	           fprintf(stdout,"else\n");
       }
       	   
	   if(rule->sub != NULL) {
	       rule = rule->sub;
	   } else {
	       rule = rule->next;
	   }
	}
	fprintf(stdout,"Rules %i:%i\n", rules->cntRule, i);
	fprintf(stdout,"Else  %i\n", rules->cntElse);
	fprintf(stdout,"------------------------\n");
}