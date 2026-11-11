#include "test.h"
#include "citadel/provider.h"
#include <string.h>
static CitadelStatus fake_generate(void *x,const CitadelProviderRequest *q,CitadelProviderResponse *r) {
 (void)x; (void)q; (void)r; return CITADEL_OK;
}
int test_provider(void) {
 CitadelProviderV1 p; memset(&p,0,sizeof(p));
 p.descriptor.abi_version=CITADEL_PROVIDER_ABI_VERSION;
 p.descriptor.struct_size=(uint32_t)sizeof(p.descriptor);
 strcpy(p.descriptor.id,"mock.inert"); p.generate=fake_generate;
 CHECK(citadel_provider_validate(&p)==CITADEL_OK);
 p.descriptor.abi_version++;
 CHECK(citadel_provider_validate(&p)==CITADEL_ERR_ABI_MISMATCH);
 PASS();
}
