#ifndef __OOK_FFMPEG_H__
#define __OOK_FFMPEG_H__

extern "C" {
	#include "__ffmpeg.h"
}

void lock_avcodec_s();
void unlock_avcodec_s();

#endif
