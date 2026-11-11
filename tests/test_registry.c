#include "test.h"
#include "citadel/citadel.h"
#include <string.h>
static CitadelStatus noop(const CitadelCommand *c,CitadelResult *o,void *u) {
 (void)c; (void)u; strcpy(o->text,"ok"); return CITADEL_OK;
}
int test_registry(void) {
 CitadelRegistry r; CitadelCapability d; memset(&d,0,sizeof(d)); strcpy(d.id,"test.noop");
 CHECK(citadel_registry_init(&r)==CITADEL_OK);
 CHECK(citadel_register_capability(&r,&d,noop,NULL)==CITADEL_OK);
 CHECK(citadel_register_capability(&r,&d,noop,NULL)==CITADEL_ERR_INVALID_ARGUMENT);
 CHECK(citadel_register_intent(&r,"intent.one","noop","test.noop")==CITADEL_OK);
 CHECK(citadel_register_intent(&r,"intent.two","noop","test.noop")==CITADEL_ERR_INVALID_ARGUMENT);
 PASS();
}
