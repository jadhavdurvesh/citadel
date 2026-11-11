#include "citadel/citadel.h"
#include "demo_adapter.h"
#include "native_adapter.h"
#include <stdio.h>
#include <string.h>
int main(void) {
 CitadelRegistry registry; CitadelPolicy policy; char line[CITADEL_MAX_INPUT+2];
 CitadelCommand command; CitadelResult result;
 puts("Citadel 0.2.0 - offline native assistant. Type help or exit.");
 if(citadel_registry_init(&registry)!=CITADEL_OK ||
    citadel_demo_register(&registry)!=CITADEL_OK ||
    citadel_native_register(&registry)!=CITADEL_OK) {
  fputs("Initialization failed.\n",stderr); return 1;
 }
 policy.granted_permissions=CITADEL_PERMISSION_READ_SYSTEM;
 policy.confirmation_granted=0;
 for(;;) {
  size_t n; fputs("citadel> ",stdout); fflush(stdout);
  if(!fgets(line,sizeof(line),stdin))break;
  n=strlen(line); if(n && (line[n-1]=='\n'||line[n-1]=='\r'))line[--n]=0;
  if(!strcmp(line,"exit")||!strcmp(line,"quit"))break;
  if(citadel_parse(line,n,&command)!=CITADEL_OK){puts("Invalid or oversized command.");continue;}
  if(citadel_dispatch(&registry,&command,&policy,&result)==CITADEL_OK)puts(result.text);
  else printf("Not available or not authorized (status %d).\n",(int)result.status);
 }
 puts("Citadel stopped."); return 0;
}
