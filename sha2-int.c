//
//  sha-256-amd64.c
//  sha
//
//  Created by Tomas Carnecky on 7/27/10.
//  Copyright (c) 2010 __MyCompanyName__. All rights reserved.
//

#include "sha2-int.h"

#include <xmmintrin.h>
#include <stdint.h>

#define	SHA256_CONST_0		0x428a2f98U
#define	SHA256_CONST_1		0x71374491U
#define	SHA256_CONST_2		0xb5c0fbcfU
#define	SHA256_CONST_3		0xe9b5dba5U
#define	SHA256_CONST_4		0x3956c25bU
#define	SHA256_CONST_5		0x59f111f1U
#define	SHA256_CONST_6		0x923f82a4U
#define	SHA256_CONST_7		0xab1c5ed5U

#define	SHA256_CONST_8		0xd807aa98U
#define	SHA256_CONST_9		0x12835b01U
#define	SHA256_CONST_10		0x243185beU
#define	SHA256_CONST_11		0x550c7dc3U
#define	SHA256_CONST_12		0x72be5d74U
#define	SHA256_CONST_13		0x80deb1feU
#define	SHA256_CONST_14		0x9bdc06a7U
#define	SHA256_CONST_15		0xc19bf174U

#define	SHA256_CONST_16		0xe49b69c1U
#define	SHA256_CONST_17		0xefbe4786U
#define	SHA256_CONST_18		0x0fc19dc6U
#define	SHA256_CONST_19		0x240ca1ccU
#define	SHA256_CONST_20		0x2de92c6fU
#define	SHA256_CONST_21		0x4a7484aaU
#define	SHA256_CONST_22		0x5cb0a9dcU
#define	SHA256_CONST_23		0x76f988daU

#define	SHA256_CONST_24		0x983e5152U
#define	SHA256_CONST_25		0xa831c66dU
#define	SHA256_CONST_26		0xb00327c8U
#define	SHA256_CONST_27		0xbf597fc7U
#define	SHA256_CONST_28		0xc6e00bf3U
#define	SHA256_CONST_29		0xd5a79147U
#define	SHA256_CONST_30		0x06ca6351U
#define	SHA256_CONST_31		0x14292967U

#define	SHA256_CONST_32		0x27b70a85U
#define	SHA256_CONST_33		0x2e1b2138U
#define	SHA256_CONST_34		0x4d2c6dfcU
#define	SHA256_CONST_35		0x53380d13U
#define	SHA256_CONST_36		0x650a7354U
#define	SHA256_CONST_37		0x766a0abbU
#define	SHA256_CONST_38		0x81c2c92eU
#define	SHA256_CONST_39		0x92722c85U

#define	SHA256_CONST_40		0xa2bfe8a1U
#define	SHA256_CONST_41		0xa81a664bU
#define	SHA256_CONST_42		0xc24b8b70U
#define	SHA256_CONST_43		0xc76c51a3U
#define	SHA256_CONST_44		0xd192e819U
#define	SHA256_CONST_45		0xd6990624U
#define	SHA256_CONST_46		0xf40e3585U
#define	SHA256_CONST_47		0x106aa070U

#define	SHA256_CONST_48		0x19a4c116U
#define	SHA256_CONST_49		0x1e376c08U
#define	SHA256_CONST_50		0x2748774cU
#define	SHA256_CONST_51		0x34b0bcb5U
#define	SHA256_CONST_52		0x391c0cb3U
#define	SHA256_CONST_53		0x4ed8aa4aU
#define	SHA256_CONST_54		0x5b9cca4fU
#define	SHA256_CONST_55		0x682e6ff3U

#define	SHA256_CONST_56		0x748f82eeU
#define	SHA256_CONST_57		0x78a5636fU
#define	SHA256_CONST_58		0x84c87814U
#define	SHA256_CONST_59		0x8cc70208U
#define	SHA256_CONST_60		0x90befffaU
#define	SHA256_CONST_61		0xa4506cebU
#define	SHA256_CONST_62		0xbef9a3f7U
#define	SHA256_CONST_63		0xc67178f2U

