#ifndef __VIDEO_FORMAT_H__
#define __VIDEO_FORMAT_H__

#define BYTESPERPIXEL			3

#define H263_SRC_UNKNOW 		0
#define H263_SRC_SQCIF  		1
#define H263_SRC_QCIF   		2
#define H263_SRC_CIF   			3
#define H263_SRC_4CIF  			4
#define H263_SRC_16CIF 			5

#define VIDEO_FORMAT_640X480	0x10 // this is not standard H.263 format
#define VIDEO_FORMAT_800X600	0x11
#define VIDEO_FORMAT_320X240	0x12
#define VIDEO_FORMAT_480X352	0x13

#define SQCIF_WIDTH				128
#define SQCIF_HEIGHT			96
#define QCIF_WIDTH				176
#define QCIF_HEIGHT				144
#define CIF_WIDTH				352
#define CIF_HEIGHT				288
#define CIF4_WIDTH				704
#define CIF4_HEIGHT				576
#define CIF16_WIDTH				1408
#define CIF16_HEIGHT			1152

#define VIDEO_FLAG_KEYFRAME 	0x01
#define VIDEO_FLAG_SRCFMTMASK	0x7f000000

#ifdef WIN32
#define WM_BROWSERWINDOW_MSG		(WM_USER + 0x1000)
#define BROWSERWINDOW_VIDEO_HWND1	0x01
#define BROWSERWINDOW_VIDEO_HWND2	0x02
#endif

/*
	1920x1080
	1280x720
	800x450
 */

inline void get_video_formatsize(int srcfmt, int * width, int * height)
{
	int w = 0;
	int h = 0;
	switch(srcfmt)
	{
	case VIDEO_FORMAT_480X352:
		w = 480;
		h = 352;
		break;
	case VIDEO_FORMAT_320X240:
		w = 320;
		h = 240;
		break;
	case VIDEO_FORMAT_640X480:
		w = 640;
		h = 480;
		break;
	case VIDEO_FORMAT_800X600:
		w = 800;
		h = 600;
		break;
	case H263_SRC_SQCIF:
		w = SQCIF_WIDTH;
		h = SQCIF_HEIGHT;
		break;
	case H263_SRC_QCIF:
		w = QCIF_WIDTH;
		h = QCIF_HEIGHT;
		break;
	case H263_SRC_CIF:
		w = CIF_WIDTH;
		h = CIF_HEIGHT;
		break;
	default:
		break;
	}
	if(width)
		*width  = w;
	if(height)
		*height = h;
}

inline int caculate_video_bufsize(int srcfmt)
{
	int bufsize = 0;
	switch(srcfmt)
	{
	case VIDEO_FORMAT_480X352:
		bufsize = 480 * 352 * BYTESPERPIXEL;
		break;
	case VIDEO_FORMAT_320X240:
		bufsize = 320 * 240 * BYTESPERPIXEL;
		break;
	case VIDEO_FORMAT_640X480:
		bufsize = 640 * 480 * BYTESPERPIXEL;
		break;
	case VIDEO_FORMAT_800X600:
		bufsize = 800 * 600 * BYTESPERPIXEL;
		break;
	case H263_SRC_SQCIF: // SQCIF using QCIF for capture so must malloc QCIF buffer
		//bufsize = SQCIF_WIDTH * SQCIF_HEIGHT * BYTESPERPIXEL;
		//break;
	case H263_SRC_QCIF:
		bufsize = QCIF_WIDTH * QCIF_HEIGHT * BYTESPERPIXEL;
		break;
	case H263_SRC_CIF:
	default:
		bufsize = CIF_WIDTH * CIF_HEIGHT * BYTESPERPIXEL;
		break;
	}
	return bufsize;
}

#endif
