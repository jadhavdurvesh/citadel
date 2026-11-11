#include "test.h"
#include "citadel/service.h"
#include <string.h>

static int executions;
static CitadelStatus echo_handler(const uint8_t *in,size_t in_len,uint8_t *out,
    size_t cap,size_t *written,void *user_data) {
    (void)user_data;
    executions++;
    if(in_len>cap) return CITADEL_ERR_LIMIT;
    if(in_len) memcpy(out,in,in_len);
    *written=in_len;
    return CITADEL_OK;
}
static CitadelStatus call(CitadelServiceRegistry *registry,CitadelServicePolicy *policy,
    uint16_t op,uint32_t claimed,uint8_t confirm,CitadelStatus expected) {
    uint8_t request_wire[CITADEL_CONTRACT_WIRE_BYTES];
    uint8_t response_wire[CITADEL_CONTRACT_WIRE_BYTES];
    uint8_t response_body[CITADEL_CONTRACT_MAX_BODY];
    const uint8_t body[]="status";
    size_t req_len=0,res_len=0;
    CitadelContractRequest request={1234,op,claimed,body,sizeof(body)-1};
    CitadelContractResponse response;
    CitadelStatus s;
    policy->confirmation_granted=confirm;
    s=citadel_contract_encode_request(&request,request_wire,sizeof(request_wire),&req_len);
    if(s!=CITADEL_OK)return s;
    s=citadel_service_dispatch_wire(registry,policy,request_wire,req_len,
        response_wire,sizeof(response_wire),&res_len);
    if(s!=CITADEL_OK)return s;
    s=citadel_contract_decode_response(response_wire,res_len,&response,
        response_body,sizeof(response_body));
    if(s!=CITADEL_OK)return s;
    if(response.request_id!=1234 || response.status!=(int32_t)expected)return CITADEL_ERR_INTERNAL;
    if(expected==CITADEL_OK &&
       (response.body_length!=sizeof(body)-1 || memcmp(response.body,body,sizeof(body)-1)))
        return CITADEL_ERR_INTERNAL;
    return CITADEL_OK;
}
int test_service(void) {
    CitadelServiceRegistry registry;
    CitadelServicePolicy policy={CITADEL_PERMISSION_READ_SYSTEM,0};
    executions=0;
    CHECK(citadel_service_registry_init(&registry)==CITADEL_OK);
    CHECK(citadel_service_register(&registry,7,CITADEL_PERMISSION_READ_SYSTEM,0,
        echo_handler,NULL)==CITADEL_OK);
    CHECK(citadel_service_register(&registry,8,CITADEL_PERMISSION_MODIFY_SETTINGS,1,
        echo_handler,NULL)==CITADEL_OK);
    CHECK(citadel_service_register(&registry,7,0,0,echo_handler,NULL)==CITADEL_ERR_INVALID_ARGUMENT);
    /* Forged wire permission bits cannot elevate the caller. */
    CHECK(call(&registry,&policy,8,0,0,CITADEL_ERR_DENIED)==CITADEL_OK);
    CHECK(executions==0);
    CHECK(call(&registry,&policy,7,UINT32_MAX,0,CITADEL_OK)==CITADEL_OK);
    CHECK(executions==1);
    CHECK(call(&registry,&policy,99,0,0,CITADEL_ERR_NOT_FOUND)==CITADEL_OK);
    CHECK(executions==1);
    policy.granted_permissions|=CITADEL_PERMISSION_MODIFY_SETTINGS;
    CHECK(call(&registry,&policy,8,0,0,CITADEL_ERR_DENIED)==CITADEL_OK);
    CHECK(call(&registry,&policy,8,0,1,CITADEL_OK)==CITADEL_OK);
    CHECK(executions==2);
    PASS();
}
