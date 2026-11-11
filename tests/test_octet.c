#include "test.h"
#include "citadel/octet.h"
#include <string.h>
int test_octet(void) {
    uint8_t wire[128],payload[8]={1,2,3,4,5,6,7,8};
    size_t written=0; CitadelOctetFrame frame;
    CHECK(citadel_octet_encode(3,payload,sizeof(payload),wire,sizeof(wire),&written)==CITADEL_OCTET_OK);
    CHECK(written==CITADEL_OCTET_HEADER_SIZE+sizeof(payload));
    CHECK(citadel_octet_decode(wire,written,sizeof(payload),&frame)==CITADEL_OCTET_OK);
    CHECK(frame.flags==3 && frame.payload_length==sizeof(payload));
    CHECK(memcmp(frame.payload,payload,sizeof(payload))==0);
    wire[0]='X';
    CHECK(citadel_octet_decode(wire,written,sizeof(payload),&frame)==CITADEL_OCTET_BAD_MAGIC);
    PASS();
}
