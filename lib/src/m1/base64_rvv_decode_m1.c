#include <libb64rvv.h>

#define NO_ERROR -1

const int8_t LOWER_INVALID = 1;
const int8_t UPPER_INVALID = 1;

const int8_t lower_bound_lut[16] =
    {LOWER_INVALID, LOWER_INVALID, 0x2B, 0x30,
     0x41, 0x50, 0x61, 0x70,
     LOWER_INVALID, LOWER_INVALID, LOWER_INVALID, LOWER_INVALID,
     LOWER_INVALID, LOWER_INVALID, LOWER_INVALID, LOWER_INVALID};

const int8_t upper_bound_lut[16] =
    {
        UPPER_INVALID, UPPER_INVALID, 0x2b, 0x39,
        0x4f, 0x5a, 0x6f, 0x7a,
        UPPER_INVALID, UPPER_INVALID, UPPER_INVALID, UPPER_INVALID,
        UPPER_INVALID, UPPER_INVALID, UPPER_INVALID, UPPER_INVALID};

vint8m2_t vector_lookup_vrgather_m2(vint8m2_t data, size_t vl)
{

    size_t vlmax_8 = __riscv_vsetvlmax_e8m2();

    const vint8m2_t vec_lower_lut = __riscv_vle8_v_i8m2(lower_bound_lut, vlmax_8);
    const vint8m2_t vec_upper_lut = __riscv_vle8_v_i8m2(upper_bound_lut, vlmax_8);
    const vint8m2_t vec_shift_lut = __riscv_vle8_v_i8m2(shift_lut, vlmax_8);

    // extract higher nibble from 8-bit data
    vuint8m2_t higher_nibble = __riscv_vsrl_vx_u8m2(__riscv_vreinterpret_v_i8m2_u8m2(data), 4, vlmax_8);

    vint8m2_t upper_bound = __riscv_vrgather_vv_i8m2(vec_upper_lut, higher_nibble, vlmax_8);
    vint8m2_t lower_bound = __riscv_vrgather_vv_i8m2(vec_lower_lut, higher_nibble, vlmax_8);

    vbool4_t lower = __riscv_vmslt_vv_i8m2_b4(data, lower_bound, vlmax_8);
    vbool4_t higher = __riscv_vmsgt_vv_i8m2_b4(data, upper_bound, vlmax_8);
    vbool4_t eq = __riscv_vmseq_vx_i8m2_b4(data, 0x2f, vlmax_8);

    vbool4_t or = __riscv_vmor_mm_b4(lower, higher, vlmax_8);
    vbool4_t outside = __riscv_vmandn_mm_b4(eq, or, vlmax_8);

    int error = __riscv_vfirst_m_b4(outside, vlmax_8);

    if (error != NO_ERROR)
    {
        printf("ERROR!\n");
    }

    vint8m2_t shift = __riscv_vrgather_vv_i8m2(vec_shift_lut, higher_nibble, vlmax_8);

    return __riscv_vadd_vv_i8m2(data, shift, vl);
    // return shift;
}

vuint32m2_t pack_data_m2(vint8m2_t data, size_t vl)
{
    size_t vlmax_32 = __riscv_vsetvlmax_e32m2();

    vuint8m2_t convert = __riscv_vreinterpret_v_i8m2_u8m2(data);
    vuint32m2_t data_vector = __riscv_vreinterpret_v_u8m2_u32m2(convert);

    vuint32m2_t ca = __riscv_vand_vx_u32m2(data_vector, 0x003f003f, vlmax_32);
    vuint32m2_t db = __riscv_vand_vx_u32m2(data_vector, 0x3f003f00, vlmax_32);

    ca = __riscv_vsll_vx_u32m2(ca, 6, vlmax_32);
    db = __riscv_vsrl_vx_u32m2(db, 8, vlmax_32);

    vuint32m2_t t0 = __riscv_vor_vv_u32m2(ca, db, vlmax_32);

    vuint32m2_t t1 = __riscv_vsll_vx_u32m2(t0, 12, vlmax_32);
    vuint32m2_t t2 = __riscv_vsrl_vx_u32m2(t0, 16, vlmax_32);

    t0 = __riscv_vor_vv_u32m2(t1, t2, vlmax_32);

    return t0;
    // return __riscv_vand_vx_u32m1(t0, 0x00FFFFFF, vlmax_32);
}

