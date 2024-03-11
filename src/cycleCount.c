#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "riscv_vector.h"
#include <time.h>

const uint16_t gather_index_lmul4[] = {1, 0, 2, 1, 4, 3, 5, 4, 7, 6, 8, 7, 10, 9, 11, 10, 13, 12, 14, 13, 16, 15, 17, 16, 19, 18, 20, 19, 22, 21, 23, 22, 25, 24, 26, 25, 28, 27, 29, 28, 31, 30, 32, 31, 34, 33, 35, 34, 37, 36, 38, 37, 40, 39, 41, 40, 43, 42, 44, 43, 46, 45, 47, 46};
const int8_t offsets[16] = {65, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -19, -16, 71, 0, 0};

uint8_t alphabet[26] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

uint64_t cycleCount_asm(uint8_t *data, char *output, const int8_t *offsets, const uint16_t *index);

int main(void)
{
    char output[35];

    uint64_t cycles = cycleCount_asm(alphabet, output, offsets, gather_index_lmul4);

    for (int i = 0; i < 16; i++)
    {
        printf("0x%02x ", output[i]);
    }
    printf("\n");

    for (int i = 0; i < 16; i++)
    {
        printf("%c", output[i]);
    }
    printf("\ncycles vrgatherei16: %ld\n", cycles);

    return 0;
}