#include "native_adapter.h"
#include "citadel/host.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
static CitadelStatus status(const CitadelCommand*c,CitadelResult*r,void*u){CitadelSystemSnapshot s;(void)c;(void)u;
 if(citadel_host_system_snapshot(&s)!=CITADEL_OK)return CITADEL_ERR_UNAVAILABLE;
 snprintf(r->text,sizeof(r->text),"Memory: %llu MB total, %llu MB available | Logical CPUs: %u",
 (unsigned long long)(s.total_memory_bytes/1048576u),(unsigned long long)(s.available_memory_bytes/1048576u),s.logical_cpu_count);return CITADEL_OK;}
static CitadelStatus clock_now(const CitadelCommand*c,CitadelResult*r,void*u){time_t now=time(NULL);(void)c;(void)u;
#if defined(_WIN32)
 {struct tm t;if(localtime_s(&t,&now))return CITADEL_ERR_UNAVAILABLE;if(!strftime(r->text,sizeof(r->text),"%A, %d %B %Y - %H:%M:%S",&t))return CITADEL_ERR_INTERNAL;}
#else
 {struct tm*t=localtime(&now);if(!t||!strftime(r->text,sizeof(r->text),"%A, %d %B %Y - %H:%M:%S",t))return CITADEL_ERR_UNAVAILABLE;}
#endif
 return CITADEL_OK;}
static CitadelStatus capabilities(const CitadelCommand*c,CitadelResult*r,void*u){
 const CitadelRegistry *registry=(const CitadelRegistry*)u;size_t used=0;uint32_t i;
 (void)c;if(!registry)return CITADEL_ERR_INTERNAL;
 used+=(size_t)snprintf(r->text+used,sizeof(r->text)-used,"Available read-only capabilities:");
 for(i=0;i<registry->capability_count&&used<sizeof(r->text);i++){
  const CitadelCapability *d=&registry->capabilities[i].descriptor;
  if(strcmp(d->id,"system.open_settings")==0)continue;
  if(d->required_permissions & ~(uint32_t)CITADEL_PERMISSION_READ_SYSTEM)continue;
  used+=(size_t)snprintf(r->text+used,sizeof(r->text)-used," %s;",d->id);
 }
 return CITADEL_OK;
}
CitadelStatus citadel_native_register(CitadelRegistry*r){CitadelCapability d;if(!r)return CITADEL_ERR_INVALID_ARGUMENT;memset(&d,0,sizeof(d));
 strcpy(d.id,"system.status");if(citadel_register_capability(r,&d,status,NULL))return CITADEL_ERR_INTERNAL;
 strcpy(d.id,"system.clock");if(citadel_register_capability(r,&d,clock_now,NULL))return CITADEL_ERR_INTERNAL;
 strcpy(d.id,"system.capabilities");if(citadel_register_capability(r,&d,capabilities,r))return CITADEL_ERR_INTERNAL;
 if(citadel_register_intent(r,"intent.system_status","system status","system.status"))return CITADEL_ERR_INTERNAL;
 if(citadel_register_intent(r,"intent.time","what time is it","system.clock"))return CITADEL_ERR_INTERNAL;
 if(citadel_register_intent(r,"intent.date","what is the date","system.clock"))return CITADEL_ERR_INTERNAL;
 if(citadel_register_intent(r,"intent.capabilities","capabilities list","system.capabilities"))return CITADEL_ERR_INTERNAL;
 return CITADEL_OK;}
