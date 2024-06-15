#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "riscv_vector.h"

const int8_t mult_constants[32] = {
    0x01,
    0x40,
    0x01,
    0x40,
    0x01,
    0x40,
    0x01,
    0x40,
    0x01,
    0x40,
    0x01,
    0x40,
    0x01,
    0x40,
    0x01,
    0x40, 0x01, 0x40, 0x01, 0x40, 0x01, 0x40, 0x01, 0x40, 0x01, 0x40, 0x01, 0x40, 0x01, 0x40, 0x01, 0x40};

const int16_t mult16_constants[32] = {0x0001, 0x1000, 0x0001, 0x1000, 0x0001, 0x1000, 0x0001, 0x1000, 0x0001, 0x1000, 0x0001, 0x1000, 0x0001, 0x1000, 0x0001, 0x1000, 0x0001, 0x1000, 0x0001, 0x1000, 0x0001, 0x1000, 0x0001, 0x1000, 0x0001, 0x1000, 0x0001, 0x1000, 0x0001, 0x1000, 0x0001, 0x1000};

const uint16_t gather_index[36] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70};
const uint32_t gather32_index[8] = {0, 2, 4, 6, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};

vint32m1_t __attribute__((always_inline)) inline pack_data(vint8m1_t data, size_t vl)
{
    vint8m1_t vec_mult_constants = __riscv_vle8_v_i8m1(&mult_constants[0], vl);
    vint16m1_t vec_mult16_constants = __riscv_vle16_v_i16m1(&mult16_constants[0], vl);
    vuint16m2_t vec_gather_index = __riscv_vle16_v_u16m2(&gather_index[0], vl);

    size_t vlmax_32 = __riscv_vsetvlmax_e32m2();

    vuint32m2_t vec_gather_index_32 = __riscv_vle32_v_u32m2((uint32_t *)&gather32_index[0], vlmax_32);

    vint16m2_t widened_multiply = __riscv_vwmul_vv_i16m2(data, vec_mult_constants, vl);

    vint16m2_t widened_multiply_shifted = __riscv_vslidedown_vx_i16m2(widened_multiply, 1, vl);

    widened_multiply = __riscv_vadd_vv_i16m2(widened_multiply, widened_multiply_shifted, vl);

    widened_multiply = __riscv_vrgather_vv_i16m2(widened_multiply, vec_gather_index, vl);

    vint16m1_t widened_multiply_shortened = __riscv_vget_v_i16m2_i16m1(widened_multiply, 0);

    vl = __riscv_vsetvlmax_e16m1();

    vint32m2_t vec_mult = __riscv_vwmul_vv_i32m2(widened_multiply_shortened, vec_mult16_constants, vl);

    vint32m2_t vec_mult_shifted = __riscv_vslidedown_vx_i32m2(vec_mult, 1, vl);

    vec_mult = __riscv_vadd_vv_i32m2(vec_mult, vec_mult_shifted, vl);

    vec_mult = __riscv_vrgather_vv_i32m2(vec_mult, vec_gather_index_32, vl);

    return __riscv_vget_v_i32m2_i32m1(vec_mult, 0);
}

int main(void)
{

    int8_t a[8] = {0x11, 0x06, 0x05, 0x33, 0x1A, 0x17, 0x0D, 0x34};

    // uint8_t a[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    // int8_t b[8] = {1, 2, 3, 4, 5, 6, 7, 8};

    // int8_t index[8] = {0, 2, 4, 6, 0xFF, 0xFF, 0xFF, 0xFF};
    // int8_t c[8];
    int32_t c32[2];

    size_t vlmax_8 = __riscv_vsetvlmax_e8m2();
    // size_t vlmax_32 = __riscv_vsetvlmax_e32m1();

    // vint32m1_t merge = __riscv_vmv_v_x_i32m1(0x01400140, vlmax_32);

    vint8m1_t vec_a = __riscv_vle8_v_i8m1(&a[0], vlmax_8);

    vint32m1_t result = pack_data(vec_a, vlmax_8);

    // vint8m1_t reduction = __riscv_vget_v_i8m2_i8m1(vec_a, 1);
    // vint8m1_t vec_mult_constants = __riscv_vle8_v_i8m1(&mult_constants[0], vlmax_8);
    // // vint8m1_t vec_b = __riscv_vle8_v_i8m1(&b[0], vlmax_8);
    // vint8m1_t index_vec = __riscv_vle8_v_i8m1(&index[0], vlmax_8);

    // vl = __riscv_vsetvl_e32m1(16);

    // widening multiply

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

    __riscv_vse32_v_i32m1(&c32[0], result, vlmax_8);

    // for (int i = 0; i < 8; i++)
    // {
    //     printf("%d ", c[i]);
    // }
    // printf("\n");

    for (int i = 0; i < 2; i++)
    {
        printf("0x%08X ", c32[i]);
    }
    printf("\n");

    return 0;
}
