#ifndef __BASE64_H__
#define __BASE64_H__

#include <stdlib.h>
#include <stdio.h>

/*
** Translation Table as described in RFC1113
*/
static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
** Translation Table to decode (created by author)
*/
static const char cd64[]="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

/*
** encodeblock
**
** encode 3 8-bit binary bytes as 4 '6-bit' characters
*/
inline void base64_encodeblock(const unsigned char in[3], unsigned char out[4], int len)
{
    out[0] = cb64[in[0] >> 2 ];
    out[1] = cb64[((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4)];
    out[2] = (unsigned char)(len > 1 ? cb64[((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
    out[3] = (unsigned char)(len > 2 ? cb64[in[2] & 0x3f ] : '=');
}

/*
** decodeblock
**
** decode 4 '6-bit' characters into 3 8-bit binary bytes
*/
inline void base64_decodeblock(unsigned char in[4], unsigned char out[3])
{   
    out[0] = (unsigned char )(in[0] << 2 | in[1] >> 4);
    out[1] = (unsigned char )(in[1] << 4 | in[2] >> 2);
    out[2] = (unsigned char )(((in[2] << 6) & 0xc0) | in[3]);
}

inline int base64_encode(const unsigned char * in, unsigned char * out, int in_len)
{
	int count = in_len / 3;
	if(count * 3 != in_len)
		count++;
	int out_len = 0;
	for(int i = 0; i < count ; i++)
	{
		base64_encodeblock(in + i * 3, out + i * 4, in_len);
		in_len  -= 3;
		out_len += 4;
	}
	return out_len;
}

inline int base64_decode(unsigned char * in, unsigned char * out, int in_len)
{
	unsigned char v;
	int i;
	int count   = 0;
	int out_len = 0;
 	int dec_len = 0;

	// trim the last "="
	for(i = in_len - 1; i >= 0; i--)
	{
		if(*(in + i) == '=')
			dec_len++;
		else
			break;
	}

    for(i = 0; i < in_len / 4; i++) 
    {
    	//printf("i=%d\n", i);
    	int len, j;
    	for(len = 0, j = 0; j < 4; j++)
    	{
    		//printf("   j=%d\n", j);
    		v = 0;
    		while((count <= in_len) && (v == 0))
    		{
            	v = in[i * 4 + j];
            	//printf("      idx=%d      v=[%c]", i*4 + j, v);
            	v = (unsigned char)((v < 43 || v > 122) ? 0 : cd64[v - 43]);
            	//printf("      v=[%c]", v);
            	if(v)
	            	v = (unsigned char)((v == '$') ? 0 : v - 61);
	            //printf("      v=[%c]\n", v);
	            count++;	            
	            //printf("      count=%d\n", count);          
	        }
	        if(count <= in_len)
	        {
	        	len++;
		        if(v)
    	        	in[i * 4 + j] = (unsigned char)(v - 1);
    	    }
    	    else
    	    {
    	    	in[i * 4 + j] = 0;
    	    }
        }
        if(len > 0)
        {
        	base64_decodeblock(in + i * 4, out + i * 3);
			out_len += 3;
        	//printf("   %c%c%c%c : %c%c%c\n", *(in + i * 4), *(in + i * 4 + 1), *(in + i * 4 + 2), *(in + i * 4 + 3), *(out + i*3), *(out + i*3 + 1), *(out + i*3 + 2));
        }
     }
	 return out_len - dec_len;
}

#endif
