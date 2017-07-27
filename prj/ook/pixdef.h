#ifndef __OOK_PIXDEF_H__

#include "fourcc.h"

// less than 8 bits
#define FCC_FIX_RGB565		OOK_FOURCC('R', 'V', '5', '6')


// 8 bits
#define FCC_FIX_I420 		OOK_FOURCC('I', '4', '2', '0')
#define FCC_FIX_YV12		OOK_FOURCC('Y', 'V', '1', '2')
#define FCC_FIX_YUY2		OOK_FOURCC('Y', 'U', 'Y', '2')
#define FCC_FIX_YUYV		OOK_FOURCC('Y', 'U', 'Y', 'V')
#define FCC_FIX_UYVY		OOK_FOURCC('U', 'Y', 'V', 'Y')
#define FCC_FIX_HDYC		OOK_FOURCC('H', 'D', 'Y', 'C')

// RGB
#define FCC_FIX_RGB24		OOK_FOURCC('R', 'V', '2', '4')
#define FCC_FIX_RGBA		OOK_FOURCC('R', 'V', '3', '2')
// BGR
#define FCC_FIX_BGR24		OOK_FOURCC('B', 'G', '2', '4')
#define FCC_FIX_BGRA		OOK_FOURCC('B', 'G', '3', '2')


// 10 bits
#define FCC_FIX_10_YUV420	OOK_FOURCC('Y', '0', '1', '0')
#define FCC_FIX_10_YUV422	OOK_FOURCC('Y', '2', '1', '0')

#endif