size_t base64_decode_rvv_m1(const char *data, int8_t *output, size_t input_length)
{
    size_t vlmax_8 = __riscv_vsetvlmax_e8m1();
    size_t dLen = 0;

    for (; input_length >= vlmax_8; input_length -= vlmax_8)
    {
        vint8m1_t data_reg = __riscv_vle8_v_i8m1((const signed char *)data, vlmax_8);

        size_t vlmax_8 = __riscv_vsetvlmax_e8m1();

        const vint8m1_t vec_shift_lut = __riscv_vle8_v_i8m1(shift_lut, vlmax_8);

        // extract higher nibble from 8-bit data
        vuint8m1_t higher_nibble = __riscv_vsrl_vx_u8m1(__riscv_vreinterpret_v_i8m1_u8m1(data_reg), 4, vlmax_8);

        // vint8m1_t upper_bound = __riscv_vrgather_vv_i8m1(vec_upper_lut, higher_nibble, vlmax_8);
        // vint8m1_t lower_bound = __riscv_vrgather_vv_i8m1(vec_lower_lut, higher_nibble, vlmax_8);

        // vbool8_t lower = __riscv_vmslt_vv_i8m1_b8(data, lower_bound, vlmax_8);
        // vbool8_t higher = __riscv_vmsgt_vv_i8m1_b8(data, upper_bound, vlmax_8);
        vbool8_t eq = __riscv_vmseq_vx_i8m1_b8(data_reg, 0x2f, vlmax_8);

        vlmax_8 = __riscv_vsetvlmax_e8m1();
        vint8m1_t shift = __riscv_vrgather_vv_i8m1(vec_shift_lut, higher_nibble, vlmax_8);

        data_reg = __riscv_vadd_vv_i8m1(data_reg, shift, vlmax_8);

        data_reg = __riscv_vadd_vx_i8m1_m(eq, data_reg, -3, vlmax_8);

        // vuint32m1_t packed_data = pack_data(data_reg, vlmax_8);

        size_t vlmax_32 = __riscv_vsetvlmax_e32m1();

        vuint8m1_t convert = __riscv_vreinterpret_v_i8m1_u8m1(data_reg);
        vuint32m1_t data_vector = __riscv_vreinterpret_v_u8m1_u32m1(convert);

        vuint32m1_t ca = __riscv_vand_vx_u32m1(data_vector, 0x003f003f, vlmax_32);
        ca = __riscv_vsll_vx_u32m1(ca, 6, vlmax_32);

        vuint32m1_t db = __riscv_vand_vx_u32m1(data_vector, 0x3f003f00, vlmax_32);
        db = __riscv_vsrl_vx_u32m1(db, 8, vlmax_32);

        vuint32m1_t t0 = __riscv_vor_vv_u32m1(ca, db, vlmax_32);

        vuint32m1_t t1 = __riscv_vsll_vx_u32m1(t0, 12, vlmax_32);
        vuint32m1_t t2 = __riscv_vsrl_vx_u32m1(t0, 16, vlmax_32);

        vuint32m1_t packed_data = __riscv_vor_vv_u32m1(t1, t2, vlmax_32);

        vuint8m1_t index_vector = __riscv_vle8_v_u8m1(index_decode, vlmax_8);

        // rearrange elements in vector

        vlmax_8 = __riscv_vsetvlmax_e8m1();

        vuint8m1_t packed_data_e8m1 = __riscv_vreinterpret_v_u32m1_u8m1(packed_data);

        vuint8m1_t result = __riscv_vrgather_vv_u8m1(packed_data_e8m1, index_vector, vlmax_8);

        size_t vl = __riscv_vsetvl_e8m1((vlmax_8 / 4) * 3);

        __riscv_vse8_v_i8m1(output, __riscv_vreinterpret_v_u8m1_i8m1(result), vl);

        data += vlmax_8;
        output += (vlmax_8 / 4) * 3;

        dLen += ((vlmax_8 / 4) * 3);
    }
    if (input_length != 0)
    {
        dLen += base64_decode_tail(data, input_length, (unsigned char *)output);
    }
    return dLen;
}

