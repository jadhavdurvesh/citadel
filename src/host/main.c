#include "citadel/citadel.h"
#include "demo_adapter.h"
#include "native_adapter.h"
#include <stdio.h>
#include <string.h>
#define HISTORY_COUNT 10u
int main(void){
 CitadelRegistry registry;CitadelPolicy policy;char line[CITADEL_MAX_INPUT+2];
 char history[HISTORY_COUNT][CITADEL_MAX_INPUT+1];unsigned hc=0,hn=0;CitadelCommand command;CitadelResult result;
 puts("Citadel 0.3.0 - offline native assistant. Type help or exit.");
 if(citadel_registry_init(&registry)!=CITADEL_OK||citadel_demo_register(&registry)!=CITADEL_OK||citadel_native_register(&registry)!=CITADEL_OK){fputs("Initialization failed.\n",stderr);return 1;}
 policy.granted_permissions=CITADEL_PERMISSION_READ_SYSTEM;policy.confirmation_granted=0;
 for(;;){size_t n;fputs("citadel> ",stdout);fflush(stdout);if(!fgets(line,sizeof(line),stdin))break;
  n=strlen(line);while(n&&(line[n-1]=='\n'||line[n-1]=='\r'))line[--n]=0;
  if(!strcmp(line,"exit")||!strcmp(line,"quit"))break;if(!n)continue;
  if(!strcmp(line,"history")){unsigned i,start=hc<HISTORY_COUNT?0:hn;if(!hc){puts("No commands recorded.");continue;}
   for(i=0;i<hc;i++)printf("%u. %s\n",i+1,history[(start+i)%HISTORY_COUNT]);continue;}
  if(citadel_parse(line,n,&command)!=CITADEL_OK){puts("Invalid or oversized command.");continue;}
  strncpy(history[hn],line,CITADEL_MAX_INPUT);history[hn][CITADEL_MAX_INPUT]=0;hn=(hn+1)%HISTORY_COUNT;if(hc<HISTORY_COUNT)hc++;
  if(citadel_dispatch(&registry,&command,&policy,&result)==CITADEL_OK)puts(result.text);
  else printf("Not available or not authorized (status %d).\n",(int)result.status);
 }
 puts("Citadel stopped.");return 0;
}
