#ifndef CITADEL_SAND_VEYRA_H
#define CITADEL_SAND_VEYRA_H

#include <stddef.h>
#include <stdint.h>
#include "citadel.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SandOS Veyra compatibility boundary.
 *
 * This header intentionally mirrors only the published Veyra descriptor
 * prefix. It does not import kernel-private headers or claim that a service
 * transport/syscall ABI exists. Callers must first ensure descriptor points
 * to readable memory (for example, a copied kernel-owned descriptor).
 */
#define CITADEL_VEYRA_MAGIC             0x56594153u
#define CITADEL_VEYRA_ABI_MAJOR         1u
#define CITADEL_VEYRA_DESCRIPTOR_MIN    16u

#define CITADEL_VEYRA_IFACE_OPTIONAL     0x00000001u
#define CITADEL_VEYRA_IFACE_EXPERIMENTAL 0x00000002u

#define CITADEL_VEYRA_CAP_READ       0x00000001u
#define CITADEL_VEYRA_CAP_WRITE      0x00000002u
#define CITADEL_VEYRA_CAP_CONTROL    0x00000004u
#define CITADEL_VEYRA_CAP_ENUMERATE  0x00000008u

typedef struct CitadelVeyraDescriptor {
    uint32_t magic;
    uint16_t abi_major;
    uint16_t abi_minor;
    uint32_t struct_size;
    uint32_t flags;
} CitadelVeyraDescriptor;

typedef struct CitadelVeyraInterfaceInfo {
    CitadelVeyraDescriptor header;
    uint32_t interface_id;
    uint32_t capabilities;
} CitadelVeyraInterfaceInfo;

/* Validate a descriptor already copied into readable caller-owned memory. */
CitadelStatus citadel_veyra_validate_descriptor(
    const void *descriptor, size_t available_bytes, size_t minimum_size);

/* Map a Veyra status value to Citadel's portable status domain. */
CitadelStatus citadel_veyra_map_status(int32_t veyra_status);

#ifdef __cplusplus
}
#endif
#endif
