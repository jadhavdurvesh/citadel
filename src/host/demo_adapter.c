#include "demo_adapter.h"
#include <stdio.h>
#include <string.h>
static CitadelStatus help(const CitadelCommand *c,CitadelResult *r,void *u) {(void)c;(void)u;snprintf(r->text,sizeof(r->text),"Commands: help, status, open settings");return CITADEL_OK;}
static CitadelStatus status(const CitadelCommand *c,CitadelResult *r,void *u) {(void)c;(void)u;snprintf(r->text,sizeof(r->text),"Citadel core is running; AI and network connectors are inactive.");return CITADEL_OK;}
static CitadelStatus settings(const CitadelCommand *c,CitadelResult *r,void *u) {(void)c;(void)u;snprintf(r->text,sizeof(r->text),"Demo only: no application launched and no setting changed.");return CITADEL_OK;}
CitadelStatus citadel_demo_register(CitadelRegistry *r) {
 CitadelCapability d; memset(&d,0,sizeof(d));
 strcpy(d.id,"core.help"); if (citadel_register_capability(r,&d,help,NULL)!=CITADEL_OK) return CITADEL_ERR_INTERNAL;
 strcpy(d.id,"core.status"); if (citadel_register_capability(r,&d,status,NULL)!=CITADEL_OK) return CITADEL_ERR_INTERNAL;
 strcpy(d.id,"system.open_settings"); d.required_permissions=CITADEL_PERMISSION_LAUNCH_APP;
 if (citadel_register_capability(r,&d,settings,NULL)!=CITADEL_OK) return CITADEL_ERR_INTERNAL;
 if (citadel_register_intent(r,"intent.help","help","core.help")!=CITADEL_OK) return CITADEL_ERR_INTERNAL;
 if (citadel_register_intent(r,"intent.status","status","core.status")!=CITADEL_OK) return CITADEL_ERR_INTERNAL;
 if (citadel_register_intent(r,"intent.settings","open settings","system.open_settings")!=CITADEL_OK) return CITADEL_ERR_INTERNAL;
 return CITADEL_OK;
}
