#ifndef __OOK_HASH_H__
#define __OOK_HASH_H__

static unsigned int BKDRHash(char * str, unsigned int len)
{
    unsigned int seed = 131;
    /* 31 131 1313 13131 131313 etc.. */
    unsigned int hash = 0;
    unsigned int i    = 0;
    for(i = 0; i < len; str++, i++)
    {
        hash = (hash * seed) + (*str);
    }
    return hash;
}

#define hashsize(n) (1U << (n))
#define hashmask(n) (hashsize(n) - 1)

#define jen_hash_mix(a, b, c)\
{\
	a -= b; a -= c; a ^= ( c >> 13 );\
	b -= c; b -= a; b ^= ( a <<  8 );\
	c -= a; c -= b; c ^= ( b >> 13 );\
	a -= b; a -= c; a ^= ( c >> 12 );\
	b -= c; b -= a; b ^= ( a << 16 );\
	c -= a; c -= b; c ^= ( b >>  5 );\
	a -= b; a -= c; a ^= ( c >>  3 );\
	b -= c; b -= a; b ^= ( a << 10 );\
	c -= a; c -= b; c ^= ( b >> 15 );\
}

static unsigned jen_hash(unsigned char * k, unsigned length, unsigned initval)
{
	unsigned a, b;
	unsigned c   = initval;
	unsigned len = length;

	a = b = 0x9e3779b9;

	while ( len >= 12 ) {
		a += ( k[0] + ( (unsigned)k[1] << 8 )
		+ ( (unsigned)k[2] << 16 )
		+ ( (unsigned)k[3] << 24 ) );
		b += ( k[4] + ( (unsigned)k[5] << 8 )
		+ ( (unsigned)k[6] << 16 )
		+ ( (unsigned)k[7] << 24 ) );
		c += ( k[8] + ( (unsigned)k[9] << 8 )
		+ ( (unsigned)k[10] << 16 )
		+ ( (unsigned)k[11] << 24 ) );

		jen_hash_mix(a, b, c);

		k   += 12;
		len -= 12;
	}

	c += length;

	switch ( len ) {
		case 11: c += ( (unsigned)k[10] << 24 );
		case 10: c += ( (unsigned)k[9] << 16 );
		case 9 : c += ( (unsigned)k[8] << 8 );
		/* First byte of c reserved for length */
		case 8 : b += ( (unsigned)k[7] << 24 );
		case 7 : b += ( (unsigned)k[6] << 16 );
		case 6 : b += ( (unsigned)k[5] << 8 );
		case 5 : b += k[4];
		case 4 : a += ( (unsigned)k[3] << 24 );
		case 3 : a += ( (unsigned)k[2] << 16 );
		case 2 : a += ( (unsigned)k[1] << 8 );
		case 1 : a += k[0];
	}

	jen_hash_mix(a, b, c);
	
	return c;
}

#endif
