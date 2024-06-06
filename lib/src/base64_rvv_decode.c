#include <libb64rvv.h>

#define NO_ERROR -1

static const int8_t decoding_table_static[256] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x3F, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

unsigned char *base64_decode(const unsigned char *data,
                             size_t input_length,
                             unsigned char *output_data,
                             size_t *output_length)
{
    if (input_length % 4 != 0)
        return NULL;

    *output_length = input_length / 4 * 3;

    // == possible on end of input stream
    if (data[input_length - 1] == '=')
        (*output_length)--;
    if (data[input_length - 2] == '=')
        (*output_length)--;

    unsigned char *decoded_data = malloc(*output_length);
    if (decoded_data == NULL)
        return NULL;

    for (int i = 0, j = 0; i < input_length;)
    {

        uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table_static[data[i++]];
        uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table_static[data[i++]];
        uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table_static[data[i++]];
        uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table_static[data[i++]];

        uint32_t triple = (sextet_a << 3 * 6) + (sextet_b << 2 * 6) + (sextet_c << 1 * 6) + (sextet_d << 0 * 6);

        if (j < *output_length)
            decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < *output_length)
            decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < *output_length)
            decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
    }

    return decoded_data;
}

#define CHAR_A 64
#define CHAR_Z 91
#define CHAR_a 96
#define CHAR_z 123
#define NUM_0 47
#define NUM_9 58

#define OFFSET_PLUS 19
#define OFFSET_SLASH 16
#define OFFSET_09 4
#define OFFSET_AZ -65
#define OFFSET_az -71

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

vint8m1_t __attribute__((always_inline)) inline vector_lookup_vrgather(vint8m1_t data, size_t vl)
{

    size_t vlmax_8 = __riscv_vsetvlmax_e8m1();

    const vint8m1_t vec_lower_lut = __riscv_vle8_v_i8m1(lower_bound_lut, vlmax_8);
    const vint8m1_t vec_upper_lut = __riscv_vle8_v_i8m1(upper_bound_lut, vlmax_8);
    const vint8m1_t vec_shift_lut = __riscv_vle8_v_i8m1(shift_lut, vlmax_8);

    // extract higher nibble from 8-bit data
    vuint8m1_t higher_nibble = __riscv_vsrl_vx_u8m1(__riscv_vreinterpret_v_i8m1_u8m1(data), 4, vlmax_8);

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

    vint8m1_t shift = __riscv_vrgather_vv_i8m1(vec_shift_lut, higher_nibble, vlmax_8);

    return __riscv_vadd_vv_i8m1(data, shift, vl);
    // return shift;
}

