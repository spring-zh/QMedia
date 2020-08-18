/*
 * copyright (c) 2004 Michael Niedermayer <michaelni@gmx.at>
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file bitstream.h
 * bitstream api header.
 */

#ifndef BITSTREAM_H
#define BITSTREAM_H

#define av_always_inline inline
#define attribute_deprecated

#include <inttypes.h>
#include <stdlib.h>
#include <limits.h>

#ifdef ROCKBOX_BIG_ENDIAN
#define WORDS_BIGENDIAN
#endif

#ifdef __cplusplus
extern "C"{
#endif

#ifdef _WIN32
#define inline _inline
#endif

#ifndef HAVE_BYTESWAP
#define HAVE_BYTESWAP

#define bswap_16(x) (((x) & 0x00ff) << 8 | ((x) & 0xff00) >> 8)

// code from bits/byteswap.h (C) 1997, 1998 Free Software Foundation, Inc.
#define bswap_32(x) \
	((((x)& 0xff000000) >> 24) | (((x)& 0x00ff0000) >> 8) | \
	(((x)& 0x0000ff00) << 8) | (((x)& 0x000000ff) << 24))

static inline uint64_t ByteSwap64(uint64_t x)
{
	union {
		uint64_t ll;
		uint32_t l[2];
	} w, r;
	w.ll = x;
	r.l[0] = bswap_32(w.l[1]);
	r.l[1] = bswap_32(w.l[0]);
	return r.ll;
}
#define bswap_64(x) ByteSwap64(x)

static inline unsigned av_mod_uintp2(unsigned a, unsigned p)
{
	return a & ((1 << p) - 1);
}

// be2me ... BigEndian to MachineEndian
// le2me ... LittleEndian to MachineEndian

#ifdef WORDS_BIGENDIAN
#define be2me_16(x) (x)
#define be2me_32(x) (x)
#define be2me_64(x) (x)
#define le2me_16(x) bswap_16(x)
#define le2me_32(x) bswap_32(x)
#define le2me_64(x) bswap_64(x)
#else
#define be2me_16(x) bswap_16(x)
#define be2me_32(x) bswap_32(x)
#define be2me_64(x) bswap_64(x)
#define le2me_16(x) (x)
#define le2me_32(x) (x)
#define le2me_64(x) (x)
#endif

#endif

#define UNCHECKED_BITSTREAM_READER 1
#if UNCHECKED_BITSTREAM_READER
//#define OPEN_READER(name, gb) OPEN_READER_NOSIZE(name, gb)

#define BITS_AVAILABLE(name, gb) 1
#else
#define OPEN_READER(name, gb)                   \
	OPEN_READER_NOSIZE(name, gb);               \
	unsigned int name ## _size_plus8 = (gb)->size_in_bits_plus8

#define BITS_AVAILABLE(name, gb) name ## _index < name ## _size_plus8
#endif

extern const uint8_t ff_golomb_vlc_len[512];
extern const uint8_t ff_ue_golomb_vlc_code[512];
extern const  int8_t ff_se_golomb_vlc_code[512];
extern const uint8_t ff_ue_golomb_len[256];

extern const uint8_t ff_interleaved_golomb_vlc_len[256];
extern const uint8_t ff_interleaved_ue_golomb_vlc_code[256];
extern const  int8_t ff_interleaved_se_golomb_vlc_code[256];
extern const uint8_t ff_interleaved_dirac_golomb_vlc_code[256];

extern const uint8_t ff_log2_tab[256];

/*misc utility functions added to make it compile */
static av_always_inline int av_log2(unsigned int v)
{
    int n;

    n = 0;
    if (v & 0xffff0000) {
        v >>= 16;
        n += 16;
    }
    if (v & 0xff00) {
        v >>= 8;
        n += 8;
    }
    n += ff_log2_tab[v];

    return n;
}

#if defined(ALT_BITSTREAM_READER_LE) && !defined(ALT_BITSTREAM_READER)
#define ALT_BITSTREAM_READER
#endif

//#define ALT_BITSTREAM_WRITER
//#define ALIGNED_BITSTREAM_WRITER
#if !defined(LIBMPEG2_BITSTREAM_READER) && !defined(A32_BITSTREAM_READER) && !defined(ALT_BITSTREAM_READER)
#   ifdef ARCH_ARMV4L
#       define A32_BITSTREAM_READER
#   else
#define ALT_BITSTREAM_READER
//#define LIBMPEG2_BITSTREAM_READER
//#define A32_BITSTREAM_READER
#   endif
#endif
#define LIBMPEG2_BITSTREAM_READER_HACK //add BERO

extern const uint8_t ff_reverse[256];

#if defined(ARCH_X86)
// avoid +32 for shift optimization (gcc should do that ...)
static inline  int32_t NEG_SSR32( int32_t a, int8_t s){
    asm ("sarl %1, %0\n\t"
         : "+r" (a)
         : "ic" ((uint8_t)(-s))
    );
    return a;
}
static inline uint32_t NEG_USR32(uint32_t a, int8_t s){
    asm ("shrl %1, %0\n\t"
         : "+r" (a)
         : "ic" ((uint8_t)(-s))
    );
    return a;
}
#else
#    define NEG_SSR32(a,s) ((( int32_t)(a))>>(32-(s)))
#    define NEG_USR32(a,s) (((uint32_t)(a))>>(32-(s)))
#endif

/* bit output */

/* buf and buf_end must be present and used by every alternative writer. */
typedef struct PutBitContext {
#ifdef ALT_BITSTREAM_WRITER
    uint8_t *buf, *buf_end;
    int index;
#else
    uint32_t bit_buf;
    int bit_left;
    uint8_t *buf, *buf_ptr, *buf_end;
#endif
} PutBitContext;

static inline void init_put_bits(PutBitContext *s, uint8_t *buffer, int buffer_size)
{
    if(buffer_size < 0) {
        buffer_size = 0;
        buffer = NULL;
    }

    s->buf = buffer;
    s->buf_end = s->buf + buffer_size;
#ifdef ALT_BITSTREAM_WRITER
    s->index=0;
    ((uint32_t*)(s->buf))[0]=0;
//    memset(buffer, 0, buffer_size);
#else
    s->buf_ptr = s->buf;
    s->bit_left=32;
    s->bit_buf=0;
#endif
}

/* return the number of bits output */
static inline int put_bits_count(PutBitContext *s)
{
#ifdef ALT_BITSTREAM_WRITER
    return s->index;
#else
    return (s->buf_ptr - s->buf) * 8 + 32 - s->bit_left;
#endif
}

/* pad the end of the output stream with zeros */
static inline void flush_put_bits(PutBitContext *s)
{
#ifdef ALT_BITSTREAM_WRITER
    align_put_bits(s);
#else
    s->bit_buf<<= s->bit_left;
    while (s->bit_left < 32) {
        /* XXX: should test end of buffer */
        *s->buf_ptr++=s->bit_buf >> 24;
        s->bit_buf<<=8;
        s->bit_left+=8;
    }
    s->bit_left=32;
    s->bit_buf=0;
#endif
}

static inline void align_put_bits(PutBitContext *s)
{
	/*if (s->bit_left != 8)
	{
		s->bit_left = 8;
		s->buf_ptr++;
	}*/
	if ((s->bit_left &0x7) != 0)
	{
		s->bit_left = s->bit_left & (~0x7) + 8;
		//s->buf_ptr++;
	}
}
static inline void align_put_bits32(PutBitContext *s)
{
	if ((s->bit_left & 0x1f) != 0)
	{
		s->bit_left = 32;
		//s->buf_ptr += 4;
	}
}
void ff_put_string(PutBitContext * pbc, char *s, int put_zero);

/* bit input */
/* buffer, buffer_end and size_in_bits must be present and used by every reader */
typedef struct GetBitContext {
    const uint8_t *buffer, *buffer_end;
#ifdef ALT_BITSTREAM_READER
    int index;
#elif defined LIBMPEG2_BITSTREAM_READER
    uint8_t *buffer_ptr;
    uint32_t cache;
    int bit_count;
#elif defined A32_BITSTREAM_READER
    uint32_t *buffer_ptr;
    uint32_t cache0;
    uint32_t cache1;
    int bit_count;
#endif
    int size_in_bits;
} GetBitContext;

#define VLC_TYPE int16_t

typedef struct VLC {
    int bits;
    VLC_TYPE (*table)[2]; /* code, bits */
    int table_size, table_allocated;
} VLC;

typedef struct RL_VLC_ELEM {
    int16_t level;
    int8_t len;
    uint8_t run;
} RL_VLC_ELEM;

#if defined(ARCH_SPARC) || defined(ARCH_ARMV4L) || defined(ARCH_MIPS) || defined(ARCH_BFIN)
#define UNALIGNED_STORES_ARE_BAD
#endif

/* used to avoid missaligned exceptions on some archs (alpha, ...) */
#if defined(ARCH_X86) || defined(CPU_COLDFIRE)
#    define unaligned16(a) (*(const uint16_t*)(a))
#    define unaligned32(a) (*(const uint32_t*)(a))
#    define unaligned64(a) (*(const uint64_t*)(a))
#else
#    ifdef __GNUC__
#    define unaligned(x)                                \
static inline uint##x##_t unaligned##x(const void *v) { \
    struct Unaligned {                                  \
        uint##x##_t i;                                  \
    } __attribute__((packed));                          \
                                                        \
    return ((const struct Unaligned *) v)->i;           \
}
#    elif defined(__DECC)
#    define unaligned(x)                                        \
static inline uint##x##_t unaligned##x(const void *v) {         \
    return *(const __unaligned uint##x##_t *) v;                \
}
#    else
#    define unaligned(x)                                        \
static inline uint##x##_t unaligned##x(const void *v) {         \
    return *(const uint##x##_t *) v;                            \
}
#    endif
unaligned(16)
unaligned(32)
unaligned(64)
#undef unaligned
#endif /* defined(ARCH_X86) */

