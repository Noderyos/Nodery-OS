#include "types.h"

#define HI32(x)   ((uint32_t)((uint64_t)(x) >> 32))
#define LO32(x)   ((uint32_t)((uint64_t)(x)))

static void sub64(uint32_t *ahi, uint32_t *alo, uint32_t bhi, uint32_t blo) {
    uint32_t lo = *alo - blo;
    uint32_t borrow = (lo > *alo);
    uint32_t hi = *ahi - bhi - borrow;

    *alo = lo;
    *ahi = hi;
}

static int ge64(uint32_t ahi, uint32_t alo, uint32_t bhi, uint32_t blo) {
    if (ahi != bhi) return ahi > bhi;
    return alo >= blo;
}

static void shl1_64(uint32_t *hi, uint32_t *lo) {
    uint32_t new_hi = (*hi << 1) | (*lo >> 31);
    uint32_t new_lo = (*lo << 1);
    *hi = new_hi;
    *lo = new_lo;
}

static uint32_t getbit64(uint64_t x, int i) {
    if (i < 32) return (HI32(x) >> (31 - i)) & 1;
    else return (LO32(x) >> (63 - i)) & 1;
}

static void udiv64(uint64_t num, uint64_t den, uint64_t *qout, uint64_t *rout) {
    uint32_t q_hi = 0, q_lo = 0;
    uint32_t r_hi = 0, r_lo = 0;

    uint32_t d_hi = HI32(den);
    uint32_t d_lo = LO32(den);

    for (int i = 0; i < 64; i++) {
        shl1_64(&r_hi, &r_lo);
        r_lo |= getbit64(num, i);

        if (ge64(r_hi, r_lo, d_hi, d_lo)) {
            sub64(&r_hi, &r_lo, d_hi, d_lo);

            if (i < 32) q_hi |= 1u << (31 - i);
            else        q_lo |= 1u << (63 - i);
        }
    }

    *qout = ((uint64_t)q_hi << 32) | q_lo;
    *rout = ((uint64_t)r_hi << 32) | r_lo;
}

static void neg64(uint32_t *hi, uint32_t *lo) {
    uint32_t lo2 = ~(*lo) + 1;
    uint32_t carry = (lo2 == 0);
    uint32_t hi2 = ~(*hi) + carry;

    *hi = hi2;
    *lo = lo2;
}

void idiv64(int64_t num, int64_t den, int64_t *qout, int64_t *rout) {
    int neg_q = ((num < 0) ^ (den < 0));
    int neg_r = (num < 0);

    uint32_t a_hi = HI32(num), a_lo = LO32(num);
    uint32_t b_hi = HI32(den), b_lo = LO32(den);

    if (num < 0) neg64(&a_hi, &a_lo);
    if (den < 0) neg64(&b_hi, &b_lo);

    uint64_t ua = ((uint64_t)a_hi << 32) | a_lo;
    uint64_t ub = ((uint64_t)b_hi << 32) | b_lo;

    uint64_t uq, ur;
    udiv64(ua, ub, &uq, &ur);

    int64_t q = (int64_t)uq;
    int64_t r = (int64_t)ur;

    if (neg_q) q = -q;
    if (neg_r) r = -r;

    *qout = q;
    *rout = r;
}

uint64_t __udivdi3(uint64_t n, uint64_t d) {
    uint64_t q, r;
    udiv64(n, d, &q, &r);
    return q;
}

int64_t __divdi3(int64_t n, int64_t d) {
    int64_t q, r;
    idiv64(n, d, &q, &r);
    return q;
}

uint64_t __umoddi3(uint64_t n, uint64_t d) {
    uint64_t q, r;
    udiv64(n, d, &q, &r);
    return r;
}

int64_t __moddi3(int64_t n, int64_t d) {
    int64_t q, r;
    idiv64(n, d, &q, &r);
    return r;
}

