#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "riscv_vector.h"
#include <time.h>

const unsigned char b64chars[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int Base64encode_len(int len)
{
    return ((len + 2) / 3 * 4) + 1;
}

int Base64encode(char *encoded, const char *string, int len)
{
    int i;
    char *p;

    p = encoded;
    for (i = 0; i < len - 2; i += 3)
    {
        *p++ = b64chars[(string[i] >> 2) & 0x3F];
        *p++ = b64chars[((string[i] & 0x3) << 4) |
                        ((int)(string[i + 1] & 0xF0) >> 4)];
        *p++ = b64chars[((string[i + 1] & 0xF) << 2) |
                        ((int)(string[i + 2] & 0xC0) >> 6)];
        *p++ = b64chars[string[i + 2] & 0x3F];
    }
    if (i < len)
    {
        *p++ = b64chars[(string[i] >> 2) & 0x3F];
        if (i == (len - 1))
        {
            *p++ = b64chars[((string[i] & 0x3) << 4)];
            *p++ = '=';
        }
        else
        {
            *p++ = b64chars[((string[i] & 0x3) << 4) |
                            ((int)(string[i + 1] & 0xF0) >> 4)];
            *p++ = b64chars[((string[i + 1] & 0xF) << 2)];
        }
        *p++ = '=';
    }

    *p++ = '\0';
    return p - encoded;
}

// const uint8_t gather_index[] = {2, 1, 0, 0xFF, 5, 4, 3, 0xFF, 8, 7, 6, 0xFF, 11, 10, 9, 0xFF};
// const uint8_t gather_index[] = {2, 1, 0, 2, 5, 4, 3, 5, 8, 7, 6, 8, 11, 10, 9, 11};
// const uint8_t gather_index_new[] = {1, 0, 2, 1, 4, 3, 5, 4, 7, 6, 8, 7, 10, 9, 11, 10};
// const uint8_t gather_index_lmul2[] = {1, 0, 2, 1, 4, 3, 5, 4, 7, 6, 8, 7, 10, 9, 11, 10, 13, 12, 14, 13, 16, 15, 17, 16, 19, 18, 20, 19, 22, 21, 23, 22};
const uint16_t gather_index_lmul4[] = {1, 0, 2, 1, 4, 3, 5, 4, 7, 6, 8, 7, 10, 9, 11, 10, 13, 12, 14, 13, 16, 15, 17, 16, 19, 18, 20, 19, 22, 21, 23, 22, 25, 24, 26, 25, 28, 27, 29, 28, 31, 30, 32, 31, 34, 33, 35, 34, 37, 36, 38, 37, 40, 39, 41, 40, 43, 42, 44, 43, 46, 45, 47, 46};
// const uint8_t gather_index_lmul8[] = {1, 0, 2, 1, 4, 3, 5, 4, 7, 6, 8, 7, 10, 9, 11, 10, 13, 12, 14, 13, 16, 15, 17, 16, 19, 18, 20, 19, 22, 21, 23, 22, 25, 24, 26, 25, 28, 27, 29, 28, 31, 30, 32, 31, 34, 33, 35, 34, 37, 36, 38, 37, 40, 39, 41, 40, 43, 42, 44, 43, 46, 45, 47, 46};

vuint16m2_t loadIndex()
{
    size_t vl = __riscv_vsetvlmax_e16m2();
    return __riscv_vle16_v_u16m2(gather_index_lmul4, vl);
    // return __riscv_vle8_v_u8m1(gather_index, vl);
}

const int8_t shift_26 = 65;
const int8_t shift_26_52 = 71;
const int8_t shift_52_62 = -4;
const int8_t shift_62 = -19;
const int8_t shift_63 = -16;

// vuint8m1_t base64_chars = branchless_table_lookup(__riscv_vreinterpret_v_u32m1_u8m1(vec_lookup_indices));
vuint8m1_t branchless_table_lookup_naive(vuint8m1_t vec_indices, size_t vl)
{

    // mask for values lower 26
    vbool8_t lt_26 = __riscv_vmsltu_vx_u8m1_b8(vec_indices, 26, vl);
    vec_indices = __riscv_vadd_vx_u8m1_m(lt_26, vec_indices, shift_26, vl);

    // mask for values lower or eq to 26 and smaller than 52
    vbool8_t eq_or_gt_26 = __riscv_vmnot_m_b8(lt_26, vl);
    vbool8_t lt_52 = __riscv_vmsltu_vx_u8m1_b8(vec_indices, 52, vl);
    vbool8_t bt_26_52 = __riscv_vmand_mm_b8(eq_or_gt_26, lt_52, vl);
    vec_indices = __riscv_vadd_vx_u8m1_m(bt_26_52, vec_indices, shift_26_52, vl);

    vbool8_t eq_or_gt_52 = __riscv_vmnot_m_b8(lt_52, vl);
    vbool8_t lt_62 = __riscv_vmsltu_vx_u8m1_b8(vec_indices, 62, vl);
    vbool8_t bt_52_62 = __riscv_vmand_mm_b8(eq_or_gt_52, lt_62, vl);
    vec_indices = __riscv_vadd_vx_u8m1_m(bt_52_62, vec_indices, shift_52_62, vl);

    vbool8_t eq_62 = __riscv_vmseq_vx_u8m1_b8(vec_indices, 62, vl);
    vec_indices = __riscv_vadd_vx_u8m1_m(eq_62, vec_indices, shift_62, vl);

    vbool8_t eq_63 = __riscv_vmseq_vx_u8m1_b8(vec_indices, 63, vl);
    vec_indices = __riscv_vadd_vx_u8m1_m(eq_63, vec_indices, shift_63, vl);

    return vec_indices;
}

const int8_t offsets[16] = {65, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -19, -16, 71, 0, 0};

vuint8m1_t __attribute__((always_inline)) inline branchless_table_lookup_opt(vuint8m1_t vec_indices, vint8m1_t offset_vec, size_t vl)
{
    vuint8m1_t result = __riscv_vssubu_vx_u8m1(vec_indices, 51, vl);

    vbool8_t ge_26_mask = __riscv_vmsgtu_vx_u8m1_b8(vec_indices, 25, vl);
    vbool8_t le_51_mask = __riscv_vmsleu_vx_u8m1_b8(vec_indices, 51, vl);

    vbool8_t mask = __riscv_vmand_mm_b8(ge_26_mask, le_51_mask, vl);

    result = __riscv_vmerge_vxm_u8m1(result, 13, mask, vl);

    offset_vec = __riscv_vrgather_vv_i8m1(offset_vec, result, vl);

    vint8m1_t ascii_vec = __riscv_vadd_vv_i8m1(__riscv_vreinterpret_v_u8m1_i8m1(vec_indices), offset_vec, vl);

    return __riscv_vreinterpret_v_i8m1_u8m1(ascii_vec);
    // return vec_indices;
}

vuint32m1_t create_lookup_indices_naive(vuint8m1_t data, size_t vl)
{

    vuint32m1_t input32 = __riscv_vreinterpret_v_u8m1_u32m1(data);
    vuint32m1_t vec_and1 = __riscv_vand_vx_u32m1(input32, 0x003F0000, vl);
    vuint32m1_t vec_and2 = __riscv_vand_vx_u32m1(input32, 0x0FC00000, vl);
    vuint32m1_t vec_and3 = __riscv_vand_vx_u32m1(input32, 0x000003F0, vl);
    vuint32m1_t vec_and4 = __riscv_vand_vx_u32m1(input32, 0x0000FC00, vl);

    vec_and1 = __riscv_vsll_vx_u32m1(vec_and1, 8, vl);
    vec_and2 = __riscv_vsrl_vx_u32m1(vec_and2, 6, vl);
    vec_and3 = __riscv_vsll_vx_u32m1(vec_and3, 4, vl);
    vec_and4 = __riscv_vsrl_vx_u32m1(vec_and4, 10, vl);

    vuint32m1_t vec_or1 = __riscv_vor_vv_u32m1(vec_and1, vec_and2, vl);
    vuint32m1_t vec_or2 = __riscv_vor_vv_u32m1(vec_and3, vec_and4, vl);
    return __riscv_vor_vv_u32m1(vec_or1, vec_or2, vl);
}

vuint32m1_t create_lookup_indices_opt(vuint8m1_t data, size_t vl)
{

    vuint32m1_t const_vec_ac = __riscv_vmv_v_x_u32m1(0x04000040, vl);
    vuint32m1_t const_vec_bd = __riscv_vmv_v_x_u32m1(0x01000010, vl);

    vuint32m1_t input32 = __riscv_vreinterpret_v_u8m1_u32m1(data);

    // mask out so that only a and c bits remain
    vuint32m1_t index_a_c = __riscv_vand_vx_u32m1(input32, 0x0FC0FC00, vl);

    // mask out so that only a and c bits remain
    vuint32m1_t index_b_d = __riscv_vand_vx_u32m1(input32, 0x003F03F0, vl);

    vl = __riscv_vsetvlmax_e16m1();
    // multiply 16-bit integers and store high 16 bits of 32-bit result
    vuint16m1_t vec_shifted_ac = __riscv_vmulhu_vv_u16m1(__riscv_vreinterpret_v_u32m1_u16m1(index_a_c), __riscv_vreinterpret_v_u32m1_u16m1(const_vec_ac), vl);

    // multiply 16-bit integers and store low 16 bits of 32-bit result
    vuint16m1_t vec_shifted_bd = __riscv_vmul_vv_u16m1(__riscv_vreinterpret_v_u32m1_u16m1(index_b_d), __riscv_vreinterpret_v_u32m1_u16m1(const_vec_bd), vl);

    vl = __riscv_vsetvlmax_e32m1();

    return __riscv_vor_vv_u32m1(__riscv_vreinterpret_v_u16m1_u32m1(vec_shifted_ac), __riscv_vreinterpret_v_u16m1_u32m1(vec_shifted_bd), vl);
}

static __attribute__((always_inline)) inline uint64_t rv_cycles(void)
{
    uint64_t cycle;
    __asm volatile("rdcycle %0" : "=r"(cycle));
    return cycle;
}

void __attribute__((noinline)) base64_encode(uint8_t *restrict input, uint8_t *output, size_t length)
{
    size_t vl;

    // size_t vlmax = __riscv_vsetvlmax_e32m1();

    size_t vlmax_8 = __riscv_vsetvlmax_e8m1();

    const vuint16m2_t vec_index = loadIndex();

    vint8m1_t offset_vec = __riscv_vle8_v_i8m1(offsets, vlmax_8);

    size_t input_slice = (vlmax_8 / 4) * 3;

    for (; length >= input_slice; length -= input_slice)
    {

        vl = __riscv_vsetvl_e8m1(input_slice);

        vuint8m1_t vec_input = __riscv_vle8_v_u8m1(input, vl);

        vl = __riscv_vsetvl_e8m1(vlmax_8);

        // TODO use vrgatherei16.vv here when possible
        uint64_t begin = rv_cycles();
        vuint8m1_t vec_gather = __riscv_vrgatherei16_vv_u8m1(vec_input, vec_index, vl);
        uint64_t end = rv_cycles();

        printf("cycles: %ld\n", end - begin);

        vl = __riscv_vsetvlmax_e32m1();

        // two different ways to compute lookup indices
        // vuint32m1_t vec_lookup_indices = create_lookup_indices_naive(vec_gather, vl);
        vuint32m1_t vec_lookup_indices = create_lookup_indices_opt(vec_gather, vl);

        vl = __riscv_vsetvlmax_e8m1();

        // three different ways to calculate the lookup step
        // vuint8m1_t base64_chars = __riscv_vluxei8_v_u8m1(b64chars, __riscv_vreinterpret_v_u32m1_u8m1(vec_lookup_indices), vl);
        // vuint8m1_t base64_chars = branchless_table_lookup_naive(__riscv_vreinterpret_v_u32m1_u8m1(vec_lookup_indices), vl);
        vuint8m1_t base64_chars = branchless_table_lookup_opt(__riscv_vreinterpret_v_u32m1_u8m1(vec_lookup_indices), offset_vec, vl);

        //  __riscv_vse8_v_u8m1(output, __riscv_vreinterpret_v_u32m1_u8m1(vec_lookup_indices), vl);
        __riscv_vse8_v_u8m1(output, base64_chars, vl);

        vl = __riscv_vsetvl_e8m1(length);
        input += input_slice;
        output += vlmax_8;
    }
    Base64encode((char *)output, (char *)input, length);
}

// #define N 400
#define N 1024 * 1024 * 32 // 32 MB

char *setupInputData()
{
    char alphabet[26] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    char *inputData = (char *)malloc(sizeof(char) * N);

    for (int i = 0; i < N; i++)
    {
        inputData[i] = alphabet[i % 26];
    }

    return inputData;
}

int64_t timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
    return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}