#ifndef ALT_BITSTREAM_WRITER
static inline void put_bits(PutBitContext *s, int n, unsigned int value)
{
    unsigned int bit_buf;
    int bit_left;

    // printf("put_bits=%d %x\n", n, value);
    // assert(n == 32 || value < (1U << n));
    bit_buf = s->bit_buf;
    bit_left = s->bit_left;

    //    printf("n=%d value=%x cnt=%d buf=%x\n", n, value, bit_cnt, bit_buf);
    /* XXX: optimize */
    if (n < bit_left) {
        bit_buf = (bit_buf<<n) | value;
        bit_left-=n;
    } else {
        bit_buf<<=bit_left;
        bit_buf |= value >> (n - bit_left);
#ifdef UNALIGNED_STORES_ARE_BAD
        if (3 & (intptr_t) s->buf_ptr) {
            s->buf_ptr[0] = bit_buf >> 24;
            s->buf_ptr[1] = bit_buf >> 16;
            s->buf_ptr[2] = bit_buf >>  8;
            s->buf_ptr[3] = bit_buf      ;
        } else
#endif
        *(uint32_t *)s->buf_ptr = be2me_32(bit_buf);
        //printf("bitbuf = %08x\n", bit_buf);
        s->buf_ptr+=4;
        bit_left+=32 - n;
        bit_buf = value;
    }

    s->bit_buf = bit_buf;
    s->bit_left = bit_left;
}
#endif

