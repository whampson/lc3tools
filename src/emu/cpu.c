#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <lc3.h>
#include <cpu.h>
#include <mem.h>
#include <state.h>

/*
 * Instruction Register fields.
 */
#define OPCODE()        ((cpu.ir & 0xF000) >> 12)
#define DR()            ((cpu.ir & 0x0E00) >> 9)
#define SR()            ((cpu.ir & 0x0E00) >> 9)
#define SR1()           ((cpu.ir & 0x01C0) >> 6)
#define SR2()           (cpu.ir & 0x0007)
#define BASER()         ((cpu.ir & 0x01C0) >> 6)
#define IMM4()          (cpu.ir & 0x000F)
#define IMM5()          (cpu.ir & 0x001F)
#define OFF6()          (cpu.ir & 0x003F)
#define OFF9()          (cpu.ir & 0x01FF)
#define OFF11()         (cpu.ir & 0x07FF)
#define TRAPVECT()      (cpu.ir & 0x00FF)
#define IR_N()          (cpu.ir & 0x0800)
#define IR_Z()          (cpu.ir & 0x0400)
#define IR_P()          (cpu.ir & 0x0200)
#define IR_11()         (cpu.ir & 0x0800)   /* N; addressing type (JSR) */
#define IR_10()         (cpu.ir & 0x0400)   /* Z */
#define IR_9()          (cpu.ir & 0x0200)   /* P */
#define IR_5()          (cpu.ir & 0x0020)   /* ALU operation (ADD/AND/XOR) */
#define IR_4()          (cpu.ir & 0x0010)   /* direction (SHF) */

/*
 * Processor State Register fields.
 */
#define PRIORITY()      (cpu.psr.priority)
#define PRIVILEGE()     (cpu.psr.privilege)
#define N()             (cpu.psr.n)
#define Z()             (cpu.psr.z)
#define P()             (cpu.psr.p)

#define SET_PRIORITY(x) (cpu.psr.priority = x)
#define SET_PRIVILEGE(x)(cpu.psr.privilege = x)
#define SET_N(x)        (cpu.psr.n = x)
#define SET_Z(x)        (cpu.psr.z = x)
#define SET_P(x)        (cpu.psr.p = x)

struct ctl_state {
    uint16_t ird    : 1;    /* next state retrieved from opcode in IR */
    uint16_t cond   : 3;    /* next state conditionals (external inputs) */
    uint16_t j      : 6;    /* the next state */
};

static struct lc3cpu cpu;

/* Unused: 26, 46, 53, 55, 57, 61, 63 */
static const struct ctl_state ctl_rom[] = {
    { 0, 0x02, 18 }, { 0, 0x00, 18 }, { 0, 0x00, 29 }, { 0, 0x00, 24 }, /*  0-3  */
    { 0, 0x03, 20 }, { 0, 0x00, 18 }, { 0, 0x00, 25 }, { 0, 0x00, 23 }, /*  4-7  */
    { 0, 0x04, 36 }, { 0, 0x00, 18 }, { 0, 0x00, 56 }, { 0, 0x00, 60 }, /*  8-11 */
    { 0, 0x00, 18 }, { 0, 0x00, 18 }, { 0, 0x00, 18 }, { 0, 0x00, 28 }, /* 12-15 */
    { 0, 0x00, 18 }, { 0, 0x01, 17 }, { 0, 0x05, 33 }, { 0, 0x05, 33 }, /* 16-19 */
    { 0, 0x00, 18 }, { 0, 0x00, 18 }, { 0, 0x00, 18 }, { 0, 0x00, 16 }, /* 20-23 */
    { 0, 0x00, 17 }, { 0, 0x01, 25 }, { 0, 0x00, 00 }, { 0, 0x00, 18 }, /* 24-27 */
    { 0, 0x01, 28 }, { 0, 0x01, 29 }, { 0, 0x00, 18 }, { 0, 0x00, 18 }, /* 28-31 */
    { 1, 0x00, 00 }, { 0, 0x01, 33 }, { 0, 0x04, 51 }, { 0, 0x00, 32 }, /* 32-35 */
    { 0, 0x01, 36 }, { 0, 0x00, 41 }, { 0, 0x00, 39 }, { 0, 0x00, 40 }, /* 36-39 */
    { 0, 0x01, 40 }, { 0, 0x01, 41 }, { 0, 0x00, 34 }, { 0, 0x00, 47 }, /* 40-43 */
    { 0, 0x00, 45 }, { 0, 0x00, 37 }, { 0, 0x00, 00 }, { 0, 0x00, 48 }, /* 44-47 */
    { 0, 0x01, 48 }, { 0, 0x04, 37 }, { 0, 0x00, 52 }, { 0, 0x00, 18 }, /* 48-51 */
    { 0, 0x01, 52 }, { 0, 0x00, 00 }, { 0, 0x00, 18 }, { 0, 0x00, 00 }, /* 52-55 */
    { 0, 0x01, 56 }, { 0, 0x00, 00 }, { 0, 0x00, 25 }, { 0, 0x00, 18 }, /* 56-59 */
    { 0, 0x01, 60 }, { 0, 0x00, 00 }, { 0, 0x00, 23 }, { 0, 0x00, 00 }  /* 60-63 */
};

