#include <libb64rvv.h>

#define NO_ERROR -1

/* ASCII order for BASE 64 decode, 255 in unused character */
static const unsigned char base64de[] = {
    /* nul, soh, stx, etx, eot, enq, ack, bel, */
    255, 255, 255, 255, 255, 255, 255, 255,

    /*  bs,  ht,  nl,  vt,  np,  cr,  so,  si, */
    255, 255, 255, 255, 255, 255, 255, 255,

    /* dle, dc1, dc2, dc3, dc4, nak, syn, etb, */
    255, 255, 255, 255, 255, 255, 255, 255,

    /* can,  em, sub, esc,  fs,  gs,  rs,  us, */
    255, 255, 255, 255, 255, 255, 255, 255,

    /*  sp, '!', '"', '#', '$', '%', '&', ''', */
    255, 255, 255, 255, 255, 255, 255, 255,

    /* '(', ')', '*', '+', ',', '-', '.', '/', */
    255, 255, 255, 62, 255, 255, 255, 63,

    /* '0', '1', '2', '3', '4', '5', '6', '7', */
    52, 53, 54, 55, 56, 57, 58, 59,

    /* '8', '9', ':', ';', '<', '=', '>', '?', */
    60, 61, 255, 255, 255, 255, 255, 255,

    /* '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', */
    255, 0, 1, 2, 3, 4, 5, 6,

    /* 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', */
    7, 8, 9, 10, 11, 12, 13, 14,

    /* 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', */
    15, 16, 17, 18, 19, 20, 21, 22,

    /* 'X', 'Y', 'Z', '[', '\', ']', '^', '_', */
    23, 24, 25, 255, 255, 255, 255, 255,

    /* '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', */
    255, 26, 27, 28, 29, 30, 31, 32,

    /* 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', */
    33, 34, 35, 36, 37, 38, 39, 40,

    /* 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', */
    41, 42, 43, 44, 45, 46, 47, 48,

    /* 'x', 'y', 'z', '{', '|', '}', '~', del, */
    49, 50, 51, 255, 255, 255, 255, 255};

#define BASE64_PAD '='
#define BASE64DE_FIRST '+'
#define BASE64DE_LAST 'z'

unsigned int
base64_decode_tail(const char *in, unsigned int inlen, unsigned char *out)
{
    unsigned int i;
    unsigned int j;
    unsigned char c;

    // if (inlen & 0x3)
    // {
    //     return 0;
    // }

    for (i = j = 0; i < inlen; i++)
    {
        if (in[i] == BASE64_PAD)
        {
            break;
        }
        if (in[i] < BASE64DE_FIRST || in[i] > BASE64DE_LAST)
        {
            return 0;
        }

        c = base64de[(unsigned char)in[i]];
        if (c == 255)
        {
            return 0;
        }

        switch (i & 0x3)
        {
        case 0:
            out[j] = (c << 2) & 0xFF;
            break;
        case 1:
            out[j++] |= (c >> 4) & 0x3;
            out[j] = (c & 0xF) << 4;
            break;
        case 2:
            out[j++] |= (c >> 2) & 0xF;
            out[j] = (c & 0x3) << 6;
            break;
        case 3:
            out[j++] |= c;
            break;
        }
    }

    return j;
}

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

const int8_t shift_lut[16] = {
    /* 0 */ 0x00, /* 1 */ 0x00, /* 2 */ 0x3e - 0x2b, /* 3 */ 0x34 - 0x30,
    /* 4 */ 0x00 - 0x41, /* 5 */ 0x0f - 0x50, /* 6 */ 0x1a - 0x61, /* 7 */ 0x29 - 0x70,
    /* 8 */ 0x00, /* 9 */ 0x00, /* a */ 0x00, /* b */ 0x00,
    /* c */ 0x00, /* d */ 0x00, /* e */ 0x00, /* f */ 0x00};

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