static inline void put_sbits(PutBitContext *pb, int n, int32_t value)
{
	put_bits(pb, n, av_mod_uintp2(value, n));
}

static inline uint8_t* pbBufPtr(PutBitContext *s)
{
#ifdef ALT_BITSTREAM_WRITER
        return s->buf + (s->index>>3);
#else
        return s->buf_ptr;
#endif
}

/**
 *
 * PutBitContext must be flushed & aligned to a byte boundary before calling this.
 */
static inline void skip_put_bytes(PutBitContext *s, int n){
       // assert((put_bits_count(s)&7)==0);
#ifdef ALT_BITSTREAM_WRITER
        FIXME may need some cleaning of the buffer
        s->index += n<<3;
#else
       // assert(s->bit_left==32);
        s->buf_ptr += n;
#endif
}

/**
 * skips the given number of bits.
 * must only be used if the actual values in the bitstream do not matter
 */
static inline void skip_put_bits(PutBitContext *s, int n){
#ifdef ALT_BITSTREAM_WRITER
    s->index += n;
#else
    s->bit_left -= n;
    s->buf_ptr-= s->bit_left>>5;
    s->bit_left &= 31;
#endif
}

/**
 * Changes the end of the buffer.
 */
static inline void set_put_bits_buffer_size(PutBitContext *s, int size){
    s->buf_end= s->buf + size;
}

