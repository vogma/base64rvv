#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "riscv_vector.h"
#include <time.h>

const uint16_t gather_index_lmul4[] = {1, 0, 2, 1, 4, 3, 5, 4, 7, 6, 8, 7, 10, 9, 11, 10, 13, 12, 14, 13, 16, 15, 17, 16, 19, 18, 20, 19, 22, 21, 23, 22, 25, 24, 26, 25, 28, 27, 29, 28, 31, 30, 32, 31, 34, 33, 35, 34, 37, 36, 38, 37, 40, 39, 41, 40, 43, 42, 44, 43, 46, 45, 47, 46};
const int8_t offsets[16] = {65, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -19, -16, 71, 0, 0};

uint8_t alphabet[26] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

void base64_encode_asm(uint8_t *data, char *output, const int8_t *offsets, const uint16_t *index, int *length);

int Base64encode_len(int len)
{
    return ((len + 2) / 3 * 4) + 1;
}
#define N 1024 * 1024

uint8_t *setupInputData(uint8_t *inputData)
{
    char alphabet[26] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    for (int i = 0; i < N; i++)
    {
        inputData[i] = alphabet[i % 26];
    }

    return inputData;
}

int main(void)
{
    char *output = (char *)malloc(Base64encode_len(N));

    uint8_t *inputData = (uint8_t *)malloc(sizeof(uint8_t) * N);

    setupInputData(inputData);

    int length = N;

    base64_encode_asm(inputData, output, offsets, gather_index_lmul4, &length);

    printf("%d \n", length);

    for (int i = 0; i < 30; i++)
    {
        printf("%c", output[i]);
    }
    printf("\n");

    free(output);
    free(inputData);

    return 0;
}