vint8m1_t vector_lookup_naive(vint8m1_t data, size_t vl)
{

    vint8m1_t offset_reg = __riscv_vmv_v_x_i8m1(0, vl);

    vbool8_t mask_gt_A = __riscv_vmsgt_vx_i8m1_b8(data, CHAR_A, vl);
    vbool8_t mask_lt_Z = __riscv_vmslt_vx_i8m1_b8(data, CHAR_Z, vl);
    vbool8_t mask_AZ = __riscv_vmand_mm_b8(mask_gt_A, mask_lt_Z, vl);
    offset_reg = __riscv_vmerge_vxm_i8m1(offset_reg, OFFSET_AZ, mask_AZ, vl);

    vbool8_t mask_gt_a = __riscv_vmsgt_vx_i8m1_b8(data, CHAR_a, vl);
    vbool8_t mask_lt_z = __riscv_vmslt_vx_i8m1_b8(data, CHAR_z, vl);
    vbool8_t mask_az = __riscv_vmand_mm_b8(mask_gt_a, mask_lt_z, vl);
    offset_reg = __riscv_vmerge_vxm_i8m1(offset_reg, OFFSET_az, mask_az, vl);

    vbool8_t mask_gt_0 = __riscv_vmsgt_vx_i8m1_b8(data, NUM_0, vl);
    vbool8_t mask_lt_9 = __riscv_vmslt_vx_i8m1_b8(data, NUM_9, vl);
    vbool8_t mask_09 = __riscv_vmand_mm_b8(mask_gt_0, mask_lt_9, vl);
    offset_reg = __riscv_vmerge_vxm_i8m1(offset_reg, OFFSET_09, mask_09, vl);

    vbool8_t mask_eq_plus = __riscv_vmseq_vx_i8m1_b8(data, '+', vl);
    offset_reg = __riscv_vmerge_vxm_i8m1(offset_reg, OFFSET_PLUS, mask_eq_plus, vl);

    vbool8_t mask_eq_slash = __riscv_vmseq_vx_i8m1_b8(data, '/', vl);
    offset_reg = __riscv_vmerge_vxm_i8m1(offset_reg, OFFSET_SLASH, mask_eq_slash, vl);

    // if any of the elements of offset_reg is 0, the input contains invalid characters
    int error = __riscv_vfirst_m_b8(__riscv_vmseq_vx_i8m1_b8(offset_reg, 0, vl), vl);

    if (error != NO_ERROR)
    {
        printf("ERROR!\n");
    }
    // return offset_reg;
    return __riscv_vadd_vv_i8m1(data, offset_reg, vl);
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

vuint32m1_t __attribute__((always_inline)) inline pack_data(vint8m1_t data, size_t vl)
{
    size_t vlmax_32 = __riscv_vsetvlmax_e32m1();

    vuint8m1_t convert = __riscv_vreinterpret_v_i8m1_u8m1(data);
    vuint32m1_t data_vector = __riscv_vreinterpret_v_u8m1_u32m1(convert);

    vuint32m1_t ca = __riscv_vand_vx_u32m1(data_vector, 0x003f003f, vlmax_32);
    vuint32m1_t db = __riscv_vand_vx_u32m1(data_vector, 0x3f003f00, vlmax_32);

    ca = __riscv_vsll_vx_u32m1(ca, 6, vlmax_32);
    db = __riscv_vsrl_vx_u32m1(db, 8, vlmax_32);

    vuint32m1_t t0 = __riscv_vor_vv_u32m1(ca, db, vlmax_32);

    vuint32m1_t t1 = __riscv_vsll_vx_u32m1(t0, 12, vlmax_32);
    vuint32m1_t t2 = __riscv_vsrl_vx_u32m1(t0, 16, vlmax_32);

    t0 = __riscv_vor_vv_u32m1(t1, t2, vlmax_32);

    return t0;
    // return __riscv_vand_vx_u32m1(t0, 0x00FFFFFF, vlmax_32);
}

const uint8_t index_decode[16] = {2, 1, 0, 6, 5, 4, 10, 9, 8, 14, 13, 12, 15, 3, 7, 11};
const uint8_t index_decode_256[32] = {2, 1, 0, 6, 5, 4, 10, 9, 8, 14, 13, 12, 18, 17, 16, 22, 21, 20, 26, 25, 24, 30, 29, 28, 15, 3, 7, 11, 19, 23, 27, 31};

void base64_decode_rvv(const char *data, int8_t *output, size_t input_length, size_t *output_length)
{
    size_t vlmax_8 = __riscv_vsetvlmax_e8m1();

    size_t output_length_int = 0;

    for (; input_length >= vlmax_8; input_length -= vlmax_8)
    {
        vint8m1_t data_reg = __riscv_vle8_v_i8m1((const signed char *)data, vlmax_8);

        // data_reg = vector_lookup_naive(data_reg, vlmax_8);
        data_reg = vector_lookup_vrgather(data_reg, vlmax_8);

        vuint32m1_t packed_data = pack_data(data_reg, vlmax_8);

        vuint8m1_t index_vector = __riscv_vle8_v_u8m1(index_decode_256, vlmax_8);

        // rearrange elements in vector
        vuint8m1_t result = __riscv_vrgather_vv_u8m1(__riscv_vreinterpret_v_u32m1_u8m1(packed_data), index_vector, vlmax_8);

        // only store 12 of 16 bytes
        size_t vl = __riscv_vsetvl_e8m1((vlmax_8 / 4) * 3);

        // __riscv_vse8_v_i8m1(output, result, vl);
        __riscv_vse8_v_i8m1(output, __riscv_vreinterpret_v_u8m1_i8m1(result), vl);

        data += vlmax_8;
        output += (vlmax_8 / 4) * 3;
        output_length_int += 12;
    }

    *output_length = output_length_int;
}

void base64_decode_rvv_m2(const char *data, int8_t *output, size_t input_length, size_t *output_length)
{
    size_t vlmax_8 = __riscv_vsetvlmax_e8m2();

    size_t output_length_int = 0;

    for (; input_length >= vlmax_8; input_length -= vlmax_8)
    {
        vint8m2_t data_reg = __riscv_vle8_v_i8m2((const signed char *)data, vlmax_8);

        // data_reg = vector_lookup_naive(data_reg, vlmax_8);
        data_reg = vector_lookup_vrgather_m2(data_reg, vlmax_8);

        vuint32m2_t packed_data = pack_data_m2(data_reg, vlmax_8);

        vuint8m2_t index_vector = __riscv_vle8_v_u8m2(index_decode_256, vlmax_8);

        // rearrange elements in vector
        vuint8m2_t result = __riscv_vrgather_vv_u8m2(__riscv_vreinterpret_v_u32m2_u8m2(packed_data), index_vector, vlmax_8);

        // only store 12 of 16 bytes
        size_t vl = __riscv_vsetvl_e8m2((vlmax_8 / 4) * 3);

        // __riscv_vse8_v_i8m1(output, result, vl);
        __riscv_vse8_v_i8m2(output, __riscv_vreinterpret_v_u8m2_i8m2(result), vl);

        data += vlmax_8;
        output += (vlmax_8 / 4) * 3;
        output_length_int += 12;
    }

    *output_length = output_length_int;
}