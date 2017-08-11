#include "hffmpeg.h"

void yuv2rgb32(const uchar* yuv, int w, int h, uchar* rgb){
    SwsContext* ctx = sws_getContext(w, h, AV_PIX_FMT_YUV420P, w, h, AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);

    AVFrame src,dst;
    int y_size = w*h;
    src.linesize[0] = w;
    src.linesize[1] = w>>1;
    src.linesize[2] = src.linesize[1];
    src.data[0] = (uint8_t*)yuv;
    src.data[1] = (uint8_t*)yuv + y_size;
    src.data[2] = (uint8_t*)src.data[1] + y_size/4;

    dst.data[0] = (uint8_t*)rgb;
    dst.linesize[0] = w*4;

    sws_scale(ctx, src.data, src.linesize, 0, h, dst.data, dst.linesize);

    sws_freeContext(ctx);
}
