#ifndef __SYNTAX_H__
#define __SYNTAX_H__

#include <stdint.h>

//System SYNTAX Definiation

typedef uint8_t	 uchar;
typedef uint16_t uint16;
typedef int32_t  int32;
typedef uint32_t uint32;
typedef uint64_t uint64;
#ifndef __em8622l__
typedef uint16_t ushort;
#endif

#ifndef INT64_MIN
#define INT64_MIN (-9223372036854775807LL-1)
#endif
#ifndef INT64_MAX
#define INT64_MAX (9223372036854775807LL)
#endif

#ifdef WIN32
#define UINT2VOID (void *)(uint64_t)(unsigned int)
#define VOID2UINT (unsigned int)(uint64_t)
#define VOID2INT  (int)(unsigned int)(int64_t)
#else
#define UINT2VOID (void *)(unsigned long)(unsigned int)
#define VOID2UINT (unsigned int)(unsigned long)
#define VOID2INT  (int)(unsigned int)(unsigned long)
#endif

#endif