typedef void (*state_fn)(void);

static const state_fn state_table[] = {
    state_00, state_01, state_02, state_03, state_04,
    state_05, state_06, state_07, state_08, state_09,
    state_10, state_11, state_12, state_13, state_14,
    state_15, state_16, state_17, state_18, state_19,
    state_20, state_21, state_22, state_23, state_24,
    state_25, state_26, state_27, state_28, state_29,
    state_30, state_31, state_32, state_33, state_34,
    state_35, state_36, state_37, state_38, state_39,
    state_40, state_41, state_42, state_43, state_44,
    state_45, state_46, state_47, state_48, state_49,
    state_50, state_51, state_52, state_53, state_54,
    state_55, state_56, state_57, state_58, state_59,
    state_60, state_61, state_62, state_63
};

static inline lc3word reg_r(int n);
static inline void reg_w(int n, lc3word data);
static inline void next_state(void);
static inline void setcc(void);
static inline lc3sword sign_extend(lc3word val, int pos);

/* ===== Public Functions ===== */

void cpu_reset(void)
{
    memset(&cpu, 0, sizeof(struct lc3cpu));
    cpu.state = 18;
    SET_Z(1);
}

void cpu_tick(void)
{
    printf("State %d!\n", cpu.state);
    // if (cpu.state == 18 || cpu.state == 19) {
    //     printf("PC: 0x%04x\n", cpu.pc);
    // }
    // else if (cpu.state == 32) {
    //     printf("IR: 0x%04x\n", cpu.ir);
    // }
    state_table[cpu.state]();
    next_state();
}

/* ===== Private Helper Functions ===== */

/*
 * Read a word from a general-purpose register.
 */
static inline lc3word reg_r(int n)
{
    return cpu.r[n & 0x07];
}

/*
 * Write a word to a general-purpose register.
 */
static inline void reg_w(int n, lc3word data)
{
    cpu.r[n & 0x07] = data;
}

/*
 * Put the CPU into the next state.
 */
inline void next_state(void)
{
    struct ctl_state ctl;

    ctl = ctl_rom[cpu.state];
    if (ctl.ird) {
        cpu.state = OPCODE();
        return;
    }

    cpu.state = ctl.j;
    if (mem_ready() && ctl.cond == 0x01) {
        cpu.state |= 0x02;
    }
    if (cpu.ben && ctl.cond == 0x02) {
        cpu.state |= 0x04;
    }
    if (IR_11() && ctl.cond == 0x03) {
        cpu.state |= 0x01;
    }
    if (PRIVILEGE() && ctl.cond == 0x04) {
        cpu.state |= 0x08;
    }
    if (cpu.intf && ctl.cond == 0x05) {
        cpu.state |= 0x10;
    }
}

