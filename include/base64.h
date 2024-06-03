#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "riscv_vector.h"
#include <time.h>
#include <b64/cencode.h>
#include <b64/cdecode.h>

void base64_encode(uint8_t *restrict input, uint8_t *output, size_t length);

void base64_decode_rvv(const char *data, int8_t *output, size_t input_length, size_t *output_length);


void hello(void);

int64_t timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
    return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}