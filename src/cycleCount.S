#define const_vec_ac v13
#define const_vec_bd v14


    .global         cycleCount_asm
cycleCount_asm:
    vsetvli         a3, zero, e16, m2, ta, ma

    vle16.v         v8, (a0)

    vsetvli         a3, zero, e8, m1, ta, ma

    vle8.v          v10, (a1)
    vrgatherei16.vv v18, v10, v8


# byte shuffle
    vsetvli         zero, a0, e32, m1, ta, ma

    li              t0, 0x0FC0FC00
    li              t1, 0x003F03F0

    li              t2,0x04000040
    li              t3,0x01000010

    vand.vx         v11, v18, t0
    vand.vx         v12, v18, t1

    vmv.v.x         const_vec_ac, t2
    vmv.v.x         const_vec_bd, t3

    vsetvli         a0, zero, e16, m1, ta, ma
    vmulhu.vv       v9, v11, const_vec_ac
    vmul.vv         v8, v12, const_vec_bd

    vsetvli         a0, zero, e32, m1, ta, ma
    vor.vv          v8, v9, v8


# lookup
    vsetvli         a3, zero, e8, m1, ta, ma
    li              t4,51
    li              t5,25
    vmsgtu.vx       v1, v8, t5
    vmsleu.vx       v2, v8, t4
    vmand.mm        v0, v1, v2
    vssubu.vx       v1, v8, t4
    vmerge.vim      v1, v1, 13, v0
    vle8.v          v2, (a2)
    vrgather.vv     v11, v2, v1
    vadd.vv         v8, v8, v11




    vsetvli         a3, zero, e8, m1, ta, ma

    vse8.v          v8, (a1)

    ret