#define VRGATHER_u8m4(tbl, idx, vl) __riscv_vcreate_v_u8m1_u8m4(         \
    __riscv_vrgather_vv_u8m1(tbl, __riscv_vget_v_u8m4_u8m1(idx, 0), vl), \
    __riscv_vrgather_vv_u8m1(tbl, __riscv_vget_v_u8m4_u8m1(idx, 1), vl), \
    __riscv_vrgather_vv_u8m1(tbl, __riscv_vget_v_u8m4_u8m1(idx, 2), vl), \
    __riscv_vrgather_vv_u8m1(tbl, __riscv_vget_v_u8m4_u8m1(idx, 3), vl))

size_t b64_decode_rvv(const char *src, char *dst, size_t length)
{

    static const unsigned char err_lo[16] = {21, 17, 17, 17, 17, 17, 17, 17, 17, 17, 19, 26, 27, 27, 27, 26};
    static const unsigned char err_hi[16] = {16, 16, 1, 2, 4, 8, 4, 8, 16, 16, 16, 16, 16, 16, 16, 16};
    static const unsigned char off[16] = {0, 16, 19, 4, -65, -65, -71, -71, 0, 0, 0, 0, 0, 0, 0, 0};

    const vuint8m1_t vErrLo = __riscv_vle8_v_u8m1(err_lo, sizeof err_lo);
    const vuint8m1_t vErrHi = __riscv_vle8_v_u8m1(err_hi, sizeof err_hi);
    const vuint8m1_t vOff = __riscv_vle8_v_u8m1(off, sizeof off);

    // assume valid for now
    size_t len = length / 4;
    for (size_t vl; len > 0; len -= vl, dst += vl * 3, src += vl * 4)
    {
        vl = __riscv_vsetvl_e8m1(len);
        vuint8m1x4_t vseg = __riscv_vlseg4e8_v_u8m1x4((const uint8_t *)src, vl);
        vuint8m1_t v0 = __riscv_vget_u8m1(vseg, 0);
        vuint8m1_t v1 = __riscv_vget_u8m1(vseg, 1);
        vuint8m1_t v2 = __riscv_vget_u8m1(vseg, 2);
        vuint8m1_t v3 = __riscv_vget_u8m1(vseg, 3);

        vuint8m4_t v = __riscv_vcreate_v_u8m1_u8m4(v0, v1, v2, v3);
        const size_t vl4 = __riscv_vsetvlmax_e8m4();
        vuint8m4_t vHi = __riscv_vsrl(v, 4, vl4);
        vuint8m4_t vLo = __riscv_vand(v, 0xF, vl4);
        vuint8m4_t vIdx = __riscv_vmerge(vHi, ('/' >> 4) - 1, __riscv_vmseq(v, '/', vl4), vl4);

        vuint8m4_t vErr = __riscv_vand(
            VRGATHER_u8m4(vErrLo, vLo, vl),
            VRGATHER_u8m4(vErrHi, vHi, vl), vl4);

        // TODO: make sure this works when vl!=vlmax
        if (__riscv_vfirst(__riscv_vmsne(vErr, 0, vl4), vl4) >= 0)
            return 0;

        vuint8m4_t bits = VRGATHER_u8m4(vOff, vIdx, vl);

        vuint8m1_t b0 = __riscv_vadd(v0, __riscv_vget_u8m1(bits, 0), vl); // 0b00AAAAAA
        vuint8m1_t b1 = __riscv_vadd(v1, __riscv_vget_u8m1(bits, 1), vl); // 0b00AABBBB
        vuint8m1_t b2 = __riscv_vadd(v2, __riscv_vget_u8m1(bits, 2), vl); // 0b00BBBBCC
        vuint8m1_t b3 = __riscv_vadd(v3, __riscv_vget_u8m1(bits, 3), vl); // 0b00CCCCCC

        vuint8m1_t o0 = __riscv_vor(__riscv_vsll(b0, 2, vl), __riscv_vsrl(b1, 4, vl), vl);
        vuint8m1_t o1 = __riscv_vor(__riscv_vsll(b1, 4, vl), __riscv_vsrl(b2, 2, vl), vl);
        vuint8m1_t o2 = __riscv_vor(__riscv_vsll(b2, 6, vl), b3, vl);
        __riscv_vsseg3e8_v_u8m1x3((uint8_t *)dst, __riscv_vcreate_v_u8m1x3(o0, o1, o2), vl);
    }

    return 0;
}

