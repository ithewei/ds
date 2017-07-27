#ifndef __PRINT_BUFFER__
#define __PRINT_BUFFER__

#include <stdio.h>

#ifdef WIN32
#ifndef inline
#define inline __inline
#endif
#endif

inline void pbuf_s(const unsigned char * buf, int size)
{
	int len;
	int i = 0;
	int j = 0;
	int k = 0;

	for(len = 0; len < size; len ++)
	{
		if(j == 0) 
		{
			if(i > 0)
				printf("\n");
			i++;
			printf("   ");
			k++;
		}

		printf("%4x", *(buf + len));
		j ++;
		if(j == 16)
			j = 0;
	}
	printf("\n");;
}

inline void pbuf(const unsigned char * buf, int size)
{
	int i, len;
	int j = 0;
	int k = 0;

	printf("Buffer Map: \n");
	printf("%3c", ' ');
	for(i = 0; i < 16; i++)
		printf("%4d", i);

	for(len = 0; len < size; len ++)
	{
		if(j == 0) 
		{
			printf("\n");
			printf("%2x%c", k,':');
			k++;
		}

		printf("%4x", *(buf + len));
		j++;
		if(j == 16)
			j = 0;
	}

	printf("\n");;
}

inline void pbuf2(const unsigned char * buf, int size, int line)
{
	int i, len;
	int j = 0;
	int k = 0;

	printf("Buffer Map: \n");
	printf("%3c", ' ');
	for(i = 0; i < line; i++)
		printf("%4d", i);

	for(len = 0; len < size; len ++)
	{
		if(j == 0) 
		{
			printf("\n");
			printf("%2x%c", k,':');
			k++;
		}

		printf("%4x", *(buf + len));
		j++;
		if(j == line)
			j = 0;
	}

	printf("\n");;
}

inline void pshort(const short * buf, int size)
{
	int i, len;
	int j = 0;
	int k = 0;
	const short * p = buf;

	printf("Buffer Map: \n");
	printf("%3c", ' ');
	for(i = 0; i < 16; i++)
		printf("%4d", i);

	for(len = 0; len < size; len++)
	{
		if(j == 0)
		{
			printf("\n");
			printf("%2x%c", k,':');
			k++;
		}

		printf("%4x", *p++);
		///printf("%6d", *p++);
		j++;
		if(j == 16)
			j = 0;
	}

	printf("\n");;
}

inline void pint(const unsigned int * buf, int size)
{
	int i, len;
	int j = 0;
	int k = 0;
	const unsigned int * p = buf;

	printf("Buffer Map: \n");
	printf("%4c", ' ');
	for(i = 0; i < 8; i++)
		printf("%9d", i);

	for(len = 0; len < size; len++)
	{
		if(j == 0)
		{
			printf("\n");
			printf("%3x%c", k,':');
			k++;
		}

		printf("%9x", *p++);
		j++;
		if(j == 8)
			j = 0;
	}

	printf("\n");;
}

inline void plong(const unsigned long * buf, int size)
{
	int i, len;
	int j = 0;
	int k = 0;
	const unsigned long * p = buf;

	printf("Long Map:\n");
	printf("%4c", ' ');
	for(i = 0; i < 8; i++)
		printf("%9d", i);

	for(len = 0; len < size; len++)
	{
		if(j == 0)
			printf("\n%3x%c", k++, ':');

		printf("%9x", (unsigned int)*p++);
		j++;
		if(j == 8)
			j = 0;
	}

	printf("\n");;
}

inline void pfloat(const float * buf, int size)
{
	int i, j, k, len;

	printf("Float Map:\n");
	printf("%4c", ' ');
	for(i = 0; i < 8; i++)
		printf("%9d", i);

	j = 0;
	k = 0;
	for(len = 0; len < size; len++)
	{
		if(j == 0) 
			printf("\n%3x%c ", k++,':');

		printf("%08.3f ", buf[len]);
		j++;
		if(j == 8)
			j = 0;
	}

	printf("\n");
}

#endif
