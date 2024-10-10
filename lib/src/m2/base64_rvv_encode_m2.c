#include <libb64rvv.h>

vuint32m2_t __attribute__((always_inline)) inline lookup_m2(vuint8m2_t data, size_t vl)
{

    const vuint32m2_t const_vec_ac = __riscv_vmv_v_x_u32m2(0x04000040, vl);
    const vuint32m2_t const_vec_bd = __riscv_vmv_v_x_u32m2(0x01000010, vl);

    vuint32m2_t input32 = __riscv_vreinterpret_v_u8m2_u32m2(data);

    // mask out so that only a and c bits remain
    vuint32m2_t index_a_c = __riscv_vand_vx_u32m2(input32, 0x0FC0FC00, vl);

    // mask out so that only a and c bits remain
    vuint32m2_t index_b_d = __riscv_vand_vx_u32m2(input32, 0x003F03F0, vl);

    vl = __riscv_vsetvlmax_e16m2();
    // multiply 16-bit integers and store high 16 bits of 32-bit result
    vuint16m2_t vec_shifted_ac = __riscv_vmulhu_vv_u16m2(__riscv_vreinterpret_v_u32m2_u16m2(index_a_c), __riscv_vreinterpret_v_u32m2_u16m2(const_vec_ac), vl);

    // multiply 16-bit integers and store low 16 bits of 32-bit result
    vuint16m2_t vec_shifted_bd = __riscv_vmul_vv_u16m2(__riscv_vreinterpret_v_u32m2_u16m2(index_b_d), __riscv_vreinterpret_v_u32m2_u16m2(const_vec_bd), vl);

    vl = __riscv_vsetvlmax_e32m2();

    return __riscv_vor_vv_u32m2(__riscv_vreinterpret_v_u16m2_u32m2(vec_shifted_ac), __riscv_vreinterpret_v_u16m2_u32m2(vec_shifted_bd), vl);
}

vuint8m2_t __attribute__((always_inline)) inline table_lookup_m2(vuint8m2_t vec_indices, vint8m1_t offset_vec, size_t vl)
{
    // reduce values 0-64 to 0-13
    vuint8m2_t result = __riscv_vssubu_vx_u8m2(vec_indices, 51, vl);
    vbool4_t vec_lt_26 = __riscv_vmsltu_vx_u8m2_b4(vec_indices, 26, vl);
    const vuint8m2_t vec_lookup = __riscv_vadd_vx_u8m2_mu(vec_lt_26, result, result, 13, vl);

    // shuffle registers one by one
    vint8m1_t offset_vec_0 = __riscv_vrgather_vv_i8m1(offset_vec, __riscv_vget_v_u8m2_u8m1(vec_lookup, 0), vl);
    vint8m1_t offset_vec_1 = __riscv_vrgather_vv_i8m1(offset_vec, __riscv_vget_v_u8m2_u8m1(vec_lookup, 1), vl);

    vint8m2_t offset_vec_bundle = __riscv_vcreate_v_i8m1_i8m2(offset_vec_0, offset_vec_1);

    vint8m2_t ascii_vec = __riscv_vadd_vv_i8m2(__riscv_vreinterpret_v_u8m2_i8m2(vec_indices), offset_vec_bundle, vl);

    return __riscv_vreinterpret_v_i8m2_u8m2(ascii_vec);
}

void base64_encode_rvv_m2(uint8_t *input, uint8_t *output, size_t length)
{
    size_t vl;

    size_t vlmax_e8m2 = __riscv_vsetvlmax_e8m2();
    size_t vlmax_e8m1 = __riscv_vsetvlmax_e8m1();

    const vuint8m1_t vec_index_e8m1 = __riscv_vle8_v_u8m1(gather_index_lmul4, vlmax_e8m1);

    vint8m1_t offset_vec = __riscv_vle8_v_i8m1(offsets, vlmax_e8m1);

    size_t input_slice_e8m2 = (vlmax_e8m2 / 4) * 3;
    size_t input_slice_e8m1 = (vlmax_e8m1 / 4) * 3;

    for (; length >= input_slice_e8m2; length -= input_slice_e8m2)
    {

        vl = __riscv_vsetvl_e8m1(input_slice_e8m1);

        /**
         * Load (vlmax_e8m1 / 4) * 3 elements into each vector register.
         */
        vuint8m1_t vec_input_0 = __riscv_vle8_v_u8m1(input, vl);
        input += (vlmax_e8m1 / 4) * 3;

        vuint8m1_t vec_input_1 = __riscv_vle8_v_u8m1(input, vl);
        input += (vlmax_e8m1 / 4) * 3;

        vl = __riscv_vsetvl_e8m1(vlmax_e8m1);

        //  the vrgather operation is cheaper at lmul=1 (4*4=16 cycles) than at lmul=4 (64 cycles), therefore each register gets shuffled seperately (https://camel-cdr.github.io/rvv-bench-results/bpi_f3/index.html)
        vuint8m1_t vec_gather_0 = __riscv_vrgather_vv_u8m1(vec_input_0, vec_index_e8m1, vl);
        vuint8m1_t vec_gather_1 = __riscv_vrgather_vv_u8m1(vec_input_1, vec_index_e8m1, vl);

        vuint8m2_t vec_gather = __riscv_vcreate_v_u8m1_u8m2(vec_gather_0, vec_gather_1);

        vl = __riscv_vsetvlmax_e32m2();

        vuint32m2_t vec_lookup_indices = lookup_m2(vec_gather, vl);

        vl = __riscv_vsetvlmax_e8m2();

        // two different ways to calculate the lookup step
        // vuint8m2_t base64_chars = __riscv_vluxei8_v_u8m2(b64chars, __riscv_vreinterpret_v_u32m2_u8m2(vec_lookup_indices), vl);
        vuint8m2_t base64_chars = table_lookup_m2(__riscv_vreinterpret_v_u32m2_u8m2(vec_lookup_indices), offset_vec, vl);

        __riscv_vse8_v_u8m2(output, base64_chars, vl);

        vl = __riscv_vsetvl_e8m2(length);

        output += vlmax_e8m2;
    }
    Base64encode((char *)output, (char *)input, length);
}
