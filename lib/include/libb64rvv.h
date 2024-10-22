#ifndef LIBB64RVV_H
#define LIBB64RVV_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "riscv_vector.h"

    void base64_encode_rvv_m4(uint8_t *input, uint8_t *output, size_t length);
    void base64_encode_rvv_m2(uint8_t *input, uint8_t *output, size_t length);
    void base64_encode_rvv_m1(uint8_t *input, uint8_t *output, size_t length);
    size_t base64_decode_rvv_m1(const char *data, int8_t *output, size_t input_length);
    size_t base64_decode_rvv_m2(const char *data, int8_t *output, size_t input_length);
    size_t base64_decode_rvv_m4(const char *data, int8_t *output, size_t input_length);

    size_t base64_encoded_length(size_t len);

    size_t b64_decode_rvv(const char *src, char *dst, size_t length);

#ifdef __cplusplus
}
#endif
#endif