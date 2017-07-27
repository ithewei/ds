#ifndef __OOK_SERIAL_H__
#define __OOK_SERIAL_H__

#include <stdint.h>

inline void pack_uint16(unsigned char * buf, uint16_t s)
{
	*buf = (s & 0xff00) >> 8;
	*(buf + 1) = s & 0xff;
}

inline void pack_uint16_l(unsigned char * buf, uint16_t s)
{
	*buf = s & 0xff;
	*(buf + 1) = (s & 0xff00) >> 8;
}

inline void pack_uint32(unsigned char * buf, uint32_t i)
{
	*buf = (i & 0xff000000) >> 24;
	*(buf + 1) = (i & 0xff0000) >> 16;
	*(buf + 2) = (i & 0xff00) >> 8;	
	*(buf + 3) = i & 0xff;
}

inline void pack_int64(unsigned char * buf, int64_t i)
{
	uint32_t ih = (uint32_t)(i >> 32);
	pack_uint32(buf, ih);
	pack_uint32(buf + 4, (uint32_t)i);
}

inline void pack_uint64(unsigned char * buf, uint64_t i)
{
	uint32_t ih = (uint32_t)(i >> 32);
	pack_uint32(buf, ih);
	pack_uint32(buf + 4, (uint32_t)i);
}

inline void pack_uint32_l(unsigned char * buf, uint32_t i)
{
	*(buf + 3) = (i & 0xff000000) >> 24;
	*(buf + 2) = (i & 0xff0000) >> 16;
	*(buf + 1) = (i & 0xff00) >> 8;	
	*(buf + 0) = i & 0xff;
}

inline void pack_int64_l(unsigned char * buf, int64_t i)
{
	uint32_t ih = (uint32_t)(i >> 32);
	pack_uint32_l(buf, (uint32_t)i);
	pack_uint32_l(buf + 4, ih);
}

inline void pack_uint64_l(unsigned char * buf, uint64_t i)
{
	uint32_t ih = (uint32_t)(i >> 32);
	pack_uint32_l(buf, (uint32_t)i);
	pack_uint32_l(buf + 4, ih);
}

inline uint16_t unpack_uint16(const unsigned char * buf)
{
    uint16_t ret = (buf[0] << 8) | buf[1];
    return ret;	
}

inline uint16_t unpack_uint16_l(const unsigned char * buf)
{
    uint16_t ret = (buf[1] << 8) | buf[0];
    return ret;	
}

inline uint32_t unpack_uint24(const unsigned char * buf)
{
    uint32_t r = (buf[0] << 16) | (buf[1] << 8) | buf[2];
    return r;
}

inline uint32_t unpack_uint32(const unsigned char * buf)
{
    uint32_t ret = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
    return ret;
}

inline uint32_t unpack_uint32_l(const unsigned char * buf)
{
    uint32_t ret = (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | buf[0];
    return ret;
}

inline uint64_t unpack_uint64(const unsigned char * buf)
{
	uint32_t iL = unpack_uint32(buf + 4);
	uint32_t iH = unpack_uint32(buf);
	return ((uint64_t)iH << 32) + (uint64_t)iL;
}

inline int64_t unpack_int64(const unsigned char * buf)
{
	uint32_t iL = unpack_uint32(buf + 4);
	uint32_t iH = unpack_uint32(buf);
	return ((int64_t)iH << 32) + (int64_t)iL;
}

#endif