static const uint32_t sha256_consts[] = {
    SHA256_CONST_0, SHA256_CONST_1, SHA256_CONST_2,
    SHA256_CONST_3, SHA256_CONST_4, SHA256_CONST_5,
    SHA256_CONST_6, SHA256_CONST_7, SHA256_CONST_8,
    SHA256_CONST_9, SHA256_CONST_10, SHA256_CONST_11,
    SHA256_CONST_12, SHA256_CONST_13, SHA256_CONST_14,
    SHA256_CONST_15, SHA256_CONST_16, SHA256_CONST_17,
    SHA256_CONST_18, SHA256_CONST_19, SHA256_CONST_20,
    SHA256_CONST_21, SHA256_CONST_22, SHA256_CONST_23,
    SHA256_CONST_24, SHA256_CONST_25, SHA256_CONST_26,
    SHA256_CONST_27, SHA256_CONST_28, SHA256_CONST_29,
    SHA256_CONST_30, SHA256_CONST_31, SHA256_CONST_32,
    SHA256_CONST_33, SHA256_CONST_34, SHA256_CONST_35,
    SHA256_CONST_36, SHA256_CONST_37, SHA256_CONST_38,
    SHA256_CONST_39, SHA256_CONST_40, SHA256_CONST_41,
    SHA256_CONST_42, SHA256_CONST_43, SHA256_CONST_44,
    SHA256_CONST_45, SHA256_CONST_46, SHA256_CONST_47,
    SHA256_CONST_48, SHA256_CONST_49, SHA256_CONST_50,
    SHA256_CONST_51, SHA256_CONST_52, SHA256_CONST_53,
    SHA256_CONST_54, SHA256_CONST_55, SHA256_CONST_56,
    SHA256_CONST_57, SHA256_CONST_58, SHA256_CONST_59,
    SHA256_CONST_60, SHA256_CONST_61, SHA256_CONST_62,
    SHA256_CONST_63
};

// #define Ch(b, c, d)	(((b) & (c)) ^ ((~b) & (d)))
static inline __m128i Ch(__m128i b, __m128i c, __m128i d) {
    return (b & c) ^ (~b & d);
}

// #define	Maj(b, c, d)	(((b) & (c)) ^ ((b) & (d)) ^ ((c) & (d)))
static inline __m128i Maj(__m128i b, __m128i c, __m128i d) {
    return (b & c) ^ (b & d) ^ (c & d);
}

static inline __m128i ROTR(__m128i x, int n) {
    return _mm_srli_epi32(x, n) | _mm_slli_epi32(x, 32 - n);
}

static inline __m128i SHR(__m128i x, int n) {
    return _mm_srli_epi32(x, n);
}

/* SHA256 Functions */
#define	BIGSIGMA0_256(x)	(ROTR((x), 2) ^ ROTR((x), 13) ^ ROTR((x), 22))
#define	BIGSIGMA1_256(x)	(ROTR((x), 6) ^ ROTR((x), 11) ^ ROTR((x), 25))
#define	SIGMA0_256(x)		(ROTR((x), 7) ^ ROTR((x), 18) ^ SHR((x), 3))
#define	SIGMA1_256(x)		(ROTR((x), 17) ^ ROTR((x), 19) ^ SHR((x), 10))

static inline __m128i load32(uint32_t x) {
    union { __m128i ret; uint32_t x[4]; } box;
    box.x[0] = x;
    return box.ret;
}

static inline __m128i load_epi32(uint32_t x0, uint32_t x1, uint32_t x2, uint32_t x3) {
    union { __m128i ret; uint32_t x[4]; } box = { .x = { x0, x1, x2, x3 } };
    return box.ret;
}

static inline uint32_t store32(__m128i x) {
    union { uint32_t ret[0]; __m128i x; } box;
    box.x = x;
    return box.ret[0];
}