/* Bitstream reader API docs:
name
    abritary name which is used as prefix for the internal variables

gb
    getbitcontext

OPEN_READER(name, gb)
    loads gb into local variables

CLOSE_READER(name, gb)
    stores local vars in gb

UPDATE_CACHE(name, gb)
    refills the internal cache from the bitstream
    after this call at least MIN_CACHE_BITS will be available,

GET_CACHE(name, gb)
    will output the contents of the internal cache, next bit is MSB of 32 or 64 bit (FIXME 64bit)

SHOW_UBITS(name, gb, num)
    will return the next num bits

SHOW_SBITS(name, gb, num)
    will return the next num bits and do sign extension

SKIP_BITS(name, gb, num)
    will skip over the next num bits
    note, this is equivalent to SKIP_CACHE; SKIP_COUNTER

SKIP_CACHE(name, gb, num)
    will remove the next num bits from the cache (note SKIP_COUNTER MUST be called before UPDATE_CACHE / CLOSE_READER)

SKIP_COUNTER(name, gb, num)
    will increment the internal bit counter (see SKIP_CACHE & SKIP_BITS)

LAST_SKIP_CACHE(name, gb, num)
    will remove the next num bits from the cache if it is needed for UPDATE_CACHE otherwise it will do nothing

LAST_SKIP_BITS(name, gb, num)
    is equivalent to SKIP_LAST_CACHE; SKIP_COUNTER

for examples see get_bits, show_bits, skip_bits, get_vlc
*/

static inline int unaligned32_be(const void *v)
{
#ifdef CONFIG_ALIGN
        const uint8_t *p=v;
        return (((p[0]<<8) | p[1])<<16) | (p[2]<<8) | (p[3]);
#else
        return be2me_32( unaligned32(v)); //original
#endif
}

static inline int unaligned32_le(const void *v)
{
#ifdef CONFIG_ALIGN
       const uint8_t *p=v;
       return (((p[3]<<8) | p[2])<<16) | (p[1]<<8) | (p[0]);
#else
       return le2me_32( unaligned32(v)); //original
#endif
}

#ifdef ALT_BITSTREAM_READER
#   define MIN_CACHE_BITS 25

#   define OPEN_READER(name, gb)\
        int name##_index= (gb)->index;\
        int name##_cache= 0;\

#   define CLOSE_READER(name, gb)\
        (gb)->index= name##_index;\

# ifdef ALT_BITSTREAM_READER_LE
#   define UPDATE_CACHE(name, gb)\
        name##_cache= unaligned32_le( ((const uint8_t *)(gb)->buffer)+(name##_index>>3) ) >> (name##_index&0x07);\

#   define SKIP_CACHE(name, gb, num)\
        name##_cache >>= (num);
# else
#   define UPDATE_CACHE(name, gb)\
        name##_cache= unaligned32_be( ((const uint8_t *)(gb)->buffer)+(name##_index>>3) ) << (name##_index&0x07);\

#   define SKIP_CACHE(name, gb, num)\
        name##_cache <<= (num);
# endif

// FIXME name?
#   define SKIP_COUNTER(name, gb, num)\
        name##_index += (num);\

#   define SKIP_BITS(name, gb, num)\
        {\
            SKIP_CACHE(name, gb, num)\
            SKIP_COUNTER(name, gb, num)\
        }\

#   define LAST_SKIP_BITS(name, gb, num) SKIP_COUNTER(name, gb, num)
#   define LAST_SKIP_CACHE(name, gb, num) ;

