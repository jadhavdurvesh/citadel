#ifndef CITADEL_OCTET_H
#define CITADEL_OCTET_H
#include <stddef.h>
#include <stdint.h>
#include "citadel.h"

#ifdef __cplusplus
extern "C" {
#endif

/* OCTET draft 0.2 / frame v1. Compatible with DMJ OCTET's 10-byte OCT1 envelope. */
#define CITADEL_OCTET_HEADER_SIZE 10u
#define CITADEL_OCTET_VERSION 1u
#define CITADEL_OCTET_MAX_PAYLOAD 4096u
#define CITADEL_OCTET_FIELD_HEADER_SIZE 7u

typedef enum CitadelOctetStatus {
    CITADEL_OCTET_OK = 0,
    CITADEL_OCTET_INVALID_ARGUMENT = -20,
    CITADEL_OCTET_OUTPUT_TOO_SMALL = -21,
    CITADEL_OCTET_INPUT_TOO_SHORT = -22,
    CITADEL_OCTET_BAD_MAGIC = -23,
    CITADEL_OCTET_UNSUPPORTED_VERSION = -24,
    CITADEL_OCTET_LENGTH_MISMATCH = -25,
    CITADEL_OCTET_PAYLOAD_TOO_LARGE = -26,
    CITADEL_OCTET_INVALID_FIELD_ID = -27,
    CITADEL_OCTET_INVALID_WIRE_TYPE = -28,
    CITADEL_OCTET_TRUNCATED_FIELD = -29
} CitadelOctetStatus;

typedef struct CitadelOctetFrame {
    uint8_t version;
    uint8_t flags;
    const uint8_t *payload;
    size_t payload_length;
} CitadelOctetFrame;

typedef struct CitadelOctetField {
    uint16_t id;
    uint8_t wire_type;
    const uint8_t *value;
    size_t value_length;
} CitadelOctetField;

CitadelOctetStatus citadel_octet_encode(uint8_t flags, const uint8_t *payload,
    size_t payload_length, uint8_t *output, size_t output_capacity,
    size_t *written);
CitadelOctetStatus citadel_octet_decode(const uint8_t *input, size_t input_length,
    size_t max_payload, CitadelOctetFrame *frame);
CitadelOctetStatus citadel_octet_append_field(uint16_t id, uint8_t wire_type,
    const uint8_t *value, size_t value_length, uint8_t *output,
    size_t output_capacity, size_t *written);
CitadelOctetStatus citadel_octet_read_field(const uint8_t *input,
    size_t input_length, CitadelOctetField *field, size_t *consumed);
void citadel_octet_write_u16(uint8_t output[2], uint16_t value);
void citadel_octet_write_u32(uint8_t output[4], uint32_t value);
void citadel_octet_write_u64(uint8_t output[8], uint64_t value);
uint16_t citadel_octet_read_u16(const uint8_t input[2]);
uint32_t citadel_octet_read_u32(const uint8_t input[4]);
uint64_t citadel_octet_read_u64(const uint8_t input[8]);

#ifdef __cplusplus
}
#endif
#endif
