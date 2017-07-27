#ifndef __OOK_HASHKEY_H__
#define __OOK_HASHKEY_H__

#include <string.h>
#include "base64.h"
extern "C" {
	#include "tools/sha1/sha1.h"
};

inline int caculate_hashkey(const char * instr, unsigned int inlen, char * hashkey, int maxhashlen)
{
	SHA1Context sha;
	int err = SHA1Reset(&sha);
	if(err == 0)
	{
		err = SHA1Input(&sha, (const unsigned char *)instr, inlen);
		if(err == 0)
		{
			unsigned char encstr[32];
			memset(encstr, 0, 32);
			err = SHA1Result(&sha, encstr);
			if(err == 0)
			{
				int baselen = base64_encode(encstr, (unsigned char *)hashkey, 20);
				if(baselen > 0 && maxhashlen > 0)
				{
					baselen = baselen > maxhashlen ? maxhashlen : baselen;
					hashkey[baselen] = 0;
				}
				return baselen;
			}
		}
	}
	return -1;
}

inline void check_hashkey(char * hashkey, int size)
{
	int i;
	for(i = 0; i < size; i++)
	{
		if(hashkey[i] == '/')
			hashkey[i] = 'A';
		else if(hashkey[i] == '+')
			hashkey[i] = 'C';
	}
}

#endif
