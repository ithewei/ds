#ifndef HFFMPEG_H
#define HFFMPEG_H

#include "qglobal.h"

extern "C"{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libswscale/swscale.h"
}

// @note rgb memroy alloc by caller
void yuv2rgb32(const uchar* yuv, int w, int h, uchar* rgb);

#endif // HFFMPEG_H