/*
 * Update the CPU's condition codes based on the value in the destination
 * register.
 */
static inline void setcc(void)
{
    lc3word val;

    val = reg_r(DR());
    SET_N((val & 0x8000) == 0x8000);
    SET_Z(val == 0);
    SET_P(!(N() || Z()));
}

/*
 * Sign-extend a value from the given bit position.
 * This effectively converts an unsigned word into a signed word.
 */
static inline lc3sword sign_extend(lc3word val, int pos)
{
    lc3word mask;

    mask = 1 << (pos - 1);
    return (lc3sword) ((val ^ mask) - mask);
}

/* ===== CPU States ===== */

void state_00(void)
{
    /* BR (1/2) */
    /* NOP */
}

void state_01(void)
{
    /* ADD (1/1) */
    lc3word op1, op2;
    lc3word result;

    op1 = reg_r(SR1());
    op2 = (IR_5())
        ? sign_extend(IMM5(), 5)
        : reg_r(SR2());
    result = op1 + op2;
    reg_w(DR(), result);
    setcc();
}

void state_02(void)
{
    /* LDB (1/3) */
    cpu.mar = reg_r(BASER()) + sign_extend(OFF6(), 6);
}

void state_03(void)
{
    /* STB (1/3) */
    cpu.mar = reg_r(BASER()) + sign_extend(OFF6(), 6);
}

void state_04(void)
{
    /* JSR (1/3) */
    /* NOP */
}

void state_05(void)
{
    /* AND (1/1) */
    lc3word op1, op2;
    lc3word result;

    op1 = reg_r(SR1());
    op2 = (IR_5())
        ? sign_extend(IMM5(), 5)
        : reg_r(SR2());
    result = op1 & op2;
    reg_w(DR(), result);
    setcc();
}

void state_06(void)
{
    /* LDW (1/3) */
    cpu.mar = reg_r(BASER()) + (sign_extend(OFF6(), 6) << 1);
}

void state_07(void)
{
    /* STW (1/3) */
    cpu.mar = reg_r(BASER()) + (sign_extend(OFF6(), 6) << 1);
}

void state_08(void)
{

}

void state_09(void)
{
    /* XOR (1/1) */
    lc3word op1, op2;
    lc3word result;

    op1 = reg_r(SR1());
    op2 = (IR_5())
        ? sign_extend(IMM5(), 5)
        : reg_r(SR2());
    result = op1 ^ op2;
    reg_w(DR(), result);
    setcc();
}

void state_10(void)
{
    /* LDI (1/5) */
    cpu.mar = reg_r(BASER()) + (sign_extend(OFF9(), 9) << 1);
}

void state_11(void)
{
    /* STI (1/5) */
    cpu.mar = reg_r(BASER()) + (sign_extend(OFF9(), 9) << 1);
}

void state_12(void)
{
    /* JMP (1/1) */
    cpu.pc = reg_r(BASER());
}

void state_13(void)
{
    /* SHF (1/1) */
    lc3word op1, op2;
    lc3word result;

    op1 = reg_r(SR1());
    op2 = IMM4();
    if (IR_4()) {
        if (IR_5()) {
            /* arithmetic right shift */
            result = (lc3sword) op1 >> op2;
        }
        else {
            /* logical right shift */
            result = op1 >> op2;
        }
    }
    else {
        result = op1 << op2;
    }
    reg_w(DR(), result);
    setcc();
}

void state_14(void)
{
    /* LEA (1/1) */
    reg_w(DR(), cpu.pc + (sign_extend(OFF9(), 9) << 1));
    setcc();
}

void state_15(void)
{
    /* TRAP (1/3) */
    cpu.mar = (TRAPVECT() << 1);
}

void state_16(void)
{
    /* STW (3/3) */
    /* STI (5/5) */
    mem_write(cpu.mar, cpu.mdr, 0xFFFF);
}

