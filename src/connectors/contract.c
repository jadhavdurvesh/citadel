#include "citadel/contract.h"
#include <string.h>

enum { FIELD_REQUEST_ID=1, FIELD_OPERATION_ID=2, FIELD_PERMISSIONS=3,
       FIELD_BODY=4, FIELD_STATUS=5 };
enum { WIRE_U16=1, WIRE_U32=2, WIRE_U64=3, WIRE_BYTES=0 };

static CitadelStatus map_octet(CitadelOctetStatus s) {
    return s==CITADEL_OCTET_OK ? CITADEL_OK :
           s==CITADEL_OCTET_OUTPUT_TOO_SMALL ? CITADEL_ERR_BUFFER_TOO_SMALL :
           s==CITADEL_OCTET_PAYLOAD_TOO_LARGE ? CITADEL_ERR_LIMIT :
           CITADEL_ERR_INVALID_ARGUMENT;
}
static CitadelStatus append(uint8_t *payload,size_t cap,size_t *offset,
    uint16_t id,uint8_t type,const uint8_t *value,size_t len) {
    size_t n=0; CitadelOctetStatus s;
    if (*offset>cap) return CITADEL_ERR_BUFFER_TOO_SMALL;
    s=citadel_octet_append_field(id,type,value,len,payload+*offset,cap-*offset,&n);
    if (s!=CITADEL_OCTET_OK) return map_octet(s);
    *offset+=n; return CITADEL_OK;
}
static CitadelStatus wrap(uint8_t type,const uint8_t *payload,size_t payload_len,
    uint8_t *out,size_t cap,size_t *written) {
    uint8_t framed[CITADEL_CONTRACT_WIRE_BYTES];
    size_t n=0;
    if (payload_len+1u>sizeof(framed)) return CITADEL_ERR_LIMIT;
    framed[0]=type; if(payload_len) memcpy(framed+1,payload,payload_len);
    return map_octet(citadel_octet_encode(0,framed,payload_len+1u,out,cap,written));
}
static CitadelStatus unwrap(uint8_t expected,const uint8_t *input,size_t length,
    const uint8_t **payload,size_t *payload_len) {
    CitadelOctetFrame f;
    CitadelOctetStatus s=citadel_octet_decode(input,length,CITADEL_OCTET_MAX_PAYLOAD,&f);
    if(s!=CITADEL_OCTET_OK) return map_octet(s);
    if(f.payload_length<1u || f.payload[0]!=expected) return CITADEL_ERR_INVALID_ARGUMENT;
    *payload=f.payload+1; *payload_len=f.payload_length-1u; return CITADEL_OK;
}
static CitadelStatus next_field(const uint8_t **cursor,size_t *remaining,
    uint16_t expected,uint8_t type,size_t exact,CitadelOctetField *field) {
    size_t consumed=0;
    if(citadel_octet_read_field(*cursor,*remaining,field,&consumed)!=CITADEL_OCTET_OK)
        return CITADEL_ERR_INVALID_ARGUMENT;
    if(field->id!=expected || field->wire_type!=type ||
       (exact!=SIZE_MAX && field->value_length!=exact)) return CITADEL_ERR_INVALID_ARGUMENT;
    *cursor+=consumed; *remaining-=consumed; return CITADEL_OK;
}
CitadelStatus citadel_contract_encode_request(const CitadelContractRequest *r,
    uint8_t *out,size_t cap,size_t *written) {
    uint8_t p[CITADEL_CONTRACT_WIRE_BYTES]; uint8_t v8[8],v4[4],v2[2]; size_t n=0;
    CitadelStatus s;
    if(!r||!out||!written||(!r->body&&r->body_length)) return CITADEL_ERR_INVALID_ARGUMENT;
    if(r->body_length>CITADEL_CONTRACT_MAX_BODY) return CITADEL_ERR_LIMIT;
    citadel_octet_write_u64(v8,r->request_id);
    if((s=append(p,sizeof(p),&n,FIELD_REQUEST_ID,WIRE_U64,v8,8))!=CITADEL_OK)return s;
    citadel_octet_write_u16(v2,r->operation_id);
    if((s=append(p,sizeof(p),&n,FIELD_OPERATION_ID,WIRE_U16,v2,2))!=CITADEL_OK)return s;
    citadel_octet_write_u32(v4,r->required_permissions);
    if((s=append(p,sizeof(p),&n,FIELD_PERMISSIONS,WIRE_U32,v4,4))!=CITADEL_OK)return s;
    if((s=append(p,sizeof(p),&n,FIELD_BODY,WIRE_BYTES,r->body,r->body_length))!=CITADEL_OK)return s;
    return wrap(CITADEL_CONTRACT_REQUEST_TYPE,p,n,out,cap,written);
}
CitadelStatus citadel_contract_decode_request(const uint8_t *in,size_t len,
    CitadelContractRequest *r,uint8_t *storage,size_t storage_cap) {
    const uint8_t *p,*cur; size_t n,remain; CitadelOctetField f; CitadelStatus s;
    if(!r||!storage) return CITADEL_ERR_INVALID_ARGUMENT;
    if((s=unwrap(CITADEL_CONTRACT_REQUEST_TYPE,in,len,&p,&n))!=CITADEL_OK)return s;
    cur=p;remain=n;
    if((s=next_field(&cur,&remain,FIELD_REQUEST_ID,WIRE_U64,8,&f))!=CITADEL_OK)return s;
    r->request_id=citadel_octet_read_u64(f.value);
    if((s=next_field(&cur,&remain,FIELD_OPERATION_ID,WIRE_U16,2,&f))!=CITADEL_OK)return s;
    r->operation_id=citadel_octet_read_u16(f.value);
    if((s=next_field(&cur,&remain,FIELD_PERMISSIONS,WIRE_U32,4,&f))!=CITADEL_OK)return s;
    r->required_permissions=citadel_octet_read_u32(f.value);
    if((s=next_field(&cur,&remain,FIELD_BODY,WIRE_BYTES,SIZE_MAX,&f))!=CITADEL_OK)return s;
    if(f.value_length>CITADEL_CONTRACT_MAX_BODY || f.value_length>storage_cap)return CITADEL_ERR_LIMIT;
    if(remain!=0)return CITADEL_ERR_INVALID_ARGUMENT;
    if(f.value_length)memcpy(storage,f.value,f.value_length);
    r->body=storage;r->body_length=f.value_length;return CITADEL_OK;
}
CitadelStatus citadel_contract_encode_response(const CitadelContractResponse *r,
    uint8_t *out,size_t cap,size_t *written) {
    uint8_t p[CITADEL_CONTRACT_WIRE_BYTES]; uint8_t v8[8],v4[4]; size_t n=0; CitadelStatus s;
    if(!r||!out||!written||(!r->body&&r->body_length))return CITADEL_ERR_INVALID_ARGUMENT;
    if(r->body_length>CITADEL_CONTRACT_MAX_BODY)return CITADEL_ERR_LIMIT;
    citadel_octet_write_u64(v8,r->request_id);
    if((s=append(p,sizeof(p),&n,FIELD_REQUEST_ID,WIRE_U64,v8,8))!=CITADEL_OK)return s;
    citadel_octet_write_u32(v4,(uint32_t)r->status);
    if((s=append(p,sizeof(p),&n,FIELD_STATUS,WIRE_U32,v4,4))!=CITADEL_OK)return s;
    if((s=append(p,sizeof(p),&n,FIELD_BODY,WIRE_BYTES,r->body,r->body_length))!=CITADEL_OK)return s;
    return wrap(CITADEL_CONTRACT_RESPONSE_TYPE,p,n,out,cap,written);
}
CitadelStatus citadel_contract_decode_response(const uint8_t *in,size_t len,
    CitadelContractResponse *r,uint8_t *storage,size_t storage_cap) {
    const uint8_t *p,*cur; size_t n,remain; CitadelOctetField f; CitadelStatus s;
    if(!r||!storage)return CITADEL_ERR_INVALID_ARGUMENT;
    if((s=unwrap(CITADEL_CONTRACT_RESPONSE_TYPE,in,len,&p,&n))!=CITADEL_OK)return s;
    cur=p;remain=n;
    if((s=next_field(&cur,&remain,FIELD_REQUEST_ID,WIRE_U64,8,&f))!=CITADEL_OK)return s;
    r->request_id=citadel_octet_read_u64(f.value);
    if((s=next_field(&cur,&remain,FIELD_STATUS,WIRE_U32,4,&f))!=CITADEL_OK)return s;
    r->status=(int32_t)citadel_octet_read_u32(f.value);
    if((s=next_field(&cur,&remain,FIELD_BODY,WIRE_BYTES,SIZE_MAX,&f))!=CITADEL_OK)return s;
    if(f.value_length>CITADEL_CONTRACT_MAX_BODY||f.value_length>storage_cap)return CITADEL_ERR_LIMIT;
    if(remain!=0)return CITADEL_ERR_INVALID_ARGUMENT;
    if(f.value_length)memcpy(storage,f.value,f.value_length);
    r->body=storage;r->body_length=f.value_length;return CITADEL_OK;
}
