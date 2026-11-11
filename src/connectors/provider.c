#include "citadel/provider.h"
#include <string.h>

CitadelStatus citadel_provider_validate(const CitadelProviderV1 *provider) {
    const CitadelProviderDescriptor *descriptor;
    if (provider == NULL) return CITADEL_ERR_INVALID_ARGUMENT;
    descriptor = &provider->descriptor;
    if (descriptor->abi_version != CITADEL_PROVIDER_ABI_VERSION ||
        descriptor->struct_size < sizeof(CitadelProviderDescriptor)) {
        return CITADEL_ERR_ABI_MISMATCH;
    }
    if (descriptor->id[0] == '\0' ||
        memchr(descriptor->id, 0, sizeof(descriptor->id)) == NULL ||
        descriptor->requires_network > 1u ||
        provider->generate == NULL) {
        return CITADEL_ERR_INVALID_ARGUMENT;
    }
    return CITADEL_OK;
}
