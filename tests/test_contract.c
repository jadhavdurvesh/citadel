#include "test.h"
#include "citadel/contract.h"
#include <string.h>
int test_contract(void) {
    uint8_t wire[CITADEL_CONTRACT_WIRE_BYTES],decoded[128];
    const uint8_t body[]={ 'h','e','l','l','o' };
    size_t written=0;
    CitadelContractRequest request={UINT64_C(0x0102030405060708),17,
        CITADEL_PERMISSION_READ_SYSTEM,body,sizeof(body)}, parsed;
    CitadelContractResponse response={request.request_id,0,body,sizeof(body)}, parsed_response;
    CHECK(citadel_contract_encode_request(&request,wire,sizeof(wire),&written)==CITADEL_OK);
    CHECK(citadel_contract_decode_request(wire,written,&parsed,decoded,sizeof(decoded))==CITADEL_OK);
    CHECK(parsed.request_id==request.request_id && parsed.operation_id==17);
    CHECK(parsed.required_permissions==CITADEL_PERMISSION_READ_SYSTEM);
    CHECK(parsed.body_length==sizeof(body) && memcmp(parsed.body,body,sizeof(body))==0);
    CHECK(citadel_contract_encode_response(&response,wire,sizeof(wire),&written)==CITADEL_OK);
    CHECK(citadel_contract_decode_response(wire,written,&parsed_response,decoded,sizeof(decoded))==CITADEL_OK);
    CHECK(parsed_response.request_id==response.request_id && parsed_response.status==0);
    CHECK(parsed_response.body_length==sizeof(body));
    PASS();
}
