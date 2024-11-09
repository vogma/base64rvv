#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <riscv_vector.h>

int Base64encode(char *encoded, const char *string, int len);
unsigned int base64_decode_tail(const char *in, unsigned int inlen, unsigned char *out);

static const uint8_t index_decode[66] = {
    2, 1, 0, 6, 5, 4, 10, 9, 8, 14, 13, 12, 18, 17, 16, 22, 21, 20,
    26, 25, 24, 30, 29, 28, 34, 33, 32, 38, 37, 36, 42, 41, 40,
    46, 45, 44, 50, 49, 48, 54, 53, 52, 58, 57, 56, 62, 61, 60,
    66, 65, 64, 70, 69, 68, 74, 73, 72, 78, 77, 76, 82, 81, 80,
    86, 85, 84};

static const int8_t shift_lut[16] = {
    /* 0 */ 0x00, /* 1 */ 0x00, /* 2 */ 0x3e - 0x2b, /* 3 */ 0x34 - 0x30,
    /* 4 */ 0x00 - 0x41, /* 5 */ 0x0f - 0x50, /* 6 */ 0x1a - 0x61, /* 7 */ 0x29 - 0x70,
    /* 8 */ 0x00, /* 9 */ 0x00, /* a */ 0x00, /* b */ 0x00,
    /* c */ 0x00, /* d */ 0x00, /* e */ 0x00, /* f */ 0x00};

static const int8_t offsets[14] = {71, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -19, -16, 65};

vuint8m1_t createGatherIndexEncode(size_t vl);
vuint8m1_t createGatherIndexDecode(size_t vl);

vuint8m1_t createDecodeIndices(size_t vl);

void printRegister(vuint8m4_t vec);