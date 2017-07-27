#ifndef __OOK_ALGORITHM_H__
#define __OOK_ALGORITHM_H__

inline unsigned int __gcd__(unsigned int m, unsigned int n)
{
	unsigned int r = m % n;
	while(r != 0)
	{
		m = n;
		n = r;
		r = m % n;
	}
	return n;
}

#endif
