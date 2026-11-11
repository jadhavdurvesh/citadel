#include "citadel/service.h"
#include <string.h>

CitadelStatus citadel_service_registry_init(CitadelServiceRegistry *registry) {
    if (!registry) return CITADEL_ERR_INVALID_ARGUMENT;
    memset(registry, 0, sizeof(*registry));
    return CITADEL_OK;
}

CitadelStatus citadel_service_register(CitadelServiceRegistry *registry,
    uint16_t operation_id, uint32_t permissions, uint8_t confirmation,
    CitadelServiceHandler handler, void *user_data) {
    uint32_t i;
    if (!registry || !operation_id || !handler || confirmation > 1u)
        return CITADEL_ERR_INVALID_ARGUMENT;
    if (registry->count >= CITADEL_MAX_SERVICE_OPERATIONS) return CITADEL_ERR_LIMIT;
    for (i=0; i<registry->count; ++i)
        if (registry->operations[i].operation_id == operation_id)
            return CITADEL_ERR_INVALID_ARGUMENT;
    registry->operations[registry->count].operation_id=operation_id;
    registry->operations[registry->count].required_permissions=permissions;
    registry->operations[registry->count].requires_confirmation=confirmation;
    registry->operations[registry->count].handler=handler;
    registry->operations[registry->count].user_data=user_data;
    registry->count++;
    return CITADEL_OK;
}

static CitadelStatus encode_status(uint64_t request_id, CitadelStatus status,
    const uint8_t *body, size_t body_length, uint8_t *out, size_t capacity,
    size_t *written) {
    CitadelContractResponse response;
    response.request_id=request_id;
    response.status=(int32_t)status;
    response.body=body;
    response.body_length=body_length;
    return citadel_contract_encode_response(&response,out,capacity,written);
}

CitadelStatus citadel_service_dispatch_wire(const CitadelServiceRegistry *registry,
    const CitadelServicePolicy *policy, const uint8_t *request_wire,
    size_t request_wire_length, uint8_t *response_wire,
    size_t response_capacity, size_t *response_wire_length) {
    CitadelContractRequest request;
    uint8_t request_body[CITADEL_CONTRACT_MAX_BODY];
    uint8_t response_body[CITADEL_SERVICE_OUTPUT_MAX];
    size_t response_body_length=0;
    uint32_t i;
    CitadelStatus status;
    const CitadelServiceOperation *operation=NULL;

    if (!registry || !policy || !request_wire || !response_wire ||
        !response_wire_length) return CITADEL_ERR_INVALID_ARGUMENT;
    status=citadel_contract_decode_request(request_wire,request_wire_length,
        &request,request_body,sizeof(request_body));
    if (status!=CITADEL_OK) return status;

    for(i=0;i<registry->count;i++) {
        if(registry->operations[i].operation_id==request.operation_id) {
            operation=&registry->operations[i];
            break;
        }
    }
    if(!operation)
        return encode_status(request.request_id,CITADEL_ERR_NOT_FOUND,NULL,0,
            response_wire,response_capacity,response_wire_length);

    /* Trust only the local registration and local policy, never wire claims. */
    if((operation->required_permissions & policy->granted_permissions) !=
        operation->required_permissions ||
        (operation->requires_confirmation && !policy->confirmation_granted))
        return encode_status(request.request_id,CITADEL_ERR_DENIED,NULL,0,
            response_wire,response_capacity,response_wire_length);

    status=operation->handler(request.body,request.body_length,response_body,
        sizeof(response_body),&response_body_length,operation->user_data);
    if(response_body_length>sizeof(response_body))
        return CITADEL_ERR_LIMIT;
    return encode_status(request.request_id,status,response_body,response_body_length,
        response_wire,response_capacity,response_wire_length);
}
