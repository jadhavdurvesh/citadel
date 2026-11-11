#include "citadel/sand_veyra.h"
#include <string.h>

CitadelStatus citadel_veyra_validate_descriptor(
    const void *descriptor, size_t available_bytes, size_t minimum_size) {
    CitadelVeyraDescriptor header;

    if (descriptor == NULL) return CITADEL_ERR_INVALID_ARGUMENT;
    if (available_bytes < sizeof(header)) return CITADEL_ERR_ABI_MISMATCH;

    /* Copy the fixed prefix; do not assume alignment of the input pointer. */
    memcpy(&header, descriptor, sizeof(header));

    if (header.magic != CITADEL_VEYRA_MAGIC ||
        header.abi_major != CITADEL_VEYRA_ABI_MAJOR ||
        header.struct_size < CITADEL_VEYRA_DESCRIPTOR_MIN ||
        header.struct_size < minimum_size ||
        header.struct_size > available_bytes) {
        return CITADEL_ERR_ABI_MISMATCH;
    }

    /* Minor versions and unknown flag bits are additive/forward compatible. */
    return CITADEL_OK;
}

CitadelStatus citadel_veyra_map_status(int32_t status) {
    switch (status) {
    case 0:   return CITADEL_OK;
    case -1:  return CITADEL_ERR_INVALID_ARGUMENT;
    case -2:  return CITADEL_ERR_NOT_FOUND;
    case -3:  return CITADEL_ERR_INTERNAL; /* Veyra: no memory */
    case -4:  return CITADEL_ERR_UNAVAILABLE; /* busy */
    case -5:  return CITADEL_ERR_UNAVAILABLE;
    case -6:  return CITADEL_ERR_DENIED;
    case -7:  return CITADEL_ERR_UNAVAILABLE; /* I/O */
    case -8:  return CITADEL_ERR_ABI_MISMATCH;
    case -9:  return CITADEL_ERR_BUFFER_TOO_SMALL;
    case -10: return CITADEL_ERR_UNAVAILABLE; /* timeout */
    default:  return CITADEL_ERR_INTERNAL;
    }
}
