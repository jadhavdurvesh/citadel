#include "citadel/citadel.h"
#include "demo_adapter.h"
#include <stdio.h>
#include <string.h>
int main(void) {
 CitadelRegistry r; CitadelPolicy p; char line[CITADEL_MAX_INPUT+2]; CitadelCommand c; CitadelResult out;
 puts("Citadel 0.1.0 — offline assistant core. Type help or exit.");
 if (citadel_registry_init(&r)!=CITADEL_OK || citadel_demo_register(&r)!=CITADEL_OK) { fputs("Initialization failed.\n",stderr); return 1; }
 p.granted_permissions=CITADEL_PERMISSION_LAUNCH_APP|CITADEL_PERMISSION_READ_SYSTEM;
 p.confirmation_granted=0;
 for (;;) {
  size_t n;
  fputs("citadel> ",stdout); fflush(stdout);
  if (!fgets(line,sizeof(line),stdin)) break;
  n=strlen(line); if (n&&line[n-1]=='\n') line[--n]='\\0';
  if (!strcmp(line,"exit")||!strcmp(line,"quit")) break;
  if (citadel_parse(line,n,&c)!=CITADEL_OK) { puts("Invalid or oversized command."); continue; }
  if (citadel_dispatch(&r,&c,&p,&out)==CITADEL_OK) puts(out.text);
  else printf("Rejected (status %d).\n",(int)out.status);
 }
 puts("Citadel stopped.");
 return 0;
}
