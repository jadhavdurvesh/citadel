#include "citadel/catalog.h"
#include <stdio.h>
#include <string.h>
static int valid(const char *s,size_t cap) {
 size_t n=0;
 if(!s || !*s)return 0;
 while(n<cap && s[n]) {
  unsigned char c=(unsigned char)s[n];
  if(c<32 || c==127)return 0;
  n++;
 }
 return n>0 && n<cap;
}
CitadelStatus citadel_catalog_init(CitadelAppCatalog *c) {
 if(!c)return CITADEL_ERR_INVALID_ARGUMENT;
 memset(c,0,sizeof(*c));return CITADEL_OK;
}
const CitadelAppEntry *citadel_catalog_find(const CitadelAppCatalog *c,const char *id) {
 uint32_t i;
 if(!c||!id)return NULL;
 for(i=0;i<c->count && i<CITADEL_CATALOG_CAPACITY;i++)
  if(strcmp(c->entries[i].id,id)==0)return &c->entries[i];
 return NULL;
}
CitadelStatus citadel_catalog_add(CitadelAppCatalog *c,const char *id,const char *name) {
 CitadelAppEntry *e;
 if(!c||!valid(id,CITADEL_ID_LENGTH)||!valid(name,CITADEL_APP_NAME_MAX))return CITADEL_ERR_INVALID_ARGUMENT;
 if(citadel_catalog_find(c,id))return CITADEL_ERR_DENIED;
 if(c->count>=CITADEL_CATALOG_CAPACITY)return CITADEL_ERR_LIMIT;
 e=&c->entries[c->count++];
 strcpy(e->id,id);strcpy(e->name,name);
 return CITADEL_OK;
}
CitadelStatus citadel_catalog_format(const CitadelAppCatalog *c,char *out,size_t capacity) {
 size_t used=0;uint32_t i;int n;
 if(!c||!out||!capacity||c->count>CITADEL_CATALOG_CAPACITY)return CITADEL_ERR_INVALID_ARGUMENT;
 out[0]=0;
 if(!c->count) {
  n=snprintf(out,capacity,"No applications registered. The host does not scan or launch installed applications.");
  return n<0?CITADEL_ERR_INTERNAL:((size_t)n>=capacity?CITADEL_ERR_BUFFER_TOO_SMALL:CITADEL_OK);
 }
 n=snprintf(out,capacity,"Registered applications:");
 if(n<0)return CITADEL_ERR_INTERNAL;
 if((size_t)n>=capacity)return CITADEL_ERR_BUFFER_TOO_SMALL;
 used=(size_t)n;
 for(i=0;i<c->count;i++){
  n=snprintf(out+used,capacity-used," %s (%s);",c->entries[i].name,c->entries[i].id);
  if(n<0)return CITADEL_ERR_INTERNAL;
  if((size_t)n>=capacity-used)return CITADEL_ERR_BUFFER_TOO_SMALL;
  used+=(size_t)n;
 }
 return CITADEL_OK;
}