void state_17(void)
{
    /* STB (3/3) */
    if (cpu.mar & 1) {
        mem_write(cpu.mar, cpu.mdr << 8, 0xFF00);
    }
    else {
        mem_write(cpu.mar, cpu.mdr, 0x00FF);
    }
}

void state_18(void)
{
    /* Fetch (1/3) */
    cpu.mar = cpu.pc;
    cpu.pc += 2;
}

void state_19(void)
{
    /* Fetch (1/3) (same as state 18) */
    cpu.mar = cpu.pc;
    cpu.pc += 2;
}

void state_20(void)
{
    /* JSR (2/3) */
    reg_w(R_7, cpu.pc);
    cpu.pc = reg_r(BASER());
}

void state_21(void)
{
    /* JSR (3/3) */
    reg_w(R_7, cpu.pc);
    cpu.pc = cpu.pc + (sign_extend(OFF11(), 11) << 1);
}

void state_22(void)
{
    /* BR (2/2) */
    cpu.pc = cpu.pc + (sign_extend(OFF9(), 9) << 1);
}

void state_23(void)
{
    /* STW (2/3) */
    /* STI (4/5) */
    cpu.mdr = reg_r(SR());
}

void state_24(void)
{
    /* STB (2/3) */
    cpu.mdr = reg_r(SR()) & 0x00FF;
}

void state_25(void)
{
    /* LDW (2/3) */
    /* LDI (4/5) */
    mem_read(&cpu.mdr, cpu.mar);
}

void state_26(void)
{

}

void state_27(void)
{
    /* LDW (3/3) */
    /* LDI (5/5) */
    reg_w(DR(), cpu.mdr);
    setcc();
}

void state_28(void)
{
    /* TRAP (2/3) */
    mem_read(&cpu.mdr, cpu.mar);
    reg_w(R_7, cpu.pc);
}

void state_29(void)
{
    /* LDB (2/3) */
    mem_read(&cpu.mdr, cpu.mar & 0xFFFE);
}

void state_30(void)
{
    /* TRAP (3/3) */
    cpu.pc = cpu.mdr;
}

void state_31(void)
{
    /* LDB (3/3) */
    lc3word val;

    val = ((cpu.mar & 1) ? (cpu.mdr >> 8) : cpu.mdr & 0xFF);
    reg_w(DR(), sign_extend(val, 8));
    setcc();
}

void state_32(void)
{
    /* Decode */
    cpu.ben = (IR_11() && N()) || (IR_10() && Z()) || (IR_9() && P());
}

void state_33(void)
{
    /* Fetch (2/3) */
    mem_read(&cpu.mdr, cpu.mar);
}

void state_34(void)
{

}

void state_35(void)
{
    /* Fetch (3/3) */
    cpu.ir = cpu.mdr;
}

void state_36(void)
{

}

void state_37(void)
{

}

void state_38(void)
{

}

void state_39(void)
{

}

void state_40(void)
{

}

void state_41(void)
{

}

void state_42(void)
{

}

void state_43(void)
{

}

void state_44(void)
{

}

void state_45(void)
{

}

void state_46(void)
{

}

void state_47(void)
{

}

void state_48(void)
{

}

void state_49(void)
{

}

void state_50(void)
{

}

void state_51(void)
{

}

void state_52(void)
{

}

void state_53(void)
{

}

void state_54(void)
{

}

void state_55(void)
{

}

void state_56(void)
{
    /* LDI (2/5) */
    mem_read(&cpu.mdr, cpu.mar);
}

void state_57(void)
{

}

void state_58(void)
{
    /* LDI (3/5) */
    cpu.mar = cpu.mdr;
}

void state_59(void)
{

}

void state_60(void)
{
    /* STI (2/5) */
    mem_read(&cpu.mdr, cpu.mar);
}

void state_61(void)
{

}

void state_62(void)
{
    /* STI (3/5) */
    cpu.mar = cpu.mdr;
}

void state_63(void)
{

}