static inline void store_epi32(__m128i x, uint32_t *x0, uint32_t *x1, uint32_t *x2, uint32_t *x3) {
    union { uint32_t ret[4]; __m128i x; } box = { .x = x };
    *x0 = box.ret[0]; *x1 = box.ret[1]; *x2 = box.ret[2]; *x3 = box.ret[3];
}

static inline __m128i SHA256_CONST(int i) {
    return load_epi32(sha256_consts[i], sha256_consts[i], sha256_consts[i], sha256_consts[i]);
}

#define add4(x0, x1, x2, x3) _mm_add_epi32(_mm_add_epi32(_mm_add_epi32(x0, x1), x2), x3)
#define add5(x0, x1, x2, x3, x4) _mm_add_epi32(add4(x0, x1, x2, x3), x4)


#define	SHA256ROUND(a, b, c, d, e, f, g, h, i, w)                       \
    T1 = add5(h, BIGSIGMA1_256(e), Ch(e, f, g), SHA256_CONST(i), w);	\
    d = _mm_add_epi32(d, T1);                                           \
    T2 = _mm_add_epi32(BIGSIGMA0_256(a), Maj(a, b, c));                 \
    h = _mm_add_epi32(T1, T2);

static inline uint32_t SWAP32(const void *addr) {
	return htonl(*((uint32_t *)(addr)));
}

static inline __m128i LOAD(const __sha256_block_t *blk[4], int i) {
    return load_epi32(SWAP32(*blk[0] + i * 4), SWAP32(*blk[1] + i * 4), SWAP32(*blk[2] + i * 4), SWAP32(*blk[3] + i * 4));
}

static inline void dumpreg(__m128i x, char *msg) {
    union { uint32_t ret[4]; __m128i x; } box = { .x = x };
    printf("%s %08x %08x %08x %08x\n", msg, box.ret[0], box.ret[1], box.ret[2], box.ret[3]);
}

#if 0
#define dumpstate() printf("%s: %08x %08x %08x %08x %08x %08x %08x %08x %08x\n", \
__func__, store32(w0), store32(a), store32(b), store32(c), store32(d), store32(e), store32(f), store32(g), store32(h));
#else
#define dumpstate()
#endif

