#define DEBUG


#include "ruleset.h"
#include "list.h"

#include <stdio.h>


int main(int argc, char **argv) {
	cfgRules   *rules;
	cfgRule    *rule;
	int        i=0;
	char       **ref;
	
	rules = (cfgRules *) rulesetRead("test/ruleset.xml");
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
	
	cfgStr *iter;
	LIST_FOREACH(iter, rules->keys) {
	   fprintf(stdout,"Key: %s\n", iter->str);
	}
	LIST_FOREACH(iter, rules->values) {
	   fprintf(stdout,"Value: %s\n", iter->str);
	}
}