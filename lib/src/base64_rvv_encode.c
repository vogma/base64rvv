#include <libb64rvv.h>

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

const uint8_t gather_index_lmul4[64] = {1, 0, 2, 1, 4, 3, 5, 4, 7, 6, 8, 7, 10, 9, 11, 10, 13, 12, 14, 13, 16, 15, 17, 16, 19, 18, 20, 19, 22, 21, 23, 22, 25, 24, 26, 25, 28, 27, 29, 28, 31, 30, 32, 31, 34, 33, 35, 34, 37, 36, 38, 37, 40, 39, 41, 40, 43, 42, 44, 43, 46, 45, 47, 46};

const int8_t offsets[16] = {71, -4, -4, -4, -4, -4, -4, -4, -4, -4, -4, -19, -16, 65, 0, 0};

vuint8m1_t __attribute__((always_inline)) inline branchless_table_lookup_opt(vuint8m1_t vec_indices, vint8m1_t offset_vec, size_t vl)
{
    vuint8m1_t result = __riscv_vssubu_vx_u8m1(vec_indices, 51, vl);

    vbool8_t vec_lt_26 = __riscv_vmsltu_vx_u8m1_b8(vec_indices, 26, vl);

    const vuint8m1_t vec_lookup = __riscv_vadd_vx_u8m1_mu(vec_lt_26, result, result, 13, vl);

    offset_vec = __riscv_vrgather_vv_i8m1(offset_vec, vec_lookup, vl);

    vint8m1_t ascii_vec = __riscv_vadd_vv_i8m1(__riscv_vreinterpret_v_u8m1_i8m1(vec_indices), offset_vec, vl);

    return __riscv_vreinterpret_v_i8m1_u8m1(ascii_vec);
}

vuint32m1_t __attribute__((always_inline)) inline create_lookup_indices_opt(vuint8m1_t data, size_t vl)
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

void base64_encode_rvv(uint8_t *input, uint8_t *output, size_t length)
{
    size_t vl;

    size_t vlmax_8 = __riscv_vsetvlmax_e8m1();

    const vuint8m1_t vec_index_e8m1 = __riscv_vle8_v_u8m1(gather_index_lmul4, vlmax_8);

    vint8m1_t offset_vec = __riscv_vle8_v_i8m1(offsets, vlmax_8);

    size_t input_slice = (vlmax_8 / 4) * 3;

    for (; length >= input_slice; length -= input_slice)
    {

        vl = __riscv_vsetvl_e8m1(input_slice);

        vuint8m1_t vec_input = __riscv_vle8_v_u8m1(input, vl);

        vl = __riscv_vsetvl_e8m1(vlmax_8);

        vuint8m1_t vec_gather = __riscv_vrgather_vv_u8m1(vec_input, vec_index_e8m1, vl);

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