void __sha256_int(const __sha256_block_t *blk[4], __sha256_hash_t *hash[4])
{
    __sha256_hash_t *h0 = hash[0], *h1 = hash[1], *h2 = hash[2], *h3 = hash[3];
#define load(x, i) __m128i x = load_epi32((*h0)[i], (*h1)[i], (*h2)[i], (*h3)[i])

    load(a, 0);
    load(b, 1);
    load(c, 2);
    load(d, 3);
    load(e, 4);
    load(f, 5);
    load(g, 6);
    load(h, 7);    
    
    
    __m128i w0, w1, w2, w3, w4, w5, w6, w7;
    __m128i w8, w9, w10, w11, w12, w13, w14, w15;
    __m128i T1, T2;

    
    /* LINTED E_BAD_PTR_CAST_ALIGN */
    w0 =  LOAD(blk, 0);
	dumpstate();
    SHA256ROUND(a, b, c, d, e, f, g, h, 0, w0);
    
    /* LINTED E_BAD_PTR_CAST_ALIGN */
    w1 =  LOAD(blk, 1);
    SHA256ROUND(h, a, b, c, d, e, f, g, 1, w1);
    /* LINTED E_BAD_PTR_CAST_ALIGN */
    w2 =  LOAD(blk, 2);
    SHA256ROUND(g, h, a, b, c, d, e, f, 2, w2);
    /* LINTED E_BAD_PTR_CAST_ALIGN */
    w3 =  LOAD(blk, 3);
    SHA256ROUND(f, g, h, a, b, c, d, e, 3, w3);
    /* LINTED E_BAD_PTR_CAST_ALIGN */
    w4 =  LOAD(blk, 4);
    SHA256ROUND(e, f, g, h, a, b, c, d, 4, w4);
    /* LINTED E_BAD_PTR_CAST_ALIGN */
    w5 =  LOAD(blk, 5);
    SHA256ROUND(d, e, f, g, h, a, b, c, 5, w5);
    /* LINTED E_BAD_PTR_CAST_ALIGN */
    w6 =  LOAD(blk, 6);
    SHA256ROUND(c, d, e, f, g, h, a, b, 6, w6);
    /* LINTED E_BAD_PTR_CAST_ALIGN */
    w7 =  LOAD(blk, 7);
    SHA256ROUND(b, c, d, e, f, g, h, a, 7, w7);
    /* LINTED E_BAD_PTR_CAST_ALIGN */
    w8 =  LOAD(blk, 8);
    SHA256ROUND(a, b, c, d, e, f, g, h, 8, w8);
    /* LINTED E_BAD_PTR_CAST_ALIGN */
    w9 =  LOAD(blk, 9);
    SHA256ROUND(h, a, b, c, d, e, f, g, 9, w9);
    /* LINTED E_BAD_PTR_CAST_ALIGN */
    w10 =  LOAD(blk, 10);
    SHA256ROUND(g, h, a, b, c, d, e, f, 10, w10);
    /* LINTED E_BAD_PTR_CAST_ALIGN */
    w11 =  LOAD(blk, 11);
    SHA256ROUND(f, g, h, a, b, c, d, e, 11, w11);
    /* LINTED E_BAD_PTR_CAST_ALIGN */
    w12 =  LOAD(blk, 12);
    SHA256ROUND(e, f, g, h, a, b, c, d, 12, w12);
    /* LINTED E_BAD_PTR_CAST_ALIGN */
    w13 =  LOAD(blk, 13);
    SHA256ROUND(d, e, f, g, h, a, b, c, 13, w13);
    /* LINTED E_BAD_PTR_CAST_ALIGN */
    w14 =  LOAD(blk, 14);
    SHA256ROUND(c, d, e, f, g, h, a, b, 14, w14);
    /* LINTED E_BAD_PTR_CAST_ALIGN */
    w15 =  LOAD(blk, 15);
    SHA256ROUND(b, c, d, e, f, g, h, a, 15, w15);
    
    w0 = add4(SIGMA1_256(w14), w9, SIGMA0_256(w1), w0);
	dumpstate();
    SHA256ROUND(a, b, c, d, e, f, g, h, 16, w0);
    w1 = add4(SIGMA1_256(w15), w10, SIGMA0_256(w2), w1);
    SHA256ROUND(h, a, b, c, d, e, f, g, 17, w1);
    w2 = add4(SIGMA1_256(w0), w11, SIGMA0_256(w3), w2);
    SHA256ROUND(g, h, a, b, c, d, e, f, 18, w2);
    w3 = add4(SIGMA1_256(w1), w12, SIGMA0_256(w4), w3);
    SHA256ROUND(f, g, h, a, b, c, d, e, 19, w3);
    w4 = add4(SIGMA1_256(w2), w13, SIGMA0_256(w5), w4);
    SHA256ROUND(e, f, g, h, a, b, c, d, 20, w4);
    w5 = add4(SIGMA1_256(w3), w14, SIGMA0_256(w6), w5);
    SHA256ROUND(d, e, f, g, h, a, b, c, 21, w5);
    w6 = add4(SIGMA1_256(w4), w15, SIGMA0_256(w7), w6);
    SHA256ROUND(c, d, e, f, g, h, a, b, 22, w6);
    w7 = add4(SIGMA1_256(w5), w0, SIGMA0_256(w8), w7);
    SHA256ROUND(b, c, d, e, f, g, h, a, 23, w7);
    w8 = add4(SIGMA1_256(w6), w1, SIGMA0_256(w9), w8);
    SHA256ROUND(a, b, c, d, e, f, g, h, 24, w8);
    w9 = add4(SIGMA1_256(w7), w2, SIGMA0_256(w10), w9);
    SHA256ROUND(h, a, b, c, d, e, f, g, 25, w9);
    w10 = add4(SIGMA1_256(w8), w3, SIGMA0_256(w11), w10);
    SHA256ROUND(g, h, a, b, c, d, e, f, 26, w10);
    w11 = add4(SIGMA1_256(w9), w4, SIGMA0_256(w12), w11);
    SHA256ROUND(f, g, h, a, b, c, d, e, 27, w11);
    w12 = add4(SIGMA1_256(w10), w5, SIGMA0_256(w13), w12);
    SHA256ROUND(e, f, g, h, a, b, c, d, 28, w12);
    w13 = add4(SIGMA1_256(w11), w6, SIGMA0_256(w14), w13);
    SHA256ROUND(d, e, f, g, h, a, b, c, 29, w13);
    w14 = add4(SIGMA1_256(w12), w7, SIGMA0_256(w15), w14);
    SHA256ROUND(c, d, e, f, g, h, a, b, 30, w14);
    w15 = add4(SIGMA1_256(w13), w8, SIGMA0_256(w0), w15);
    SHA256ROUND(b, c, d, e, f, g, h, a, 31, w15);
    
    w0 = add4(SIGMA1_256(w14), w9, SIGMA0_256(w1), w0);
    dumpstate();
    SHA256ROUND(a, b, c, d, e, f, g, h, 32, w0);
    w1 = add4(SIGMA1_256(w15), w10, SIGMA0_256(w2), w1);
    SHA256ROUND(h, a, b, c, d, e, f, g, 33, w1);
    w2 = add4(SIGMA1_256(w0), w11, SIGMA0_256(w3), w2);
    SHA256ROUND(g, h, a, b, c, d, e, f, 34, w2);
    w3 = add4(SIGMA1_256(w1), w12, SIGMA0_256(w4), w3);
    SHA256ROUND(f, g, h, a, b, c, d, e, 35, w3);
    w4 = add4(SIGMA1_256(w2), w13, SIGMA0_256(w5), w4);
    SHA256ROUND(e, f, g, h, a, b, c, d, 36, w4);
    w5 = add4(SIGMA1_256(w3), w14, SIGMA0_256(w6), w5);
    SHA256ROUND(d, e, f, g, h, a, b, c, 37, w5);
    w6 = add4(SIGMA1_256(w4), w15, SIGMA0_256(w7), w6);
    SHA256ROUND(c, d, e, f, g, h, a, b, 38, w6);
    w7 = add4(SIGMA1_256(w5), w0, SIGMA0_256(w8), w7);
    SHA256ROUND(b, c, d, e, f, g, h, a, 39, w7);
    w8 = add4(SIGMA1_256(w6), w1, SIGMA0_256(w9), w8);
    SHA256ROUND(a, b, c, d, e, f, g, h, 40, w8);
    w9 = add4(SIGMA1_256(w7), w2, SIGMA0_256(w10), w9);
    SHA256ROUND(h, a, b, c, d, e, f, g, 41, w9);
    w10 = add4(SIGMA1_256(w8), w3, SIGMA0_256(w11), w10);
    SHA256ROUND(g, h, a, b, c, d, e, f, 42, w10);
    w11 = add4(SIGMA1_256(w9), w4, SIGMA0_256(w12), w11);
    SHA256ROUND(f, g, h, a, b, c, d, e, 43, w11);
    w12 = add4(SIGMA1_256(w10), w5, SIGMA0_256(w13), w12);
    SHA256ROUND(e, f, g, h, a, b, c, d, 44, w12);
    w13 = add4(SIGMA1_256(w11), w6, SIGMA0_256(w14), w13);
    SHA256ROUND(d, e, f, g, h, a, b, c, 45, w13);
    w14 = add4(SIGMA1_256(w12), w7, SIGMA0_256(w15), w14);
    SHA256ROUND(c, d, e, f, g, h, a, b, 46, w14);
    w15 = add4(SIGMA1_256(w13), w8, SIGMA0_256(w0), w15);
    SHA256ROUND(b, c, d, e, f, g, h, a, 47, w15);
    
    w0 = add4(SIGMA1_256(w14), w9, SIGMA0_256(w1), w0);
    dumpstate();
    SHA256ROUND(a, b, c, d, e, f, g, h, 48, w0);
    w1 = add4(SIGMA1_256(w15), w10, SIGMA0_256(w2), w1);
    SHA256ROUND(h, a, b, c, d, e, f, g, 49, w1);
    w2 = add4(SIGMA1_256(w0), w11, SIGMA0_256(w3), w2);
    SHA256ROUND(g, h, a, b, c, d, e, f, 50, w2);
    w3 = add4(SIGMA1_256(w1), w12, SIGMA0_256(w4), w3);
    SHA256ROUND(f, g, h, a, b, c, d, e, 51, w3);
    w4 = add4(SIGMA1_256(w2), w13, SIGMA0_256(w5), w4);
    SHA256ROUND(e, f, g, h, a, b, c, d, 52, w4);
    w5 = add4(SIGMA1_256(w3), w14, SIGMA0_256(w6), w5);
    SHA256ROUND(d, e, f, g, h, a, b, c, 53, w5);
    w6 = add4(SIGMA1_256(w4), w15, SIGMA0_256(w7), w6);
    SHA256ROUND(c, d, e, f, g, h, a, b, 54, w6);
    w7 = add4(SIGMA1_256(w5), w0, SIGMA0_256(w8), w7);
    SHA256ROUND(b, c, d, e, f, g, h, a, 55, w7);
    w8 = add4(SIGMA1_256(w6), w1, SIGMA0_256(w9), w8);
    SHA256ROUND(a, b, c, d, e, f, g, h, 56, w8);
    w9 = add4(SIGMA1_256(w7), w2, SIGMA0_256(w10), w9);
    SHA256ROUND(h, a, b, c, d, e, f, g, 57, w9);
    w10 = add4(SIGMA1_256(w8), w3, SIGMA0_256(w11), w10);
    SHA256ROUND(g, h, a, b, c, d, e, f, 58, w10);
    w11 = add4(SIGMA1_256(w9), w4, SIGMA0_256(w12), w11);
    SHA256ROUND(f, g, h, a, b, c, d, e, 59, w11);
    w12 = add4(SIGMA1_256(w10), w5, SIGMA0_256(w13), w12);
    SHA256ROUND(e, f, g, h, a, b, c, d, 60, w12);
    w13 = add4(SIGMA1_256(w11), w6, SIGMA0_256(w14), w13);
    SHA256ROUND(d, e, f, g, h, a, b, c, 61, w13);
    w14 = add4(SIGMA1_256(w12), w7, SIGMA0_256(w15), w14);
    SHA256ROUND(c, d, e, f, g, h, a, b, 62, w14);
    w15 = add4(SIGMA1_256(w13), w8, SIGMA0_256(w0), w15);
    SHA256ROUND(b, c, d, e, f, g, h, a, 63, w15);
    
    //dumpreg(d, "last d");

	dumpstate();


#define store(x,i)  \
    w0 = load_epi32((*h0)[i], (*h1)[i], (*h2)[i], (*h3)[i]); \
    w1 = _mm_add_epi32(w0, x); \
    store_epi32(w1, &(*h0)[i], &(*h1)[i], &(*h2)[i], &(*h3)[i]);

    //printf("%s a: %08x %08x\n", __func__, store32(a), *h0[0]);
    store(a, 0);
    //printf("%s a: %08x\n", __func__, *h0[0]);
    store(b, 1);
    store(c, 2);
    store(d, 3);
    store(e, 4);
    store(f, 5);
    store(g, 6);
    store(h, 7);
}