const uint8_t index_decode[16] = {2, 1, 0, 6, 5, 4, 10, 9, 8, 14, 13, 12, 15, 3, 7, 11};
const uint8_t index_decode_256[32] = {2, 1, 0, 6, 5, 4, 10, 9, 8, 14, 13, 12, 18, 17, 16, 22, 21, 20, 26, 25, 24, 30, 29, 28, 15, 3, 7, 11, 19, 23, 27, 31};

// static uint8_t lookup_vlen8_m2[32*8];

void base64_decode_rvv(const char *data, int8_t *output, size_t input_length)
{
    size_t vlmax_8 = __riscv_vsetvlmax_e8m2();

    for (; input_length >= vlmax_8; input_length -= vlmax_8)
    {
        vint8m2_t data_reg = __riscv_vle8_v_i8m2((const signed char *)data, vlmax_8);

        // data_reg = vector_loochromkup_naive(data_reg, vlmax_8);
        // data_reg = vector_lookup_vrgather(data_reg, vlmax_8);

        size_t vlmax_8 = __riscv_vsetvlmax_e8m2();

        const vint8m1_t vec_shift_lut = __riscv_vle8_v_i8m1(shift_lut, vlmax_8);

        // const vint8m1_t vec_shift_lut = __riscv_vle8_v_i8m2(lookup_vlen8_m2, vlmax_8);

        // extract higher nibble from 8-bit data
        vuint8m2_t higher_nibble = __riscv_vsrl_vx_u8m2(__riscv_vreinterpret_v_i8m2_u8m2(data_reg), 4, vlmax_8);

        // vint8m1_t upper_bound = __riscv_vrgather_vv_i8m1(vec_upper_lut, higher_nibble, vlmax_8);
        // vint8m1_t lower_bound = __riscv_vrgather_vv_i8m1(vec_lower_lut, higher_nibble, vlmax_8);

        // vbool8_t lower = __riscv_vmslt_vv_i8m1_b8(data, lower_bound, vlmax_8);
        // vbool8_t higher = __riscv_vmsgt_vv_i8m1_b8(data, upper_bound, vlmax_8);
        // vbool8_t eq = __riscv_vmseq_vx_i8m1_b8(data, 0x2f, vlmax_8);

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

        vuint8m1_t index_vector = __riscv_vle8_v_u8m1(index_decode_256, vlmax_8);

        // rearrange elements in vector

        vlmax_8 = __riscv_vsetvlmax_e8m1();

        vuint8m2_t packed_data_e8m2 = __riscv_vreinterpret_v_u32m2_u8m2(packed_data);

        vuint8m1_t packed_data_e8m2_0 = __riscv_vget_v_u8m2_u8m1(packed_data_e8m2, 0);
        vuint8m1_t packed_data_e8m2_1 = __riscv_vget_v_u8m2_u8m1(packed_data_e8m2, 1);
        vuint8m1_t result_0 = __riscv_vrgather_vv_u8m1(packed_data_e8m2_0, index_vector, vlmax_8);
        vuint8m1_t result_1 = __riscv_vrgather_vv_u8m1(packed_data_e8m2_1, index_vector, vlmax_8);

        size_t vl = __riscv_vsetvl_e8m1((vlmax_8 / 4) * 3);

        // __riscv_vse8_v_i8m1(output, result, vl);
        __riscv_vse8_v_i8m1(output, __riscv_vreinterpret_v_u8m1_i8m1(result_0), vl);
        // __riscv_vse8_v_i8m1(output, data_reg, vl);

        data += vlmax_8;
        output += (vlmax_8 / 4) * 3;

        __riscv_vse8_v_i8m1(output, __riscv_vreinterpret_v_u8m1_i8m1(result_1), vl);

        data += vlmax_8;
        output += (vlmax_8 / 4) * 3;
    }
    if (input_length != 0)
    {
        base64_decode_tail(data, input_length, (unsigned char *)output);
    }
}

