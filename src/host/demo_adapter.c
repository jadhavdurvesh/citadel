#include "demo_adapter.h"
#include <stdio.h>
#include <string.h>
static CitadelStatus help(const CitadelCommand*c,CitadelResult*r,void*u){(void)c;(void)u;
 snprintf(r->text,sizeof(r->text),"Commands: help, status, system status, capabilities list, what time is it, what is the date, history, exit");return CITADEL_OK;}
static CitadelStatus status(const CitadelCommand*c,CitadelResult*r,void*u){(void)c;(void)u;
 snprintf(r->text,sizeof(r->text),"Citadel core is running. Offline deterministic mode; AI and network connectors inactive.");return CITADEL_OK;}
static CitadelStatus settings(const CitadelCommand*c,CitadelResult*r,void*u){(void)c;(void)u;
 snprintf(r->text,sizeof(r->text),"Application launching is disabled in this host build. No setting was changed.");return CITADEL_OK;}
CitadelStatus citadel_demo_register(CitadelRegistry*r){CitadelCapability d;if(!r)return CITADEL_ERR_INVALID_ARGUMENT;memset(&d,0,sizeof(d));
 strcpy(d.id,"core.help");if(citadel_register_capability(r,&d,help,NULL))return CITADEL_ERR_INTERNAL;
 strcpy(d.id,"core.status");if(citadel_register_capability(r,&d,status,NULL))return CITADEL_ERR_INTERNAL;
 strcpy(d.id,"system.open_settings");d.required_permissions=CITADEL_PERMISSION_LAUNCH_APP;
 if(citadel_register_capability(r,&d,settings,NULL))return CITADEL_ERR_INTERNAL;
 if(citadel_register_intent(r,"intent.help","help","core.help"))return CITADEL_ERR_INTERNAL;
 if(citadel_register_intent(r,"intent.status","status","core.status"))return CITADEL_ERR_INTERNAL;
 if(citadel_register_intent(r,"intent.settings","open settings","system.open_settings"))return CITADEL_ERR_INTERNAL;
 return CITADEL_OK;}