void checkResults(uint8_t *output_scalar, uint8_t *output_vector, size_t length)
{
    size_t error = 0;
    for (int i = 0; i < length; i++)
    {
        if (output_scalar[i] != output_vector[i])
        {
            printf("Error at index %d!\n", i);
            error = 1;
            break;
        }
    }
    if (!error)
    {
        printf("No Errors\n");
    }
}

int main(void)
{

    // for (int i = 0; i < 48; i += 3)
    // {
    //     printf("%d, %d, %d, %d, ", i + 1, i, i + 2, i + 1);
    // }
    // printf("\n");

    struct timespec start, end;
    uint64_t timeElapsed_scalar, timeElapsed_vector;

    char *inputData = setupInputData();

    int encode_length = Base64encode_len(N * sizeof(char));

    uint8_t *output_scalar = (uint8_t *)malloc(sizeof(char) * encode_length);
    uint8_t *output_vector = (uint8_t *)malloc(sizeof(char) * encode_length);

    // measure scalar code
    clock_gettime(CLOCK_MONOTONIC, &start);
    Base64encode((char *)output_scalar, inputData, N);
    clock_gettime(CLOCK_MONOTONIC, &end);
    timeElapsed_scalar = timespecDiff(&end, &start);
    printf("base64_scalar time: %ld\n", timeElapsed_scalar / 1000000);

    // measure vector code
    clock_gettime(CLOCK_MONOTONIC, &start);
    base64_encode((uint8_t *)inputData, output_vector, N);
    clock_gettime(CLOCK_MONOTONIC, &end);
    timeElapsed_vector = timespecDiff(&end, &start);
    printf("base64_vector time: %ld\n", timeElapsed_vector / 1000000);

    float speedup = ((float)timeElapsed_scalar / (float)timeElapsed_vector);
    printf("speedup %.02f %%\n", speedup * 100);

    for (int i = 0; i < 70; i++)
    {
        printf("%c", output_scalar[i]);
    }
    printf("\n");

    for (int i = 0; i < 70; i++)
    {
        printf("%c", output_vector[i]);
    }
    printf("\n");

    checkResults(output_scalar, output_vector, encode_length);

    free(inputData);
    free(output_scalar);
    free(output_vector);

    return 0;
}