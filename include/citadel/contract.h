#ifndef CITADEL_CONTRACT_H
#define CITADEL_CONTRACT_H
#include <stddef.h>
#include <stdint.h>
#include "citadel.h"
#include "octet.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Citadel's bounded local service contract, following CONTRACT's
 * transport-independent Request/Response boundary. It is not the Rust
 * CONTRACT crate or its HTTP/RPC server. */
#define CITADEL_CONTRACT_MAX_BODY 1024u
#define CITADEL_CONTRACT_REQUEST_TYPE 1u
#define CITADEL_CONTRACT_RESPONSE_TYPE 2u
#define CITADEL_CONTRACT_WIRE_BYTES 1200u

typedef struct CitadelContractRequest {
    uint64_t request_id;
    uint16_t operation_id;
    uint32_t required_permissions;
    const uint8_t *body;
    size_t body_length;
} CitadelContractRequest;

typedef struct CitadelContractResponse {
    uint64_t request_id;
    int32_t status;
    const uint8_t *body;
    size_t body_length;
} CitadelContractResponse;

CitadelStatus citadel_contract_encode_request(const CitadelContractRequest *,
    uint8_t *output, size_t capacity, size_t *written);
CitadelStatus citadel_contract_decode_request(const uint8_t *, size_t,
    CitadelContractRequest *, uint8_t *body_storage, size_t body_capacity);
CitadelStatus citadel_contract_encode_response(const CitadelContractResponse *,
    uint8_t *output, size_t capacity, size_t *written);
CitadelStatus citadel_contract_decode_response(const uint8_t *, size_t,
    CitadelContractResponse *, uint8_t *body_storage, size_t body_capacity);

#ifdef __cplusplus
}
#endif
#endif
