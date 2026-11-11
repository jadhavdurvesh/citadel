#include "citadel/citadel.h"
#include <string.h>
static int valid_id(const char *s) { return s && s[0] && strlen(s)<CITADEL_ID_LENGTH; }
CitadelStatus citadel_registry_init(CitadelRegistry *r) {
 if (!r) return CITADEL_ERR_INVALID_ARGUMENT;
 memset(r,0,sizeof(*r));
 return CITADEL_OK;
}
CitadelStatus citadel_register_capability(CitadelRegistry *r,const CitadelCapability *d,CitadelCapabilityFn fn,void *u) {
 uint32_t i;
 if (!r||!d||!fn||!valid_id(d->id)) return CITADEL_ERR_INVALID_ARGUMENT;
 if (r->capability_count>=CITADEL_MAX_CAPABILITIES) return CITADEL_ERR_LIMIT;
 for (i=0;i<r->capability_count;i++) if (!strcmp(r->capabilities[i].descriptor.id,d->id)) return CITADEL_ERR_INVALID_ARGUMENT;
 r->capabilities[r->capability_count].descriptor=*d;
 r->capabilities[r->capability_count].execute=fn;
 r->capabilities[r->capability_count].user_data=u;
 r->capability_count++;
 return CITADEL_OK;
}
CitadelStatus citadel_register_intent(CitadelRegistry *r,const char *id,const char *phrase,const char *cap) {
 uint32_t i; size_t n;
 if (!r||!valid_id(id)||!valid_id(cap)||!phrase) return CITADEL_ERR_INVALID_ARGUMENT;
 n=strlen(phrase);
 if (!n||n>=CITADEL_MAX_TOKEN_LENGTH) return CITADEL_ERR_LIMIT;
 if (r->intent_count>=CITADEL_MAX_INTENTS) return CITADEL_ERR_LIMIT;
 for (i=0;i<r->intent_count;i++) if (!strcmp(r->intents[i].phrase,phrase)||!strcmp(r->intents[i].id,id)) return CITADEL_ERR_INVALID_ARGUMENT;
 memset(&r->intents[r->intent_count],0,sizeof(CitadelIntent));
 strcpy(r->intents[r->intent_count].id,id);
 strcpy(r->intents[r->intent_count].phrase,phrase);
 strcpy(r->intents[r->intent_count].capability_id,cap);
 r->intent_count++;
 return CITADEL_OK;
}
