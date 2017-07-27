#ifndef __OOK_BITSTREAM_H__
#define __OOK_BITSTREAM_H__

#include <stdint.h>

#define WORD_SIZE_ sizeof(void *)

#if WORDS_BIGENDIAN
	#define endian_fix_(x) (x)
	#define endian_fix64_(x) (x)
	#define endian_fix32_(x) (x)
	#define endian_fix16_(x) (x)
#else
	static inline uint32_t endian_fix32_(uint32_t x)
	{
	    return (x << 24) + ((x << 8) & 0xff0000) + ((x >> 8) & 0xff00) + (x >> 24);
	}
	static inline uint64_t endian_fix64_(uint64_t x)
	{
	    return endian_fix32_((uint32_t)(x >> 32)) + ((uint64_t)endian_fix32_((uint32_t)x) << 32);
	}
	static inline intptr_t endian_fix_(intptr_t x)
	{
	    return (intptr_t)(WORD_SIZE_ == 8 ? endian_fix64_(x) : endian_fix32_(x));
	}
	static inline uint16_t endian_fix16_(uint16_t x)
	{
	    return (x << 8) | (x >> 8);
	}
#endif

typedef union 
{ 
	uint32_t i; 
	uint16_t b[2]; 
	uint8_t  c[4]; 
} ook_union32_t;

#define M32_(src) (((ook_union32_t *)(src))->i)

typedef struct
{
    uint8_t * p_start;
    uint8_t * p;
    uint8_t * p_end;

    uintptr_t cur_bits;
    int i_left;
} bs_s;

static inline void bs_init(bs_s * s, void * bits, int size)
{
	int offset    = ((intptr_t)bits & 3);
    s->p          = s->p_start = (uint8_t *)bits - offset;
    s->p_end      = (uint8_t *)bits + size;
    s->i_left     = (WORD_SIZE_ - offset) * 8;
    s->cur_bits   = endian_fix32_(M32_(s->p));
    s->cur_bits >>= (4 - offset) * 8;
    ///printf("[%p/%p/%d]::cur_bits=0x%x, i_left=%d\n", bits, s->p, offset, s->cur_bits, s->i_left);
}

static inline void bs_flush(bs_s * s)
{
    M32_(s->p) = endian_fix32_(s->cur_bits << (s->i_left & 31));
    s->p      += WORD_SIZE_ - (s->i_left >> 3);
    s->i_left  = WORD_SIZE_ * 8;
}

static inline void bs_write(bs_s * s, int i_count, uint32_t i_bits)
{
	if(WORD_SIZE_ == 8)
	{
        s->cur_bits = (s->cur_bits << i_count) | i_bits;
        s->i_left  -= i_count;
        if(s->i_left <= 32)
        {
		#if WORDS_BIGENDIAN
            M32_(s->p) = s->cur_bits >> (32 - s->i_left);
		#else
            M32_(s->p) = endian_fix_(s->cur_bits << s->i_left);
		#endif
            s->i_left += 32;
            s->p      += 4;
        }
	}
	else
	{
        if(i_count < s->i_left)
        {
            s->cur_bits = (s->cur_bits << i_count) | i_bits;
            s->i_left  -= i_count;
            ///printf("[%p]::cur_bits=0x%x, i_left=%d\n", s->p, s->cur_bits, s->i_left);
        }
		else
		{
            i_count    -= s->i_left;
            s->cur_bits = (s->cur_bits << s->i_left) | (i_bits >> i_count);
            M32_(s->p)  = endian_fix_( s->cur_bits );
            s->p       += 4;
            s->cur_bits = i_bits;
            s->i_left   = 32 - i_count;			
		}
	}
}

#endif