# ifdef ALT_BITSTREAM_READER_LE
#   define SHOW_UBITS(name, gb, num)\
        ((name##_cache) & (NEG_USR32(0xffffffff,num)))

#   define SHOW_SBITS(name, gb, num)\
        NEG_SSR32((name##_cache)<<(32-(num)), num)
# else
#   define SHOW_UBITS(name, gb, num)\
        NEG_USR32(name##_cache, num)

#   define SHOW_SBITS(name, gb, num)\
        NEG_SSR32(name##_cache, num)
# endif

#   define GET_CACHE(name, gb)\
        ((uint32_t)name##_cache)

static inline int get_bits_count(GetBitContext *s){
    return s->index;
}

static inline void skip_bits_long(GetBitContext *s, int n){
    s->index += n;
}

#elif defined LIBMPEG2_BITSTREAM_READER
//libmpeg2 like reader

#   define MIN_CACHE_BITS 17

#   define OPEN_READER(name, gb)\
        int name##_bit_count=(gb)->bit_count;\
        int name##_cache= (gb)->cache;\
        uint8_t * name##_buffer_ptr=(gb)->buffer_ptr;\

#   define CLOSE_READER(name, gb)\
        (gb)->bit_count= name##_bit_count;\
        (gb)->cache= name##_cache;\
        (gb)->buffer_ptr= name##_buffer_ptr;\

#ifdef LIBMPEG2_BITSTREAM_READER_HACK

#   define UPDATE_CACHE(name, gb)\
    if(name##_bit_count >= 0){\
        name##_cache+= (int)be2me_16(*(uint16_t*)name##_buffer_ptr) << name##_bit_count;\
        name##_buffer_ptr += 2;\
        name##_bit_count-= 16;\
    }\

#else

#   define UPDATE_CACHE(name, gb)\
    if(name##_bit_count >= 0){\
        name##_cache+= ((name##_buffer_ptr[0]<<8) + name##_buffer_ptr[1]) << name##_bit_count;\
        name##_buffer_ptr+=2;\
        name##_bit_count-= 16;\
    }\

#endif

#   define SKIP_CACHE(name, gb, num)\
        name##_cache <<= (num);\

#   define SKIP_COUNTER(name, gb, num)\
        name##_bit_count += (num);\

#   define SKIP_BITS(name, gb, num)\
        {\
            SKIP_CACHE(name, gb, num)\
            SKIP_COUNTER(name, gb, num)\
        }\

#   define LAST_SKIP_BITS(name, gb, num) SKIP_BITS(name, gb, num)
#   define LAST_SKIP_CACHE(name, gb, num) SKIP_CACHE(name, gb, num)

#   define SHOW_UBITS(name, gb, num)\
        NEG_USR32(name##_cache, num)

#   define SHOW_SBITS(name, gb, num)\
        NEG_SSR32(name##_cache, num)

#   define GET_CACHE(name, gb)\
        ((uint32_t)name##_cache)

static inline int get_bits_count(GetBitContext *s){
    return (s->buffer_ptr - s->buffer)*8 - 16 + s->bit_count;
}

static inline void skip_bits_long(GetBitContext *s, int n){
    OPEN_READER(re, s)
    re_bit_count += n;
    re_buffer_ptr += 2*(re_bit_count>>4);
    re_bit_count &= 15;
    re_cache = ((re_buffer_ptr[-2]<<8) + re_buffer_ptr[-1]) << (16+re_bit_count);
    UPDATE_CACHE(re, s)
    CLOSE_READER(re, s)
}

#elif defined A32_BITSTREAM_READER

#   define MIN_CACHE_BITS 32

#   define OPEN_READER(name, gb)\
        int name##_bit_count=(gb)->bit_count;\
        uint32_t name##_cache0= (gb)->cache0;\
        uint32_t name##_cache1= (gb)->cache1;\
        uint32_t * name##_buffer_ptr=(gb)->buffer_ptr;\

#   define CLOSE_READER(name, gb)\
        (gb)->bit_count= name##_bit_count;\
        (gb)->cache0= name##_cache0;\
        (gb)->cache1= name##_cache1;\
        (gb)->buffer_ptr= name##_buffer_ptr;\

#   define UPDATE_CACHE(name, gb)\
    if(name##_bit_count > 0){\
        const uint32_t next= be2me_32( *name##_buffer_ptr );\
        name##_cache0 |= NEG_USR32(next,name##_bit_count);\
        name##_cache1 |= next<<name##_bit_count;\
        name##_buffer_ptr++;\
        name##_bit_count-= 32;\
    }\

#if defined(ARCH_X86)
#   define SKIP_CACHE(name, gb, num)\
        asm(\
            "shldl %2, %1, %0          \n\t"\
            "shll %2, %1               \n\t"\
            : "+r" (name##_cache0), "+r" (name##_cache1)\
            : "Ic" ((uint8_t)(num))\
           );
#else
#   define SKIP_CACHE(name, gb, num)\
        name##_cache0 <<= (num);\
        name##_cache0 |= NEG_USR32(name##_cache1,num);\
        name##_cache1 <<= (num);
#endif

#   define SKIP_COUNTER(name, gb, num)\
        name##_bit_count += (num);\

#   define SKIP_BITS(name, gb, num)\
        {\
            SKIP_CACHE(name, gb, num)\
            SKIP_COUNTER(name, gb, num)\
        }\

#   define LAST_SKIP_BITS(name, gb, num) SKIP_BITS(name, gb, num)
#   define LAST_SKIP_CACHE(name, gb, num) SKIP_CACHE(name, gb, num)

#   define SHOW_UBITS(name, gb, num)\
        NEG_USR32(name##_cache0, num)

#   define SHOW_SBITS(name, gb, num)\
        NEG_SSR32(name##_cache0, num)

#   define GET_CACHE(name, gb)\
        (name##_cache0)

static inline int get_bits_count(GetBitContext *s){
    return ((uint8_t*)s->buffer_ptr - s->buffer)*8 - 32 + s->bit_count;
}

static inline void skip_bits_long(GetBitContext *s, int n){
    OPEN_READER(re, s)
    re_bit_count += n;
    re_buffer_ptr += re_bit_count>>5;
    re_bit_count &= 31;
    re_cache0 = be2me_32( re_buffer_ptr[-1] ) << re_bit_count;
    re_cache1 = 0;
    UPDATE_CACHE(re, s)
    CLOSE_READER(re, s)
}

#endif

/**
 * read mpeg1 dc style vlc (sign bit + mantisse with no MSB).
 * if MSB not set it is negative
 * @param n length in bits
 * @author BERO
 */
static inline int get_xbits(GetBitContext *s, int n){
    register int sign;
    register int32_t cache;
    OPEN_READER(re, s)
    UPDATE_CACHE(re, s)
    cache = GET_CACHE(re,s);
    sign=(~cache)>>31;
    LAST_SKIP_BITS(re, s, n)
    CLOSE_READER(re, s)
    return (NEG_USR32(sign ^ cache, n) ^ sign) - sign;
}

static inline int get_sbits(GetBitContext *s, int n){
    register int tmp;
    OPEN_READER(re, s)
    UPDATE_CACHE(re, s)
    tmp= SHOW_SBITS(re, s, n);
    LAST_SKIP_BITS(re, s, n)
    CLOSE_READER(re, s)
    return tmp;
}

/**
 * reads 1-17 bits.
 * Note, the alt bitstream reader can read up to 25 bits, but the libmpeg2 reader can't
 */
static inline unsigned int get_bits(GetBitContext *s, int n){
    register int tmp;
    OPEN_READER(re, s)
    UPDATE_CACHE(re, s)
    tmp= SHOW_UBITS(re, s, n);
    LAST_SKIP_BITS(re, s, n)
    CLOSE_READER(re, s)
    return tmp;
}

/**
 * Read 0-25 bits.
 */
static av_always_inline int get_bitsz(GetBitContext *s, int n)
{
    return n ? get_bits(s, n) : 0;
}

/**
 * shows 1-17 bits.
 * Note, the alt bitstream reader can read up to 25 bits, but the libmpeg2 reader can't
 */
static inline unsigned int show_bits(GetBitContext *s, int n){
    register int tmp;
    OPEN_READER(re, s)
    UPDATE_CACHE(re, s)
    tmp= SHOW_UBITS(re, s, n);
//    CLOSE_READER(re, s)
    return tmp;
}

static inline void skip_bits(GetBitContext *s, int n){
/* Note: gcc seems to optimize this to s->index+=n for the ALT_READER :)) */
    OPEN_READER(re, s)
    UPDATE_CACHE(re, s)
    LAST_SKIP_BITS(re, s, n)
    CLOSE_READER(re, s)
}

static inline unsigned int get_bits1(GetBitContext *s){
#ifdef ALT_BITSTREAM_READER
    int index= s->index;
    uint8_t result= s->buffer[ index>>3 ];
#ifdef ALT_BITSTREAM_READER_LE
    result>>= (index&0x07);
    result&= 1;
#else
    result<<= (index&0x07);
    result>>= 8 - 1;
#endif
    index++;
    s->index= index;

    return result;
#else
    return get_bits(s, 1);
#endif
}

static inline unsigned int show_bits1(GetBitContext *s){
    return show_bits(s, 1);
}

static inline void skip_bits1(GetBitContext *s){
    skip_bits(s, 1);
}

/**
 * reads 0-32 bits.
 */
static inline unsigned int get_bits_long(GetBitContext *s, int n){
    if(n<=17) return get_bits(s, n);
    else{
#ifdef ALT_BITSTREAM_READER_LE
        int ret= get_bits(s, 16);
        return ret | (get_bits(s, n-16) << 16);
#else
        int ret= get_bits(s, 16) << (n-16);
        return ret | get_bits(s, n-16);
#endif
    }
}

/**
* Read 0-64 bits.
*/
static inline uint64_t get_bits64(GetBitContext *s, int n)
{
	if (n <= 32) {
		return get_bits_long(s, n);
	}
	else {
#ifdef BITSTREAM_READER_LE
		uint64_t ret = get_bits_long(s, 32);
		return ret | (uint64_t)get_bits_long(s, n - 32) << 32;
#else
		uint64_t ret = (uint64_t)get_bits_long(s, n - 32) << 32;
		return ret | get_bits_long(s, 32);
#endif
	}
}

/**
 * shows 0-32 bits.
 */
static inline unsigned int show_bits_long(GetBitContext *s, int n){
    if(n<=17) return show_bits(s, n);
    else{
        GetBitContext gb= *s;
        int ret= get_bits_long(s, n);
        *s= gb;
        return ret;
    }
}

static inline int get_bits_left(GetBitContext *gb)
{
	return gb->size_in_bits - get_bits_count(gb);
}

/*
static inline int check_marker(GetBitContext *s, const char *msg)
{
    int bit= get_bits1(s);
    if(!bit)
        av_log(NULL, AV_LOG_INFO, "Marker bit missing %s\n", msg);

    return bit;
}
*/

/**
 * init GetBitContext.
 * @param buffer bitstream buffer, must be FF_INPUT_BUFFER_PADDING_SIZE bytes larger then the actual read bits
 * because some optimized bitstream readers read 32 or 64 bit at once and could read over the end
 * @param bit_size the size of the buffer in bits
 */
static inline int init_get_bits(GetBitContext *s,
                   const uint8_t *buffer, int bit_size)
{
    int buffer_size= (bit_size+7)>>3;
    if(buffer_size < 0 || bit_size < 0) {
        buffer_size = bit_size = 0;
        buffer = NULL;
    }

    s->buffer= buffer;
    s->size_in_bits= bit_size;
    s->buffer_end= buffer + buffer_size;
#ifdef ALT_BITSTREAM_READER
    s->index=0;
#elif defined LIBMPEG2_BITSTREAM_READER
    s->buffer_ptr = (uint8_t*)((intptr_t)buffer&(~1));
    s->bit_count = 16 + 8*((intptr_t)buffer&1);
    skip_bits_long(s, 0);
#elif defined A32_BITSTREAM_READER
    s->buffer_ptr = (uint32_t*)((intptr_t)buffer&(~3));
    s->bit_count = 32 + 8*((intptr_t)buffer&3);
    skip_bits_long(s, 0);
#endif
	return 0;
}

static inline void align_get_bits(GetBitContext *s)
{
    int n= (-get_bits_count(s)) & 7;
    if(n) skip_bits(s, n);
}

static inline int init_get_bits8(GetBitContext *s, const uint8_t *buffer,
	int byte_size)
{
	if (byte_size > INT_MAX / 8 || byte_size < 0)
		byte_size = -1;
	return init_get_bits(s, buffer, byte_size * 8);
}

int init_vlc(VLC *vlc, int nb_bits, int nb_codes,
             const void *bits, int bits_wrap, int bits_size,
             const void *codes, int codes_wrap, int codes_size,
             int flags);
#define INIT_VLC_USE_STATIC 1
#define INIT_VLC_LE         2
void free_vlc(VLC *vlc);

/**
 *
 * if the vlc code is invalid and max_depth=1 than no bits will be removed
 * if the vlc code is invalid and max_depth>1 than the number of bits removed
 * is undefined
 */
#define GET_VLC(code, name, gb, table, bits, max_depth)\
{\
    int n, index, nb_bits;\
\
    index= SHOW_UBITS(name, gb, bits);\
    code = table[index][0];\
    n    = table[index][1];\
\
    if(max_depth > 1 && n < 0){\
        LAST_SKIP_BITS(name, gb, bits)\
        UPDATE_CACHE(name, gb)\
\
        nb_bits = -n;\
\
        index= SHOW_UBITS(name, gb, nb_bits) + code;\
        code = table[index][0];\
        n    = table[index][1];\
        if(max_depth > 2 && n < 0){\
            LAST_SKIP_BITS(name, gb, nb_bits)\
            UPDATE_CACHE(name, gb)\
\
            nb_bits = -n;\
\
            index= SHOW_UBITS(name, gb, nb_bits) + code;\
            code = table[index][0];\
            n    = table[index][1];\
        }\
    }\
    SKIP_BITS(name, gb, n)\
}

#define GET_RL_VLC(level, run, name, gb, table, bits, max_depth, need_update)\
{\
    int n, index, nb_bits;\
\
    index= SHOW_UBITS(name, gb, bits);\
    level = table[index].level;\
    n     = table[index].len;\
\
    if(max_depth > 1 && n < 0){\
        SKIP_BITS(name, gb, bits)\
        if(need_update){\
            UPDATE_CACHE(name, gb)\
        }\
\
        nb_bits = -n;\
\
        index= SHOW_UBITS(name, gb, nb_bits) + level;\
        level = table[index].level;\
        n     = table[index].len;\
    }\
    run= table[index].run;\
    SKIP_BITS(name, gb, n)\
}


/**
 * parses a vlc code, faster then get_vlc()
 * @param bits is the number of bits which will be read at once, must be
 *             identical to nb_bits in init_vlc()
 * @param max_depth is the number of times bits bits must be read to completely
 *                  read the longest vlc code
 *                  = (max_vlc_length + bits - 1) / bits
 */
static av_always_inline int get_vlc2(GetBitContext *s, VLC_TYPE (*table)[2],
                                  int bits, int max_depth)
{
    int code;

    OPEN_READER(re, s)
    UPDATE_CACHE(re, s)

    GET_VLC(code, re, s, table, bits, max_depth)

    CLOSE_READER(re, s)
    return code;
}

static inline int decode012(GetBitContext *gb){
    int n;
    n = get_bits1(gb);
    if (n == 0)
        return 0;
    else
        return get_bits1(gb) + 1;
}

#include <errno.h>
#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#define ERROR_INVALIDDATA -2
#define ERROR_PATCHWELCOME -3
static av_always_inline int av_clip_c(int a, int amin, int amax)
{
	if (a < amin) return amin;
	else if (a > amax) return amax;
	else               return a;
}
#define av_clip av_clip_c



typedef struct bits_buffer_s
{
	int     i_size;

	int     i_data;
	uint8_t i_mask;
	uint8_t *p_data;

} bits_buffer_t;

static inline int bits_initwrite(bits_buffer_t *p_buffer, void *p_data,
	int i_size)
{
	p_buffer->i_size = i_size;
	p_buffer->i_data = 0;
	p_buffer->i_mask = 0x80;
	p_buffer->p_data = (uint8_t*)p_data;
	if (!p_buffer->p_data)
	{
		if (!(p_buffer->p_data = (uint8_t*)malloc(i_size)))
			return -1;
	}
	p_buffer->p_data[0] = 0;
	return 0;
}

static inline void bits_align(bits_buffer_t *p_buffer)
{
	if (p_buffer->i_mask != 0x80 && p_buffer->i_data < p_buffer->i_size)
	{
		p_buffer->i_mask = 0x80;
		p_buffer->i_data++;
		p_buffer->p_data[p_buffer->i_data] = 0x00;
	}
}

static inline void bits_write(bits_buffer_t *p_buffer,
	int i_count, uint64_t i_bits)
{
	while (i_count > 0)
	{
		i_count--;

		if ((i_bits >> i_count) & 0x01)
		{
			p_buffer->p_data[p_buffer->i_data] |= p_buffer->i_mask;
		}
		else
		{
			p_buffer->p_data[p_buffer->i_data] &= ~p_buffer->i_mask;
		}
		p_buffer->i_mask >>= 1;
		if (p_buffer->i_mask == 0)
		{
			p_buffer->i_data++;
			p_buffer->i_mask = 0x80;
		}
	}
}

#ifdef __cplusplus
}
#endif

#endif /* BITSTREAM_H */
