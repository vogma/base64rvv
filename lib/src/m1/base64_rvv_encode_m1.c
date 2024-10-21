#include <libb64rvv.h>
#include <utils.h>

vuint8m1_t __attribute__((always_inline)) inline table_lookup_m1(vuint8m1_t vec_indices, vint8m1_t offset_vec, size_t vl)
{
    // reduce values 0-64 to 0-13
    vuint8m1_t result = __riscv_vssubu_vx_u8m1(vec_indices, 51, vl);
    vbool8_t vec_lt_26 = __riscv_vmsltu_vx_u8m1_b8(vec_indices, 26, vl);
    const vuint8m1_t vec_lookup = __riscv_vadd_vx_u8m1_mu(vec_lt_26, result, result, 13, vl);

    offset_vec = __riscv_vrgather_vv_i8m1(offset_vec, vec_lookup, vl);

    vint8m1_t ascii_vec = __riscv_vadd_vv_i8m1(__riscv_vreinterpret_v_u8m1_i8m1(vec_indices), offset_vec, vl);

    return __riscv_vreinterpret_v_i8m1_u8m1(ascii_vec);
}

vuint32m1_t __attribute__((always_inline)) inline lookup_m1(vuint8m1_t data, size_t vl)
{

    const vuint32m1_t const_vec_ac = __riscv_vmv_v_x_u32m1(0x04000040, vl);
    const vuint32m1_t const_vec_bd = __riscv_vmv_v_x_u32m1(0x01000010, vl);

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

void base64_encode_rvv_m1(uint8_t *input, uint8_t *output, size_t length)
{
    size_t vl;

    size_t vlmax_e8m1 = __riscv_vsetvlmax_e8m1();

    const vuint8m1_t vec_index_e8m1 = __riscv_vle8_v_u8m1(gather_index_lmul8, vlmax_e8m1);

    vint8m1_t offset_vec = __riscv_vle8_v_i8m1(offsets, vlmax_e8m1);

    size_t input_slice_e8m1 = (vlmax_e8m1 / 4) * 3;

    for (; length >= input_slice_e8m1; length -= input_slice_e8m1)
    {

        vl = __riscv_vsetvl_e8m1(input_slice_e8m1);

        /**
         * Load (vlmax_e8m1 / 4) * 3 elements
         */
        vuint8m1_t vec_input = __riscv_vle8_v_u8m1(input, vl);
        input += (vlmax_e8m1 / 4) * 3;

        vl = __riscv_vsetvl_e8m1(vlmax_e8m1);

        vuint8m1_t vec_gather = __riscv_vrgather_vv_u8m1(vec_input, vec_index_e8m1, vl);

        vl = __riscv_vsetvlmax_e32m1();

        vuint32m1_t vec_lookup_indices = lookup_m1(vec_gather, vl);

        vl = __riscv_vsetvlmax_e8m1();

        // two different ways to calculate the lookup step
        // vuint8m1_t base64_chars = __riscv_vluxei8_v_u8m1(b64chars, __riscv_vreinterpret_v_u32m1_u8m1(vec_lookup_indices), vl);
        vuint8m1_t base64_chars = table_lookup_m1(__riscv_vreinterpret_v_u32m1_u8m1(vec_lookup_indices), offset_vec, vl);

        __riscv_vse8_v_u8m1(output, base64_chars, vl);

        vl = __riscv_vsetvl_e8m1(length);

        output += vlmax_e8m1;
    }
    Base64encode((char *)output, (char *)input, length);
}
