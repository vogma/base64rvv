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
#include <time.h>

    // scalar code
    int Base64encode(char *encoded, const char *string, int len);
    unsigned int base64_decode_tail(const char *in, unsigned int inlen, unsigned char *out);

    // vectorized code
    void base64_encode_rvv(uint8_t *input, uint8_t *output, size_t length);
    size_t base64_decode_rvv_m1(const char *data, int8_t *output, size_t input_length);
    size_t base64_decode_rvv_m2(const char *data, int8_t *output, size_t input_length);
    size_t base64_decode_rvv_m4(const char *data, int8_t *output, size_t input_length);

    // size_t DecodeChunk(const char *in, size_t inLen,uint8_t *out);

    int Base64encode_len(int len);

#ifdef __cplusplus
}
#endif
#endif