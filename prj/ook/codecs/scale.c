#include "scale.h"
#include "../pixdef.h"

#ifndef NOFFMPEG_AVCODEC_LIB
#define VIDEOSCALE_FFMPEG
#endif

#ifdef VIDEOSCALE_FFMPEG
#include "libswscale/swscale.h"
#endif

int videoscalefmt_ = -1;

struct video_scale_s
{
	void * context;
	
	int src_stride;
	int src_w;
	int src_h;
	int dst_w;
	int dst_h;
};

#ifdef VIDEOSCALE_FFMPEG

static int __chromaformat(int format)
{
	int chroma = -1;
	switch(format)
	{
	case OOK_FOURCC('I', '4', '2', '0'):
	case OOK_FOURCC('Y', 'V', '1', '2'): // need to xchange UV
		chroma = AV_PIX_FMT_YUV420P;
		break;
	case OOK_FOURCC('U', 'Y', 'V', 'Y'):
	case OOK_FOURCC('H', 'D', 'Y', 'C'):
		chroma = AV_PIX_FMT_UYVY422;
		break;
	case OOK_FOURCC('Y', 'U', 'Y', '2'):
	case OOK_FOURCC('Y', 'U', 'Y', 'V'):
		chroma = AV_PIX_FMT_YUYV422;
		break;
	
	case FCC_FIX_BGR24:
		chroma = AV_PIX_FMT_BGR24;
		break;
	case FCC_FIX_RGB24:
		chroma = AV_PIX_FMT_RGB24; // modify @ 2016/08/13
		break;
	case FCC_FIX_BGRA:
		chroma = AV_PIX_FMT_BGRA;
		break;
	case FCC_FIX_RGBA:
		chroma = AV_PIX_FMT_RGBA;
		break;
	case FCC_FIX_RGB565:
		chroma = AV_PIX_FMT_RGB565;
		break;

	case FCC_FIX_10_YUV422:
		chroma = AV_PIX_FMT_YUV420P10BE;
		break;
	default:
		break;
	}
	return chroma;
}

#endif

void * video_scale_new(int src_format,
					   int src_w, 
					   int src_h, 
					   int dst_format,
					   int dst_w, 
					   int dst_h,
					   int scale_format)
{
	video_scale_s * s = (video_scale_s *)malloc(sizeof(video_scale_s));

#if 0
	char chroma[5];
	memcpy(chroma, &src_format, 4);
	chroma[4] = 0;
	printf("video_scale_new::chroma=%s\n", chroma);
#endif

	s->context    = NULL;
	s->src_stride = 0;
	s->src_w      = src_w;
	s->src_h      = src_h;
	s->dst_w      = dst_w;
	s->dst_h      = dst_h;

#ifdef VIDEOSCALE_FFMPEG
	src_format = __chromaformat(src_format);
	if(src_format < 0)
	{
		delete s;
		return NULL;
	}
	if(dst_format < 0)
	{
		dst_format = src_format;
	}
	else
	{
		dst_format = __chromaformat(dst_format);
		if(dst_format < 0)
		{
			delete s;
			return NULL;
		}
	}
	if(scale_format < 1)
		scale_format = videoscalefmt_ > 0 ? videoscalefmt_ : SWS_BICUBIC;
	///scale_format = 2;
	///printf("scale::format=%d/%d, mode=0x%x\n", src_format, dst_format, scale_format);
  #if 1
	s->context = sws_getContext(src_w, src_h, (AVPixelFormat)src_format,	
							    dst_w, dst_h, (AVPixelFormat)dst_format,
							    scale_format, 
							    NULL, NULL, NULL);
  #else
	s->context = sws_alloc_context();
	if(s->context)
	{
		SwsFilter src;
		SwsFilter dst;
		src.lumH = src_w;
		src.lumV = src_h;
		src.chrH = src_format;
		src.chrV = src_format;
		dst.lumH = dst_w;
		dst.lumV = dst_h;
		dst.chrH = dst_format;
		dst.chrV = dst_format;
		sws_init_context(s->context, &src, &dst);
	}
  #endif  
#endif	
	return s;
}

void video_scale_free(void * context)
{
	video_scale_s * s = (video_scale_s *)context;
	if(s)
	{
		if(s->context)
		{
		#ifdef VIDEOSCALE_FFMPEG
			struct SwsContext * sw_context = (struct SwsContext *)s->context;
			sws_freeContext(sw_context);
		#endif
		}
		free(s);
	}
}

