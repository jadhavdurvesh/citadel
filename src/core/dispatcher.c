#include "citadel/citadel.h"
#include <string.h>
static int matches(const CitadelCommand *c,const char *p) {
 size_t i=0; uint32_t t=0; size_t n=strlen(p);
 while (i<n) {
  size_t s,l;
  while (i<n&&p[i]==' ') i++;
  if (i==n) break;
  s=i; while (i<n&&p[i]!=' ') i++; l=i-s;
  if (t>=c->token_count||c->tokens[t].length!=l||memcmp(c->tokens[t].data,p+s,l)) return 0;
  t++;
 }
 return t>0 && t<=c->token_count;
}
CitadelStatus citadel_dispatch(const CitadelRegistry *r,const CitadelCommand *c,const CitadelPolicy *p,CitadelResult *o) {
 uint32_t i,j;
 if (!r||!c||!p||!o) return CITADEL_ERR_INVALID_ARGUMENT;
 memset(o,0,sizeof(*o));
 for (i=0;i<r->intent_count;i++) {
  if (matches(c,r->intents[i].phrase)) {
   for (j=0;j<r->capability_count;j++) {
    const CitadelCapability *d=&r->capabilities[j].descriptor;
    if (!strcmp(d->id,r->intents[i].capability_id)) {
     if ((d->required_permissions&p->granted_permissions)!=d->required_permissions ||
         (d->requires_confirmation&&!p->confirmation_granted)) { o->status=CITADEL_ERR_DENIED; return o->status; }
     o->status=r->capabilities[j].execute(c,o,r->capabilities[j].user_data);
     return o->status;
    }
   }
   o->status=CITADEL_ERR_UNAVAILABLE; return o->status;
  }
 }
 o->status=CITADEL_ERR_NOT_FOUND;
 return o->status;
}
