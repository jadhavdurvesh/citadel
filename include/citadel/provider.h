#ifndef CITADEL_PROVIDER_H
#define CITADEL_PROVIDER_H
#include <stddef.h>
#include <stdint.h>
#include "citadel.h"
#ifdef __cplusplus
extern "C" {
#endif
#define CITADEL_PROVIDER_ABI_VERSION 1u
#define CITADEL_PROVIDER_ID_MAX 48u
/* Dormant contract only: M1 never loads or initializes providers. */
typedef enum CitadelProviderKind { CITADEL_PROVIDER_LOCAL_MODEL=1, CITADEL_PROVIDER_REMOTE_MODEL=2, CITADEL_PROVIDER_RULE_ENGINE=3 } CitadelProviderKind;
typedef enum CitadelProviderCapability { CITADEL_PROVIDER_CAP_TEXT_GENERATION=1u<<0, CITADEL_PROVIDER_CAP_STREAMING=1u<<1, CITADEL_PROVIDER_CAP_TOOL_REQUESTS=1u<<2, CITADEL_PROVIDER_CAP_EMBEDDINGS=1u<<3 } CitadelProviderCapability;
typedef struct CitadelProviderRequest { uint32_t struct_size; uint32_t flags; const char *input; size_t input_length; uint32_t max_output_bytes; } CitadelProviderRequest;
typedef struct CitadelProviderResponse { uint32_t struct_size; CitadelStatus status; char *output; size_t output_capacity; size_t output_length; } CitadelProviderResponse;
typedef struct CitadelProviderDescriptor { uint32_t abi_version; uint32_t struct_size; char id[CITADEL_PROVIDER_ID_MAX]; CitadelProviderKind kind; uint32_t capabilities; uint32_t declared_memory_budget_mb; uint8_t requires_network; } CitadelProviderDescriptor;
typedef struct CitadelProviderV1 {
 CitadelProviderDescriptor descriptor; void *context;
 CitadelStatus (*initialize)(void *);
 CitadelStatus (*generate)(void *, const CitadelProviderRequest *, CitadelProviderResponse *);
 void (*shutdown)(void *);
} CitadelProviderV1;
CitadelStatus citadel_provider_validate(const CitadelProviderV1 *);
#ifdef __cplusplus
}
#endif
#endif
