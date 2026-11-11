#include "citadel/octet.h"
#include <string.h>

void citadel_octet_write_u16(uint8_t out[2], uint16_t v) {
    out[0]=(uint8_t)(v>>8); out[1]=(uint8_t)v;
}
void citadel_octet_write_u32(uint8_t out[4], uint32_t v) {
    out[0]=(uint8_t)(v>>24); out[1]=(uint8_t)(v>>16);
    out[2]=(uint8_t)(v>>8); out[3]=(uint8_t)v;
}
void citadel_octet_write_u64(uint8_t out[8], uint64_t v) {
    unsigned i; for (i=0;i<8;i++) out[i]=(uint8_t)(v>>(56u-8u*i));
}
uint16_t citadel_octet_read_u16(const uint8_t in[2]) {
    return (uint16_t)(((uint16_t)in[0]<<8)|in[1]);
}
uint32_t citadel_octet_read_u32(const uint8_t in[4]) {
    return ((uint32_t)in[0]<<24)|((uint32_t)in[1]<<16)|((uint32_t)in[2]<<8)|(uint32_t)in[3];
}
uint64_t citadel_octet_read_u64(const uint8_t in[8]) {
    uint64_t v=0; unsigned i; for(i=0;i<8;i++) v=(v<<8)|in[i]; return v;
}

CitadelOctetStatus citadel_octet_encode(uint8_t flags, const uint8_t *payload,
    size_t payload_length, uint8_t *output, size_t capacity, size_t *written) {
    size_t total;
    if (!output || !written || (!payload && payload_length)) return CITADEL_OCTET_INVALID_ARGUMENT;
    if (payload_length>CITADEL_OCTET_MAX_PAYLOAD || payload_length>UINT32_MAX)
        return CITADEL_OCTET_PAYLOAD_TOO_LARGE;
    total=CITADEL_OCTET_HEADER_SIZE+payload_length;
    if (capacity<total) return CITADEL_OCTET_OUTPUT_TOO_SMALL;
    memcpy(output,"OCT1",4);
    output[4]=CITADEL_OCTET_VERSION;
    output[5]=flags;
    citadel_octet_write_u32(output+6,(uint32_t)payload_length);
    if (payload_length) memcpy(output+CITADEL_OCTET_HEADER_SIZE,payload,payload_length);
    *written=total;
    return CITADEL_OCTET_OK;
}

CitadelOctetStatus citadel_octet_decode(const uint8_t *input, size_t length,
    size_t max_payload, CitadelOctetFrame *frame) {
    uint32_t payload_length;
    if (!input || !frame) return CITADEL_OCTET_INVALID_ARGUMENT;
    if (length<CITADEL_OCTET_HEADER_SIZE) return CITADEL_OCTET_INPUT_TOO_SHORT;
    if (memcmp(input,"OCT1",4)!=0) return CITADEL_OCTET_BAD_MAGIC;
    if (input[4]!=CITADEL_OCTET_VERSION) return CITADEL_OCTET_UNSUPPORTED_VERSION;
    payload_length=citadel_octet_read_u32(input+6);
    if ((size_t)payload_length>max_payload || (size_t)payload_length>CITADEL_OCTET_MAX_PAYLOAD)
        return CITADEL_OCTET_PAYLOAD_TOO_LARGE;
    if (length!=CITADEL_OCTET_HEADER_SIZE+(size_t)payload_length)
        return CITADEL_OCTET_LENGTH_MISMATCH;
    frame->version=input[4]; frame->flags=input[5];
    frame->payload=input+CITADEL_OCTET_HEADER_SIZE;
    frame->payload_length=payload_length;
    return CITADEL_OCTET_OK;
}

CitadelOctetStatus citadel_octet_append_field(uint16_t id, uint8_t wire_type,
    const uint8_t *value, size_t value_length, uint8_t *output,
    size_t capacity, size_t *written) {
    size_t total=CITADEL_OCTET_FIELD_HEADER_SIZE+value_length;
    if (!output || !written || (!value && value_length)) return CITADEL_OCTET_INVALID_ARGUMENT;
    if (!id) return CITADEL_OCTET_INVALID_FIELD_ID;
    if (wire_type>3u) return CITADEL_OCTET_INVALID_WIRE_TYPE;
    if (value_length>UINT32_MAX) return CITADEL_OCTET_PAYLOAD_TOO_LARGE;
    if (capacity<total) return CITADEL_OCTET_OUTPUT_TOO_SMALL;
    citadel_octet_write_u16(output,id);
    output[2]=wire_type;
    citadel_octet_write_u32(output+3,(uint32_t)value_length);
    if (value_length) memcpy(output+7,value,value_length);
    *written=total;
    return CITADEL_OCTET_OK;
}

CitadelOctetStatus citadel_octet_read_field(const uint8_t *input,
    size_t length, CitadelOctetField *field, size_t *consumed) {
    uint32_t value_length;
    if (!input || !field || !consumed) return CITADEL_OCTET_INVALID_ARGUMENT;
    if (length<CITADEL_OCTET_FIELD_HEADER_SIZE) return CITADEL_OCTET_TRUNCATED_FIELD;
    field->id=citadel_octet_read_u16(input);
    if (!field->id) return CITADEL_OCTET_INVALID_FIELD_ID;
    field->wire_type=input[2];
    if (field->wire_type>3u) return CITADEL_OCTET_INVALID_WIRE_TYPE;
    value_length=citadel_octet_read_u32(input+3);
    if ((size_t)value_length>length-CITADEL_OCTET_FIELD_HEADER_SIZE)
        return CITADEL_OCTET_TRUNCATED_FIELD;
    field->value=input+7;
    field->value_length=value_length;
    *consumed=7u+(size_t)value_length;
    return CITADEL_OCTET_OK;
}