int video_scale_call(void             * context,
					 const av_picture * src_pic,
					 av_picture       * dst_pic)
{
	video_scale_s * s;
#ifdef VIDEOSCALE_FFMPEG
	struct SwsContext * sw_context;
#endif

	s = (video_scale_s *)context;
	if(!s)
		return -2;
	
	if(s->src_stride == 0)
		s->src_stride = src_pic->stride[0];

	if(src_pic->width != s->src_w || src_pic->height != s->src_h)
		return -3;

	dst_pic->framerate = src_pic->framerate;
	dst_pic->stamp     = src_pic->stamp;
	dst_pic->flag      = src_pic->flag;
	dst_pic->index     = src_pic->index;
	
#ifdef VIDEOSCALE_FFMPEG

	sw_context = (struct SwsContext *)s->context;
	if(sw_context)
	{
		return sws_scale(sw_context, 
				         ((av_picture *)src_pic)->data, 
				         ((av_picture *)src_pic)->stride, 
				         0, 
				         s->src_h, 
				         dst_pic->data, 
				         dst_pic->stride);
	}

#endif

	if(src_pic->fourcc == OOK_FOURCC('I', '4', '2', '0'))
	{
		float fx, fy;
		float xscale = (float)s->src_w / (float)s->dst_w;
		float yscale = (float)s->src_h / (float)s->dst_h;
		///printf("xscale=%.2f, yscale=%.2f\n", xscale, yscale);
	
		int y, x;
		int w = s->dst_w;
		int h = s->dst_h;
		int ws = src_pic->stride[0];
				
		unsigned char * Y = dst_pic->data[0];
		unsigned char * U = dst_pic->data[1];
		unsigned char * V = dst_pic->data[2];
		unsigned char * Ys, * Us, * Vs;

		// Y
		for(y = 0; y < h; y++)
		{
			fy = y * yscale;
			///printf("[%d-%d]\n", y, (int)fy);
			Ys = src_pic->data[0] + ws * (int)fy;
			for(x = 0; x < w; x++)
			{
				fx = x * xscale;
				///printf("<%d:%d>", x, (int)fx);
				*Y = Ys[(int)fx];
				Y++;
			}
			///printf("\n");
		}
	
		w >>= 1;
		h >>= 1;
		ws  = src_pic->stride[1];

		// U V
		for(y = 0; y < h; y++)
		{
			fy = y * yscale;
			///printf("[%d-%d]\n", y, (int)fy);
			Us = src_pic->data[1] + ws * (int)fy;
			Vs = src_pic->data[2] + ws * (int)fy;
			for(x = 0; x < w; x++)
			{
				fx = x * xscale;
				///printf("<%d:%d>", x, (int)fx);
				*U = Us[(int)fx];
				*V = Vs[(int)fx];
				U++;
				V++;
			}
			///printf("---\n");
		}
		return 1;
	}
	else if(src_pic->fourcc == OOK_FOURCC('Y', 'U', 'Y', '2') || 
	        src_pic->fourcc == OOK_FOURCC('Y', 'U', 'Y', 'V'))
	{
	#if 0
		if(!_videoenc.pic.data[0])
		{
			_videoenc.pic.data[0] = (unsigned char *)malloc(_videoenc.dstwidth * _videoenc.dstheight * 2);
			_videoenc.pic.stride[0] = (_videoenc.dstwidth << 1);
			_videoenc.pic.fourcc = pic->fourcc;
			_videoenc.pic.selfrelease = 1;
		}

		float xscale, yscale, fx, fy;
		xscale = (float)desc->w / (float)_videoenc.dstwidth;
		yscale = (float)desc->h / (float)_videoenc.dstheight;
		///printf("xscale=%.2f, yscale=%.2f\n", xscale, yscale);

		int y, x;
		int w = _videoenc.dstwidth;
		int h = _videoenc.dstheight;
		int ws = pic->stride[0];
		printf("ws=%d\n", ws);
		
		unsigned char * Y = _videoenc.pic.data[0];
		unsigned char * Ys;
		for(y = 0; y < h; y++)
		{
			fy = y * yscale;
			///printf("[%d-%d]\n", y, (int)fy);
			Ys = pic->data[0] + ws * (int)fy;
			for(x = 0; x < w; x++)
			{
				fx = x * xscale;
				///printf("<%d:%d>", x, (int)fx);
				*Y = Ys[(int)fx];
				Y++;
				if((x % 2) == 0)
					*Y = Ys[(int)fx + 1];
				else
					*Y = Ys[(int)fx + 1];
				Y++;
			}
			///printf("\n");
		}
		pic = &_videoenc.pic;
  	#endif
	}

	return -1;
}
