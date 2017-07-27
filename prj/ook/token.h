#ifndef __OOK_TOKEN_H__
#define __OOK_TOKEN_H__

#include <ook/hash.h>
#include <ook/hashkey.h>

inline unsigned int caculate_token_byname(const char * name)
{
	if(!name)
		return 0;
	unsigned int token = 0;
	unsigned int s = (unsigned int)strlen(name);
	SHA1Context sha;
	int err = SHA1Reset(&sha);
	if(err == 0)
	{
		err = SHA1Input(&sha, (const unsigned char *)name, s);
		if(err == 0)
		{
			unsigned char encstr[21];
			///unsigned char * p = encstr;
			memset(encstr, 0, 21);
			err = SHA1Result(&sha, encstr);
			if(err == 0)
			{
			#if 1
				///printf("caculate_token_byname::hash\n");
				///pbuf(encstr, 20);
				unsigned int initval = BKDRHash((char *)encstr, 20);
				///printf("caculate_token_byname::BKDRHash=%u\n", initval);
				token = jen_hash(encstr, 20, initval);
				///printf("caculate_token_byname::jen_hash=%u\n", token);
			#else
				for(int i = 0; i < 20; i += 4)
				{
					t = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
					p += 4;
					if(token == 0)
						token = t;
					else
						token = (token >> 1) + (t >> 1);
				}
			#endif
			}
		}
	}
	return token;
}

inline unsigned int caculate_token_bystr(const char * str)
{
	char hash[64];
	int r = caculate_hashkey(str, (int)strlen(str), hash, 64);
	unsigned int initval = BKDRHash(hash, r);
	return jen_hash((unsigned char *)hash, r, initval);
}

#endif
