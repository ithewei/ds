#ifndef __OOK_VIDEO_SCALE_H__
#define __OOK_VIDEO_SCALE_H__

#include "avdef.h"

#define SCALE_METHOD_HWACINT 0x21

void * video_scale_new(int src_format,
					   int src_w, 
					   int src_h, 
					   int dst_format,
					   int dst_w, 
					   int dst_h,
					   int scale_format);

void video_scale_free(void * context);

int video_scale_call(void * context,
					 const av_picture * src_pic,
					 av_picture * dst_pic);

#endif
