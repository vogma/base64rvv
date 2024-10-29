#define vl t0
#define vec_mask_A v31
#define vec_mask_B v30
#define vec_mask_C v29
#define vec_mask_D v28
#define bytes_per_register t6

    .global    base64rvv_encode_m1_asm
base64rvv_encode_m1_asm:
    addi       sp, sp, -12
    sw         ra, 0(sp)
    sw         s0, 4(sp)
    sw         s1, 8(sp)


    li         t2, 0x0000FC00
    li         t3, 0x00F00300
    li         t4, 0xC00F0000
    li         t5, 0x3F000000

    //populate vector registers with constant values
    vsetvli    vl, zero, e32, m1, ta, ma
    vmv.v.x    vec_mask_A, t2
    vmv.v.x    vec_mask_B, t3
    vmv.v.x    vec_mask_C, t4
    vmv.v.x    vec_mask_D, t5

    csrr       bytes_per_register, vlenb


    sb         vl,(a1)

    lw         ra,0(sp)
    lw         s0,4(sp)
    lw         s1,8(sp)
    addi       sp, sp, 12

    ret