size_t base64_decode_rvv_m4(const char *data, int8_t *output, size_t input_length)
{
    size_t vlmax_8 = __riscv_vsetvlmax_e8m4();
    size_t vlmax_e8m1 = __riscv_vsetvlmax_e8m1();

    size_t dLen = 0;

    const vuint8m1_t index_vector = __riscv_vle8_v_u8m1(index_decode, vlmax_e8m1);

    for (; input_length >= vlmax_8; input_length -= vlmax_8)
    {
        vint8m4_t data_reg = __riscv_vle8_v_i8m4((const signed char *)data, vlmax_8);

        size_t vlmax_8 = __riscv_vsetvlmax_e8m4();

        const vint8m1_t vec_shift_lut = __riscv_vle8_v_i8m1(shift_lut, vlmax_8);

        // const vint8m1_t vec_shift_lut = __riscv_vle8_v_i8m2(lookup_vlen8_m2, vlmax_8);

        // extract higher nibble from 8-bit data
        vuint8m4_t higher_nibble = __riscv_vsrl_vx_u8m4(__riscv_vreinterpret_v_i8m4_u8m4(data_reg), 4, vlmax_8);

        // vint8m1_t upper_bound = __riscv_vrgather_vv_i8m1(vec_upper_lut, higher_nibble, vlmax_8);
        // vint8m1_t lower_bound = __riscv_vrgather_vv_i8m1(vec_lower_lut, higher_nibble, vlmax_8);

        // vbool8_t lower = __riscv_vmslt_vv_i8m1_b8(data, lower_bound, vlmax_8);
        // vbool8_t higher = __riscv_vmsgt_vv_i8m1_b8(data, upper_bound, vlmax_8);
        vbool2_t eq = __riscv_vmseq_vx_i8m4_b2(data_reg, 0x2f, vlmax_8);

        // vbool8_t or = __riscv_vmor_mm_b8(lower, higher, vlmax_8);
        // vbool8_t outside = __riscv_vmandn_mm_b8(eq, or, vlmax_8);

        // int error = __riscv_vfirst_m_b8(outside, vlmax_8);

        // if (error != NO_ERROR)
        // {
        //     printf("ERROR!\n");
        // }

        // vint8m1_t shift = __riscv_vrgather_vv_i8m1(vec_shift_lut, higher_nibble, vlmax_8);

        vlmax_8 = __riscv_vsetvlmax_e8m1();
        vint8m1_t vec_vrgather_0 = __riscv_vrgather_vv_i8m1(vec_shift_lut, __riscv_vget_v_u8m4_u8m1(higher_nibble, 0), vlmax_8);
        vint8m1_t vec_vrgather_1 = __riscv_vrgather_vv_i8m1(vec_shift_lut, __riscv_vget_v_u8m4_u8m1(higher_nibble, 1), vlmax_8);
        vint8m1_t vec_vrgather_2 = __riscv_vrgather_vv_i8m1(vec_shift_lut, __riscv_vget_v_u8m4_u8m1(higher_nibble, 2), vlmax_8);
        vint8m1_t vec_vrgather_3 = __riscv_vrgather_vv_i8m1(vec_shift_lut, __riscv_vget_v_u8m4_u8m1(higher_nibble, 3), vlmax_8);

        vint8m4_t shift = __riscv_vcreate_v_i8m1_i8m4(vec_vrgather_0, vec_vrgather_1, vec_vrgather_2, vec_vrgather_3);

        vlmax_8 = __riscv_vsetvlmax_e8m4();
        data_reg = __riscv_vadd_vv_i8m4(data_reg, shift, vlmax_8);
        data_reg = __riscv_vadd_vx_i8m4_m(eq, data_reg, -3, vlmax_8);

        // vuint32m1_t packed_data = pack_data(data_reg, vlmax_8);

        size_t vlmax_32 = __riscv_vsetvlmax_e32m4();

        vuint8m4_t convert = __riscv_vreinterpret_v_i8m4_u8m4(data_reg);
        vuint32m4_t data_vector = __riscv_vreinterpret_v_u8m4_u32m4(convert);

        vuint32m4_t ca = __riscv_vand_vx_u32m4(data_vector, 0x003f003f, vlmax_32);
        ca = __riscv_vsll_vx_u32m4(ca, 6, vlmax_32);

        vuint32m4_t db = __riscv_vand_vx_u32m4(data_vector, 0x3f003f00, vlmax_32);
        db = __riscv_vsrl_vx_u32m4(db, 8, vlmax_32);

        vuint32m4_t t0 = __riscv_vor_vv_u32m4(ca, db, vlmax_32);

        vuint32m4_t t1 = __riscv_vsll_vx_u32m4(t0, 12, vlmax_32);
        vuint32m4_t t2 = __riscv_vsrl_vx_u32m4(t0, 16, vlmax_32);

        vuint32m4_t packed_data = __riscv_vor_vv_u32m4(t1, t2, vlmax_32);

        // vlmax_8 = __riscv_vsetvlmax_e8m1();

        // rearrange elements in vector

        // vuint8m4_t packed_data_e8m4 = __riscv_vreinterpret_v_u32m4_u8m4(data_vector);
        vuint8m4_t packed_data_e8m4 = __riscv_vreinterpret_v_u32m4_u8m4(packed_data);

        vuint8m1_t result_0 = __riscv_vrgather_vv_u8m1(__riscv_vget_v_u8m4_u8m1(packed_data_e8m4, 0), index_vector, vlmax_8);
        vuint8m1_t result_1 = __riscv_vrgather_vv_u8m1(__riscv_vget_v_u8m4_u8m1(packed_data_e8m4, 1), index_vector, vlmax_8);
        vuint8m1_t result_2 = __riscv_vrgather_vv_u8m1(__riscv_vget_v_u8m4_u8m1(packed_data_e8m4, 2), index_vector, vlmax_8);
        vuint8m1_t result_3 = __riscv_vrgather_vv_u8m1(__riscv_vget_v_u8m4_u8m1(packed_data_e8m4, 3), index_vector, vlmax_8);

        size_t vl = __riscv_vsetvl_e8m1((vlmax_e8m1 / 4) * 3);

        __riscv_vse8_v_i8m1(output, __riscv_vreinterpret_v_u8m1_i8m1(result_0), vl);
        output += (vlmax_e8m1 / 4) * 3;

        __riscv_vse8_v_i8m1(output, __riscv_vreinterpret_v_u8m1_i8m1(result_1), vl);
        output += (vlmax_e8m1 / 4) * 3;

        __riscv_vse8_v_i8m1(output, __riscv_vreinterpret_v_u8m1_i8m1(result_2), vl);
        output += (vlmax_e8m1 / 4) * 3;

        __riscv_vse8_v_i8m1(output, __riscv_vreinterpret_v_u8m1_i8m1(result_3), vl);
        output += (vlmax_e8m1 / 4) * 3;

        vlmax_8 = __riscv_vsetvlmax_e8m4();
        data += vlmax_8;
        dLen += vlmax_e8m1 * 3;
    }
    if (input_length != 0)
    {
        dLen += base64_decode_tail(data, input_length, (unsigned char *)output);
    }
    return dLen;
}
