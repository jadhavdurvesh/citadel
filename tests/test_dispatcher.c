#include "test.h"
#include "citadel/citadel.h"
#include <string.h>
static CitadelStatus run(const CitadelCommand *c,CitadelResult *o,void *u) {
 (void)c; (void)u; strcpy(o->text,"executed"); return CITADEL_OK;
}
int test_dispatcher(void) {
 CitadelRegistry r; CitadelCapability d; CitadelPolicy p; CitadelCommand c; CitadelResult o;
 memset(&d,0,sizeof(d)); strcpy(d.id,"sys.change");
 d.required_permissions=CITADEL_PERMISSION_MODIFY_SETTINGS; d.requires_confirmation=1;
 CHECK(citadel_registry_init(&r)==CITADEL_OK);
 CHECK(citadel_register_capability(&r,&d,run,NULL)==CITADEL_OK);
 CHECK(citadel_register_intent(&r,"intent.change","change setting","sys.change")==CITADEL_OK);
 CHECK(citadel_parse("change setting",15,&c)==CITADEL_OK);
 p.granted_permissions=CITADEL_PERMISSION_MODIFY_SETTINGS; p.confirmation_granted=0;
 CHECK(citadel_dispatch(&r,&c,&p,&o)==CITADEL_ERR_DENIED);
 p.confirmation_granted=1;
 CHECK(citadel_dispatch(&r,&c,&p,&o)==CITADEL_OK);
 CHECK(!strcmp(o.text,"executed"));
 PASS();
}
