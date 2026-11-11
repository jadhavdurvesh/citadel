#include "citadel/provider.h"
#include <string.h>
CitadelStatus citadel_provider_validate(const CitadelProviderV1 *p) {
 const CitadelProviderDescriptor *d;
 if (!p) return CITADEL_ERR_INVALID_ARGUMENT;
 d=&p->descriptor;
 if (d->abi_version!=CITADEL_PROVIDER_ABI_VERSION || d->struct_size<sizeof(*d)) return CITADEL_ERR_ABI_MISMATCH;
 if (!d->id[0] || !memchr(d->id,'\\0',sizeof(d->id)) || d->requires_network>1u || !p->generate) return CITADEL_ERR_INVALID_ARGUMENT;
 return CITADEL_OK;
}
