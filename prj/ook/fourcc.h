#ifndef __OOK_FOURCC_H__
#define __OOK_FOURCC_H__

#ifdef WORDS_BIGENDIAN
 
#define OOK_FOURCC( a, b, c, d ) \
        ( ((unsigned int)d) | ( ((unsigned int)c) << 8 ) \
           | ( ((unsigned int)b) << 16 ) | ( ((unsigned int)a) << 24 ) )

#define OOK_TWOCC( a, b ) \
        ( (unsigned short)(b) | ( (nsigned short)(a) << 8 ) )

#else

#define OOK_FOURCC( a, b, c, d ) \
        ( ((unsigned int)a) | ( ((unsigned int)b) << 8 ) \
           | ( ((unsigned int)c) << 16 ) | ( ((unsigned int)d) << 24 ) )

#define OOK_TWOCC( a, b ) \
        ( (nsigned short)(a) | ( (nsigned short)(b) << 8 ) )

#endif

#endif
