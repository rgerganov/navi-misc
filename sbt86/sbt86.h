/* -*- Mode: C; c-basic-offset: 4 -*-
 *
 * Definitions and data types for the code generated by sbt86, an
 * experimental 8086 -> C static binary translator.
 *
 * Copyright (c) 2009 Micah Dowty <micah@navi.cx>
 *
 *    Permission is hereby granted, free of charge, to any person
 *    obtaining a copy of this software and associated documentation
 *    files (the "Software"), to deal in the Software without
 *    restriction, including without limitation the rights to use,
 *    copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following
 *    conditions:
 *
 *    The above copyright notice and this permission notice shall be
 *    included in all copies or substantial portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *    OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __SBT86_H__
#define __SBT86_H__

#include <stdint.h>
#include <endian.h>
#include <string.h>
#include <stdlib.h>
#include <setjmp.h>


/*****************************************************************
 * Definitions
 */

#if BYTE_ORDER == LITTLE_ENDIAN
#define DEF_WORD_REG(o)                         \
    union {                                     \
        uint16_t o##x;                          \
        struct {                                \
            uint8_t o##l, o##h;                 \
        };                                      \
    }
#define DEF_FLAG_BITS                           \
    uint16_t c : 1;                             \
    uint16_t res0 : 1;                          \
    uint16_t p : 1;                             \
    uint16_t res1 : 1;                          \
    uint16_t a : 1;                             \
    uint16_t res2 : 1;                          \
    uint16_t z : 1;                             \
    uint16_t s : 1;                             \
    uint16_t t : 1;                             \
    uint16_t i : 1;                             \
    uint16_t d : 1;                             \
    uint16_t o : 1;                             \
    uint16_t res3 : 4;
#endif

#if BYTE_ORDER == BIG_ENDIAN
#define DEF_WORD_REG(o)                         \
    union {                                     \
        uint16_t o##x;                          \
        struct {                                \
            uint8_t o##h, o##l;                 \
        };                                      \
    }
#define DEF_FLAG_BITS                           \
    uint16_t t : 1;                             \
    uint16_t i : 1;                             \
    uint16_t d : 1;                             \
    uint16_t o : 1;                             \
    uint16_t res3 : 4;                          \
    uint16_t c : 1;                             \
    uint16_t res0 : 1;                          \
    uint16_t p : 1;                             \
    uint16_t res1 : 1;                          \
    uint16_t a : 1;                             \
    uint16_t res2 : 1;                          \
    uint16_t z : 1;                             \
    uint16_t s : 1;
#endif

typedef struct Regs {
    DEF_WORD_REG(a);
    DEF_WORD_REG(b);
    DEF_WORD_REG(c);
    DEF_WORD_REG(d);
    uint16_t si, di;
    uint16_t cs, ds, es, ss;
    uint16_t bp, sp;

    /*
     * We cheat enormously on implementing 8086 flags: Instead of
     * calculating the flags for every ALU instruction, we store a
     * 32-bit version of that instruction's result.  All flag tests
     * are rewritten in terms of this result word. Anything that
     * explicitly sets flags does so by tweaking this result word in
     * such a way as to change the flag value we would calculate.
     *
     * To avoid having to store the word width separately, all 8-bit
     * results are left-shifted by 8.
     */
    uint32_t uresult;
    int32_t sresult;

    /*
     * As another trick to make generated code smaller, we cache
     * pointers to the memory behind all segment registers.  Any time
     * we write to a segment registers, this cache is also updated.
     * (This is much more like how the x86 works in protected mode,
     * and it saves us the SEG() calculation on every memory access.)
     */
    struct {
        uint8_t *cs;
        uint8_t *ds;
        uint8_t *es;
        uint8_t *ss;
    } ptr;

} Regs;

typedef union StackItem {
    uint16_t  word;
    struct {
        uint32_t  uresult;
        int32_t   sresult;
    };
} StackItem;


/*****************************************************************
 * Functions and data defined in sbt86.c
 */

void decompressRLE(uint8_t *dest, uint8_t *src, uint32_t srcLength);

void failedDynamicBranch(uint16_t cs, uint16_t ip, uint32_t value);

Regs int10(Regs reg);
Regs int16(Regs reg);
Regs int21(Regs reg);
uint8_t in(uint16_t port, uint32_t timestamp);
void out(uint16_t port, uint8_t value, uint32_t timestamp);

void consoleBlitToScreen(uint8_t *fb);

extern uint8_t mem[];
extern jmp_buf dosExitJump;


/*****************************************************************
 * Inline functions and macors for use in translated code.
 */

/*
 * For global segment/offset calculations. If you're loading or
 * storing relative to a segment register, use the reg.ptr pointers
 * instead.
 */

static inline
SEG(uint16_t seg, uint16_t off)
{
    /*
     * This needs to be an inline. A macro was provoking gcc to
     * use signed subtraction for offsets >= 0x8000...
     */
    uint32_t off32 = off;
    uint32_t seg32 = seg;

    return (seg32 << 4) + off32;
}

/*
 * Convert an (8-bit) lvalue into a 16-bit lvalue.
 */

#define M16(x)  (*(uint16_t*)&(x))

/*
 * Calculate/set flag values
 */

#define ZF   ((reg.uresult & 0xFFFF) == 0)
#define SF   ((reg.uresult & 0x8000) != 0)
#define OF   ((((reg.sresult >> 1) ^ (reg.sresult)) & 0x8000) != 0)  // Signed
#define CF   ((reg.uresult & 0x10000) != 0)                          // Unsigned

#define SET_ZF  reg.uresult &= ~0xFFFF
#define CLR_ZF  reg.uresult |= 1
#define SET_OF  reg.sresult = 0x8000
#define CLR_OF  reg.sresult = 0
#define SET_CF  reg.uresult |= 0x10000
#define CLR_CF  reg.uresult &= 0xFFFF

#endif /* __SBT86_H__ */