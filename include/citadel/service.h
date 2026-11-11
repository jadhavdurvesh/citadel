#ifndef CITADEL_SERVICE_H
#define CITADEL_SERVICE_H
#include <stddef.h>
#include <stdint.h>
#include "contract.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CITADEL_MAX_SERVICE_OPERATIONS 32u
#define CITADEL_SERVICE_OUTPUT_MAX CITADEL_CONTRACT_MAX_BODY

typedef CitadelStatus (*CitadelServiceHandler)(
    const uint8_t *request_body, size_t request_length,
    uint8_t *response_body, size_t response_capacity,
    size_t *response_length, void *user_data);

typedef struct CitadelServiceOperation {
    uint16_t operation_id;
    uint32_t required_permissions;
    uint8_t requires_confirmation;
    CitadelServiceHandler handler;
    void *user_data;
} CitadelServiceOperation;

typedef struct CitadelServiceRegistry {
    uint32_t count;
    CitadelServiceOperation operations[CITADEL_MAX_SERVICE_OPERATIONS];
} CitadelServiceRegistry;

typedef struct CitadelServicePolicy {
    uint32_t granted_permissions;
    uint8_t confirmation_granted;
} CitadelServicePolicy;

CitadelStatus citadel_service_registry_init(CitadelServiceRegistry *registry);
CitadelStatus citadel_service_register(CitadelServiceRegistry *registry,
    uint16_t operation_id, uint32_t required_permissions,
    uint8_t requires_confirmation, CitadelServiceHandler handler,
    void *user_data);

/* Decode -> resolve local allow-list -> enforce local policy -> execute -> encode.
 * Request permission bits are metadata only and never grant authority. */
CitadelStatus citadel_service_dispatch_wire(const CitadelServiceRegistry *registry,
    const CitadelServicePolicy *policy, const uint8_t *request_wire,
    size_t request_wire_length, uint8_t *response_wire,
    size_t response_capacity, size_t *response_wire_length);

#ifdef __cplusplus
}
#endif
#endif
