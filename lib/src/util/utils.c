#include <utils.h>
#include <riscv_vector.h>

static const unsigned char b64chars[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void printRegister(vuint8m4_t vec)
{
    size_t vlmax_e8m4 = __riscv_vsetvlmax_e8m4();
    uint8_t memory[vlmax_e8m4];

    __riscv_vse8_v_u8m4(memory, vec, vlmax_e8m4);
    printf("register contents: ");
    for (int i = 0; i < vlmax_e8m4; i++)
    {
        printf("%c", memory[i]);
    }
    printf("\n");
}

/**
 * creates gather indices for a given vlen.
 *
 * example vlen=128, bytes shown
 * 0 0 0 0 0 0 0 1 0 0 0 2 0 0 0 3 <- vid 32-bit lanes
 * 0 0 0 0 0 0 1 0 0 0 2 0 0 0 3 0 <- left shift by 8
 * 0 0 0 0 0 1 0 0 0 2 0 0 0 3 0 0 <- left shift by 16
 * 0 0 0 0 1 0 0 0 2 0 0 0 3 0 0 0 <- left shift by 24

 * 0 0 0 0 1 1 1 1 2 2 2 2 3 3 3 3 <- or all of the above
 * 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 3 <- broadcast constant value 3

 * 0 0 0 0 3 3 3 3 6 6 6 6 9 9 9 9 <- multiply both lines above
 * 1 0 2 1 1 0 2 1 1 0 2 1 1 0 2 1 <- broadcast index base value 0x01020001

 * 1 0 2 1 4 3 5 4 7 6 8 7 10 9 11 10 <- add both lines above / finished index values
 */
vuint8m1_t __attribute__((always_inline)) inline createGatherIndexEncode(size_t vl)
{
    vuint32m1_t ids = __riscv_vid_v_u32m1(vl * 4);
    vuint32m1_t ids_shift8 = __riscv_vsll_vx_u32m1(ids, 8, vl);
    vuint32m1_t ids_shift16 = __riscv_vsll_vx_u32m1(ids, 16, vl);
    vuint32m1_t ids_shift24 = __riscv_vsll_vx_u32m1(ids, 24, vl);
    ids = __riscv_vor_vv_u32m1(ids, ids_shift8, vl);
    ids = __riscv_vor_vv_u32m1(ids, ids_shift16, vl);
    ids = __riscv_vor_vv_u32m1(ids, ids_shift24, vl);

    const vuint8m1_t const_vec_3 = __riscv_vmv_v_x_u8m1(3, vl);
    const vuint8m1_t const_index_vec = __riscv_vreinterpret_u8m1(__riscv_vmv_v_x_u32m1(0x01020001, vl));

    vuint8m1_t index_vec = __riscv_vmul_vv_u8m1(const_vec_3, __riscv_vreinterpret_u8m1(ids), vl);

    return __riscv_vadd_vv_u8m1(index_vec, const_index_vec, vl);
}

/**
 * creates the indices for the decode gather, if VLEN > 512 bit.
 * Index pattern: 2, 1, 0, 6, 5 ,4, 10, 9, 8, ...
 */
vuint8m1_t createGatherIndexDecode(size_t vl)
{
    size_t index_size = vl / 3 + 1;
    uint8_t indices[index_size];

    for (int i = 0, j = 0; i < index_size; i++, j += 3)
    {
        indices[j] = 4 * i + 2;
        indices[j + 1] = 4 * i + 1;
        indices[j + 2] = 4 * i;
    }
    return __riscv_vle8_v_u8m1(indices, vl);
}

vuint8m1_t createDecodeIndices(size_t vl)
{
    if (vl <= (512 / 8))
    {
        return __riscv_vle8_v_u8m1(index_decode, vl);
    }
    else
    {
        return createGatherIndexDecode(vl);
    }
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

unsigned int base64_decode_tail(const char *in, unsigned int inlen, unsigned char *out)
{
    unsigned int i;
    unsigned int j;
    unsigned char c;

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