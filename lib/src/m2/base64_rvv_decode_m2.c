
#include <libb64rvv.h>
#include <utils.h>

size_t base64_decode_rvv_m2(const char *data, int8_t *output, size_t input_length)
{
    size_t vlmax_8 = __riscv_vsetvlmax_e8m2();
    size_t vlmax_e8m1 = __riscv_vsetvlmax_e8m1();
    size_t dLen = 0;

    const vuint8m1_t index_vector = __riscv_vle8_v_u8m1(index_decode, vlmax_e8m1);

    for (; input_length >= vlmax_8; input_length -= vlmax_8)
    {
        vint8m2_t data_reg = __riscv_vle8_v_i8m2((const signed char *)data, vlmax_8);

        // data_reg = vector_loochromkup_naive(data_reg, vlmax_8);
        // data_reg = vector_lookup_vrgather(data_reg, vlmax_8);

        size_t vlmax_8 = __riscv_vsetvlmax_e8m2();

        const vint8m1_t vec_shift_lut = __riscv_vle8_v_i8m1(shift_lut, 16);

        // const vint8m1_t vec_shift_lut = __riscv_vle8_v_i8m2(lookup_vlen8_m2, vlmax_8);

        // extract higher nibble from 8-bit data
        vuint8m2_t higher_nibble = __riscv_vsrl_vx_u8m2(__riscv_vreinterpret_v_i8m2_u8m2(data_reg), 4, vlmax_8);

        // vint8m1_t upper_bound = __riscv_vrgather_vv_i8m1(vec_upper_lut, higher_nibble, vlmax_8);
        // vint8m1_t lower_bound = __riscv_vrgather_vv_i8m1(vec_lower_lut, higher_nibble, vlmax_8);

        // vbool8_t lower = __riscv_vmslt_vv_i8m1_b8(data, lower_bound, vlmax_8);
        // vbool8_t higher = __riscv_vmsgt_vv_i8m1_b8(data, upper_bound, vlmax_8);
        vbool4_t eq = __riscv_vmseq_vx_i8m2_b4(data_reg, 0x2f, vlmax_8);

        // vbool8_t or = __riscv_vmor_mm_b8(lower, higher, vlmax_8);
        // vbool8_t outside = __riscv_vmandn_mm_b8(eq, or, vlmax_8);

        // int error = __riscv_vfirst_m_b8(outside, vlmax_8);

        // if (error != NO_ERROR)
        // {
        //     printf("ERROR!\n");
        // }

        // vint8m1_t shift = __riscv_vrgather_vv_i8m1(vec_shift_lut, higher_nibble, vlmax_8);

        vlmax_8 = __riscv_vsetvlmax_e8m1();
        vint8m2_t shift = __riscv_vcreate_v_i8m1_i8m2(__riscv_vrgather_vv_i8m1(vec_shift_lut, __riscv_vget_v_u8m2_u8m1(higher_nibble, 0), vlmax_8), __riscv_vrgather_vv_i8m1(vec_shift_lut, __riscv_vget_v_u8m2_u8m1(higher_nibble, 1), vlmax_8));

        vlmax_8 = __riscv_vsetvlmax_e8m2();
        data_reg = __riscv_vadd_vv_i8m2(data_reg, shift, vlmax_8);
        data_reg = __riscv_vadd_vx_i8m2_m(eq, data_reg, -3, vlmax_8);

        // vuint32m1_t packed_data = pack_data(data_reg, vlmax_8);

        size_t vlmax_32 = __riscv_vsetvlmax_e32m2();

        vuint8m2_t convert = __riscv_vreinterpret_v_i8m2_u8m2(data_reg);
        vuint32m2_t data_vector = __riscv_vreinterpret_v_u8m2_u32m2(convert);

        vuint32m2_t ca = __riscv_vand_vx_u32m2(data_vector, 0x003f003f, vlmax_32);
        ca = __riscv_vsll_vx_u32m2(ca, 6, vlmax_32);

        vuint32m2_t db = __riscv_vand_vx_u32m2(data_vector, 0x3f003f00, vlmax_32);
        db = __riscv_vsrl_vx_u32m2(db, 8, vlmax_32);

        vuint32m2_t t0 = __riscv_vor_vv_u32m2(ca, db, vlmax_32);

        vuint32m2_t t1 = __riscv_vsll_vx_u32m2(t0, 12, vlmax_32);
        vuint32m2_t t2 = __riscv_vsrl_vx_u32m2(t0, 16, vlmax_32);

        vuint32m2_t packed_data = __riscv_vor_vv_u32m2(t1, t2, vlmax_32);

        // rearrange elements in vector

        vlmax_8 = __riscv_vsetvlmax_e8m1();

        vuint8m2_t packed_data_e8m2 = __riscv_vreinterpret_v_u32m2_u8m2(packed_data);
        // vuint8m2_t packed_data_e8m2 = convert;
        vuint8m1_t result_0 = __riscv_vrgather_vv_u8m1(__riscv_vget_v_u8m2_u8m1(packed_data_e8m2, 0), index_vector, vlmax_8);
        vuint8m1_t result_1 = __riscv_vrgather_vv_u8m1(__riscv_vget_v_u8m2_u8m1(packed_data_e8m2, 1), index_vector, vlmax_8);

        size_t vl = __riscv_vsetvl_e8m1((vlmax_8 / 4) * 3);

        __riscv_vse8_v_i8m1(output, __riscv_vreinterpret_v_u8m1_i8m1(result_0), vl);
        output += (vlmax_8 / 4) * 3;

        __riscv_vse8_v_i8m1(output, __riscv_vreinterpret_v_u8m1_i8m1(result_1), vl);
        output += (vlmax_8 / 4) * 3;

        data += vlmax_8 * 2;
        dLen += ((vlmax_8 / 4) * 3) * 2;
    }
    if (input_length != 0)
    {
        dLen += base64_decode_tail(data, input_length, (unsigned char *)output);
    }
    return dLen;
}