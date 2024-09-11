#include "riscv_vector.h"
#include "stdio.h"
#include "stdint.h"

int main(void)
{

    uint64_t data[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

    size_t vlmax_64 = 2;//__riscv_vsetvlmax_e64m2();

    vuint64m2_t vec = __riscv_vle64_v_u64m2(data, vlmax_64);

    // vuint8m1x3_t vec = __riscv_vlsseg3e8_v_u8m1x3(data, -1, vlmax_64);
    vuint64m1_t test = __riscv_vget_v_u64m2_u64m1(vec, 0);
    vuint64m1_t test1 = __riscv_vget_v_u64m2_u64m1(vec, 1);
    // vuint8m1_t test1 = __riscv_vget_v_u8m1x3_u8m1(vec, 1);
    // vuint8m1_t test2 = __riscv_vget_v_u8m1x3_u8m1(vec, 2);

    uint64_t result[12];
    uint64_t result1[12];
    uint8_t result2[12];

    // __riscv_vse8_v_u8m1(result, test, vlmax_8);
    __riscv_vse64_v_u64m1(result, test, vlmax_64);
    __riscv_vse64_v_u64m1(result1, test1, vlmax_64);
    // __riscv_vse8_v_u8m1(result1, test1, vlmax_8);
    // __riscv_vse8_v_u8m1(result2, test2, vlmax_8);

    for (int i = 0; i < 4; i++)
    {
        printf("%d ", result[i]);
    }
    printf("\n");
    for (int i = 0; i < 4; i++)
    {
        printf("%d ", result1[i]);
    }
    printf("\n");
    for (int i = 0; i < 4; i++)
    {
        printf("%d ", result2[i]);
    }
    printf("\n");

    return 0;
}