#define const_vec_ac v13
#define const_vec_bd v14
#define offsets a2
#define output a1
#define input a0
#define gather_index a3
#define length_pt a4
#define length s2
#define offsets_vec v5


    .global         base64_encode_asm
base64_encode_asm:

    addi            sp, sp, -32
    sd              s0, 0(sp)
    sd              s1, 8(sp)
    sd              s2, 16(sp)
    sd              s3, 24(sp)

    li              t0, 0x0FC0FC00
    li              t1, 0x003F03F0

    li              t2, 0x04000040
    li              t3, 0x01000010

    li              t4,51
    li              t5,25

    vsetvli         zero, a0, e32, m1, ta, ma

    vmv.v.x         const_vec_ac, t2
    vmv.v.x         const_vec_bd, t3

# inputSlice = (vlmax_8m1 / 4) * 3
    vsetvli         s3, zero, e8, m1, ta, ma
    srli            s1, s3, 2
    sh1add          s1, s1, s1


# load length
    lw              length, 0(length_pt)

# load gather index into vector register
    vsetvli         t6, zero, e16, m2, ta, ma
    vle16.v         v6, (gather_index)        # occupies v6-v7

# load lookup offsets
    vle8.v          offsets_vec, (offsets)

# loop begin
.loop_start:
    vsetvli         t6, zero, e8, m1, ta, ma

    vle8.v          v10, (input)
    vrgatherei16.vv v18, v10, v6


# byte shuffle
    vsetvli         zero, a0, e32, m1, ta, ma

    vand.vx         v11, v18, t0
    vand.vx         v12, v18, t1

    vsetvli         t6, zero, e16, m1, ta, ma
    vmulhu.vv       v9, v11, const_vec_ac
    vmul.vv         v8, v12, const_vec_bd

    vsetvli         t6, zero, e32, m1, ta, ma
    vor.vv          v8, v9, v8


# lookup
    vsetvli         t6, zero, e8, m1, ta, ma

    vmsgtu.vx       v1, v8, t5
    vmsleu.vx       v2, v8, t4
    vmand.mm        v0, v1, v2
    vssubu.vx       v1, v8, t4
    vmerge.vim      v1, v1, 13, v0
    vrgather.vv     v11, offsets_vec, v1
    vadd.vv         v8, v8, v11

# write ascii characters to memory
    vse8.v          v8, (output)

# bump pointers
    add             input, input, s1
    add             output, output, s3

    sub             length, length, s1
    bge             length, s1, .loop_start   # if length >= s1 then .loop_start

    sw              length,(length_pt)

    ld              s0,0(sp)
    ld              s1,8(sp)
    ld              s2,16(sp)
    ld              s3,24(sp)
    addi            sp,sp,32

    ret

