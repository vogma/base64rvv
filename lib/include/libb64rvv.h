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

    // static const uint8_t index_decode[32] = {2, 1, 0, 6, 5, 4, 10, 9, 8, 14, 13, 12, 18, 17, 16, 22, 21, 20, 26, 25, 24, 30, 29, 28, 15, 0, 0, 0, 0, 0, 0, 0};
    static const uint8_t index_decode[] = {2, 1, 0, 6, 5, 4, 10, 9, 8, 14, 13, 12, 18, 17, 16, 22, 21, 20, 26, 25, 24, 30, 29, 28, 34, 33, 32, 38, 37, 36, 42, 41, 40, 46, 45, 44, 50, 49, 48, 54, 53, 52, 58, 57, 56, 62, 61, 60, 66, 65, 64, 70, 69, 68, 74, 73, 72, 78, 77, 76, 82, 81, 80, 86, 85, 84, 90, 89, 88, 94, 93, 92, 98, 97, 96, 102, 101, 100, 106, 105, 104, 110, 109, 108, 114, 113, 112, 118, 117, 116, 122, 121, 120, 126, 125, 124};
    static const int8_t shift_lut[16] = {
        /* 0 */ 0x00, /* 1 */ 0x00, /* 2 */ 0x3e - 0x2b, /* 3 */ 0x34 - 0x30,
        /* 4 */ 0x00 - 0x41, /* 5 */ 0x0f - 0x50, /* 6 */ 0x1a - 0x61, /* 7 */ 0x29 - 0x70,
        /* 8 */ 0x00, /* 9 */ 0x00, /* a */ 0x00, /* b */ 0x00,
        /* c */ 0x00, /* d */ 0x00, /* e */ 0x00, /* f */ 0x00};

    static const uint8_t gather_index_lmul4[32] = {1, 0, 2, 1, 4, 3, 5, 4, 7, 6, 8, 7, 10, 9, 11, 10, 13, 12, 14, 13, 16, 15, 17, 16, 19, 18, 20, 19, 22, 21, 23, 22};

    static const uint8_t gather_index_lmul8[] = {1, 0, 2, 1, 4, 3, 5, 4, 7, 6, 8, 7, 10, 9, 11, 10, 13, 12, 14, 13, 16, 15, 17, 16, 19, 18, 20, 19, 22, 21, 23, 22, 25, 24, 26, 25, 28, 27, 29, 28, 31, 30, 32, 31, 34, 33, 35, 34, 37, 36, 38, 37, 40, 39, 41, 40, 43, 42, 44, 43, 46, 45, 47, 46, 49, 48, 50, 49, 52, 51, 53, 52, 55, 54, 56, 55, 58, 57, 59, 58, 61, 60, 62, 61};

    static const int8_t offsets[68] = {71, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -19, -16, 65, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    void base64_encode_rvv_m4(uint8_t *input, uint8_t *output, size_t length);
    void base64_encode_rvv_m2(uint8_t *input, uint8_t *output, size_t length);
    void base64_encode_rvv_m1(uint8_t *input, uint8_t *output, size_t length);
    size_t base64_decode_rvv_m1(const char *data, int8_t *output, size_t input_length);
    size_t base64_decode_rvv_m2(const char *data, int8_t *output, size_t input_length);
    size_t base64_decode_rvv_m4(const char *data, int8_t *output, size_t input_length);

    size_t b64_decode_rvv(const char *src, char *dst, size_t length);

#ifdef __cplusplus
}
#endif
#endif