void base64_decode_rvv_m4(const char *data, int8_t *output, size_t input_length)
{
    size_t vlmax_8 = __riscv_vsetvlmax_e8m4();

    for (; input_length >= vlmax_8; input_length -= vlmax_8)
    {
        vint8m4_t data_reg = __riscv_vle8_v_i8m4((const signed char *)data, vlmax_8);

        // data_reg = vector_loochromkup_naive(data_reg, vlmax_8);
        // data_reg = vector_lookup_vrgather(data_reg, vlmax_8);

        size_t vlmax_8 = __riscv_vsetvlmax_e8m4();

        const vint8m1_t vec_shift_lut = __riscv_vle8_v_i8m1(shift_lut, vlmax_8);

        // const vint8m1_t vec_shift_lut = __riscv_vle8_v_i8m2(lookup_vlen8_m2, vlmax_8);

        // extract higher nibble from 8-bit data
        vuint8m4_t higher_nibble = __riscv_vsrl_vx_u8m4(__riscv_vreinterpret_v_i8m4_u8m4(data_reg), 4, vlmax_8);

        // vint8m1_t upper_bound = __riscv_vrgather_vv_i8m1(vec_upper_lut, higher_nibble, vlmax_8);
        // vint8m1_t lower_bound = __riscv_vrgather_vv_i8m1(vec_lower_lut, higher_nibble, vlmax_8);

        // vbool8_t lower = __riscv_vmslt_vv_i8m1_b8(data, lower_bound, vlmax_8);
        // vbool8_t higher = __riscv_vmsgt_vv_i8m1_b8(data, upper_bound, vlmax_8);
        // vbool8_t eq = __riscv_vmseq_vx_i8m1_b8(data, 0x2f, vlmax_8);

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

        vlmax_8 = __riscv_vsetvlmax_e8m1();
        vuint8m1_t index_vector = __riscv_vle8_v_u8m1(index_decode_256, vlmax_8);

        // rearrange elements in vector

        vuint8m4_t packed_data_e8m4 = __riscv_vreinterpret_v_u32m4_u8m4(packed_data);

        vuint8m1_t packed_data_e8m4_0 = __riscv_vget_v_u8m4_u8m1(packed_data_e8m4, 0);
        vuint8m1_t packed_data_e8m4_1 = __riscv_vget_v_u8m4_u8m1(packed_data_e8m4, 1);
        vuint8m1_t packed_data_e8m4_2 = __riscv_vget_v_u8m4_u8m1(packed_data_e8m4, 2);
        vuint8m1_t packed_data_e8m4_3 = __riscv_vget_v_u8m4_u8m1(packed_data_e8m4, 3);

        vuint8m1_t result_0 = __riscv_vrgather_vv_u8m1(packed_data_e8m4_0, index_vector, vlmax_8);
        vuint8m1_t result_1 = __riscv_vrgather_vv_u8m1(packed_data_e8m4_1, index_vector, vlmax_8);
        vuint8m1_t result_2 = __riscv_vrgather_vv_u8m1(packed_data_e8m4_2, index_vector, vlmax_8);
        vuint8m1_t result_3 = __riscv_vrgather_vv_u8m1(packed_data_e8m4_3, index_vector, vlmax_8);

        size_t vl = __riscv_vsetvl_e8m1((vlmax_8 / 4) * 3);

        __riscv_vse8_v_i8m1(output, __riscv_vreinterpret_v_u8m1_i8m1(result_0), vl);
        output += (vlmax_8 / 4) * 3;

        __riscv_vse8_v_i8m1(output, __riscv_vreinterpret_v_u8m1_i8m1(result_1), vl);
        output += (vlmax_8 / 4) * 3;

        __riscv_vse8_v_i8m1(output, __riscv_vreinterpret_v_u8m1_i8m1(result_2), vl);
        output += (vlmax_8 / 4) * 3;

        __riscv_vse8_v_i8m1(output, __riscv_vreinterpret_v_u8m1_i8m1(result_3), vl);
        output += (vlmax_8 / 4) * 3;

        vlmax_8 = __riscv_vsetvlmax_e8m4();
        data += vlmax_8;
    }
    if (input_length != 0)
    {
        base64_decode_tail(data, input_length, (unsigned char *)output);
    }
}
