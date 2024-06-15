#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "riscv_vector.h"

int main(void)
{

    size_t vl;

    int8_t a[8] = {0x11, 0x06, 0x05, 0x33, 0x1A, 0x17, 0x0D, 0x34};

    int8_t mult_constants[8] = {0x01, 0x40, 0x01, 0x40, 0x01, 0x40, 0x01, 0x40};
    // uint8_t a[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    int8_t b[8] = {1, 2, 3, 4, 5, 6, 7, 8};

    int8_t index[8] = {0, 2, 4, 6, 0xFF, 0xFF, 0xFF, 0xFF};
    int8_t c[8];
    int32_t c32[2];

    size_t vlmax_8 = __riscv_vsetvlmax_e8m2();
    // size_t vlmax_32 = __riscv_vsetvlmax_e32m1();

    // vint32m1_t merge = __riscv_vmv_v_x_i32m1(0x01400140, vlmax_32);

    vint8m2_t vec_a = __riscv_vle8_v_i8m2(&a[0], vlmax_8);

    vint8m1_t reduction = __riscv_vget_v_i8m2_i8m1(vec_a, 1);
    // vint8m1_t vec_mult_constants = __riscv_vle8_v_i8m1(&mult_constants[0], vlmax_8);
    // vint8m1_t vec_b = __riscv_vle8_v_i8m1(&b[0], vlmax_8);
    // vint8m1_t index_vec = __riscv_vle8_v_i8m1(&index[0], vlmax_8);

    // vl = __riscv_vsetvl_e32m1(16);

    // widening multiply
    // vint16m2_t widened_multiply = __riscv_vwmul_vv_i16m2(vec_a, vec_mult_constants, vlmax_8);

    //-> slidedown and then multiply horizontally
    //-> vrgather of every second result
    //-> reduce to e16m1
    //-> multiply with second constant -> widening to 32bit
    //-> again slidedown and horizontally multiplying
    //-> vrgather
    //-> reduce to 32m1?

    // vec_a = __riscv_vmul_vv_u32m1(vec_a, vec_b, vl);

    // vuint8m1_t vec_a_slide = __riscv_vslidedown_vx_u8m1(vec_a, 1, vlmax_8);
    // // vec_a = __riscv_vslideup_vx_u8m1(vec_a, vec_a, 2, vlmax_8);
    // // vec_a = __riscv_vslidedown_vx_u8m1(vec_b, 1, vlmax_8);
    // vec_a = __riscv_vadd_vv_u8m1(vec_a, vec_a_slide, vlmax_8);

    // // vuint8m1_t result = __riscv_vnsrl_wx_u8m1(vec_a, 0, vlmax_8);

    // vec_a = __riscv_vrgather_vv_u8m1(vec_a, index_vec, vlmax_8);

    __riscv_vse8_v_i8m1(&c[0], reduction, vlmax_8);

    for (int i = 0; i < 8; i++)
    {
        printf("%d ", c[i]);
    }
    printf("\n");

    // for (int i = 0; i < 2; i++)
    // {
    //     printf("0x%08X ", c32[i]);
    // }
    // printf("\n");

    return 0;
}
