#include <string.h>
#include <stdio.h>
#include <ook/codecs/avdef.h>
#ifndef BASICAVDEF
#include <ook/codecs/scale.h>
#endif
#include <ook/fourcc.h>
#include <ook/pixdef.h>

#if 0
void init_pixel_s(unsigned int fourcc, pixel_s * fmt)
{
	fmt->fourcc     = fourcc;
	fmt->bitmask[0] = 0xff;
	fmt->bitmask[1] = 0xff00;
	fmt->bitmask[2] = 0xff0000;
}

unsigned int rgb_2_format(unsigned int rgb, const pixel_s * fmt)
{
	unsigned int l;
	int r, g, b;
	int y, u, v;

	r = (rgb >>  0) & 255;
	g = (rgb >>  8) & 255;
	b = (rgb >> 16) & 255;	
	
	y = ( (2105 * r) + (4128 * g) + ( 802 * b)) / 0x2000 +  16;
	v = ( (3596 * r) - (3015 * g) - ( 582 * b)) / 0x2000 + 128;
	u = (-(1212 * r) - (2384 * g) + (3596 * b)) / 0x2000 + 128;

	l =  (fmt->bitmask[0] / 255) * y;
	l += (fmt->bitmask[1] / 255) * u;
	l += (fmt->bitmask[2] / 255) * v;
	
	return l;
}
#endif

#if 0
#define CLAMP(v) ((v) > 255 ? 255 : (v) < 0 ? 0 : (v))

static inline void I420_2_RV24__(const av_picture * yuv, av_picture * rgb) 
{
	unsigned char * ptry = yuv->data[0];
	unsigned char * ptru = yuv->data[1];
	unsigned char * ptrv = yuv->data[2];
	unsigned char * ptro = rgb->data[0];
	unsigned char * ptro2;
	short pr, pg, pb;
	short r, g, b;
	int i, j;

  	for(i = 0; i < yuv->height; i++) 
  	{
    	ptro2 = ptro;
    	for (j = 0; j < yuv->width; j += 2) 
    	{
			///pr = ((128 + (ptrv[j/2] - 128) * 292) >> 8) - 16; /* 1.14 * 256 */
			pr = (-41344 + ptrv[j/2] * 292) >> 8;
			
			///pg = ((128 - (ptru[j/2] - 128) * 101 - (ptrv[j/2] - 128) * 149) >> 8)-16; /* 0.395 & 0.581 */
			pg = (28032 - ptru[j/2] * 101 - ptrv[j/2] * 149) >> 8;
			
			///pb = ((128 + (ptru[j/2] - 128) * 520) >> 8) - 16; /* 2.032 */
			pb = (-70528 + ptru[j/2] * 520) >> 8;
			
			r = ptry[j] + pr;
			g = ptry[j] + pg;
			b = ptry[j] + pb;

			*ptro2++ = CLAMP(b);
			*ptro2++ = CLAMP(g);
			*ptro2++ = CLAMP(r);
      
			r = ptry[j + 1] + pr;
			g = ptry[j + 1] + pg;
			b = ptry[j + 1] + pb;
			
			*ptro2++ = CLAMP(b);
			*ptro2++ = CLAMP(g);
			*ptro2++ = CLAMP(r);
    	}

    	ptry += yuv->stride[0];
    	if(i & 1) 
   		{
      		ptru += yuv->stride[1];
      		ptrv += yuv->stride[1];
    	}
   		ptro += rgb->stride[0];
  	}
}
#endif

//
// PCM data
//

av_pcmbuff * create_av_pcmbuff(unsigned int assign_pcmlen)
{
	av_pcmbuff * f = (av_pcmbuff *)malloc(sizeof(av_pcmbuff));
	reset_av_pcmbuff(f, assign_pcmlen);
	return f;
}

void reset_av_pcmbuff(av_pcmbuff * f,
					  unsigned int assign_pcmbuf_len)
{
	if(!f)
		return;
	f->channels   = 0;
	f->samplerate = 0;
	f->track      = 0;
	f->stamp      = 0;
	f->flag       = 0;
	f->pcmlen     = assign_pcmbuf_len;
	if(f->pcmlen > 0)
		f->pcmbuf = (unsigned char *)malloc(f->pcmlen + 16);
	else
		f->pcmbuf = NULL;
	f->arg        = NULL;
}

void release_av_pcmbuff(av_pcmbuff * f)
{
	if(f)
	{
		if(f->pcmbuf)
			free(f->pcmbuf);
		reset_av_pcmbuff(f, 0);
	}
}

void free_av_pcmbuff(av_pcmbuff * f)
{
	if(f)
	{
		if(f->pcmbuf)
			free(f->pcmbuf);
		free(f);
	}	
}

void clone_av_pcmbuff(av_pcmbuff * dst, 
					  const av_pcmbuff * src,
					  int copy_pcmbuf)
{
	dst->channels   = src->channels;
	dst->samplerate = src->samplerate;
	dst->track      = src->track;
	dst->stamp      = src->stamp;
	dst->flag       = src->flag;
	dst->arg        = src->arg;
	dst->pcmlen     = src->pcmlen;	
	if(copy_pcmbuf)
	{
		if(dst->pcmlen > 0)
		{
			dst->pcmbuf = (unsigned char *)malloc(dst->pcmlen);
			memcpy(dst->pcmbuf, src->pcmbuf, dst->pcmlen);
		}
		else
		{
			dst->pcmbuf = NULL;
		}
	}
	else
	{
		dst->pcmbuf = src->pcmbuf;
	}
}

//
// av_frame_s
//

void reset_av_frame_s(av_frame_s * frm)
{
	if(frm)
	{
		frm->bits    = NULL;
		frm->bitslen = 0;
		frm->bitspos = 0;
		frm->medtype = 0;
		frm->stmtype = 0;
		frm->frmtype = 0;
		frm->frmmisc = 0;
		frm->tmscale = 90;
		frm->frmsequ = 0;
		frm->pcr     = 0;
		frm->pts     = 0;
		frm->dts     = 0;
		frm->dur     = 0;
		frm->track   = 0;
		frm->arg     = NULL;
		frm->language= 0;
	}
}

void release_av_frame_s(av_frame_s * frm)
{
	if(frm)
	{
		if(frm->bits)
		{
			free(frm->bits);
			frm->bits    = NULL;
			frm->bitslen = 0;
		}
		frm->arg = NULL;
	}
}

void copy_av_frame_s(av_frame_s       * dst, 
					 const av_frame_s * src)
{
	dst->bits    = src->bits;
	dst->bitslen = src->bitslen;
	dst->bitspos = src->bitspos;
	dst->medtype = src->medtype;
	dst->stmtype = src->stmtype;
	dst->frmtype = src->frmtype;
	dst->frmmisc = src->frmmisc;
	dst->tmscale = src->tmscale;
	dst->frmsequ = src->frmsequ;
	dst->pcr     = src->pcr;
	dst->pts     = src->pts;
	dst->dts     = src->dts;
	dst->dur     = src->dur;
	dst->track   = src->track;
	dst->arg     = src->arg;
	dst->language= src->language;
}

void clone_av_frame_s(av_frame_s       * dst, 
					  const av_frame_s * src,
					  unsigned int bitspos,
					  unsigned int extralen)
{
	copy_av_frame_s(dst, src);
	if(src->bits)
	{
		dst->bits = (unsigned char *)malloc(dst->bitspos + dst->bitslen + extralen);
		if(dst->bits)
		{
			memcpy(dst->bits + dst->bitspos, src->bits + src->bitspos, src->bitslen);
			if(extralen > 0)
				memset(dst->bits + dst->bitspos + dst->bitslen, 0, extralen);
		}
	}
}

//
// picture data
//
		    
void reset_av_picture(av_picture * pic)
{
	int i;
	if(pic)
	{
		for(i = 0; i < 4; i++)
		{
			pic->data[i] 	= NULL;
			pic->stride[i] 	= 0;
		}
		pic->width          = 0;
		pic->height         = 0;
		pic->display_width  = -1;
		pic->display_height = -1;
		pic->fourcc 		= -1;
		pic->framerate      = -1;
		pic->selfrelease 	= 0;
		pic->continuebuf	= 0;
		pic->stamp          = 0;
		pic->flag 			= 0;
		pic->index          = 0;
		pic->track          = 0;
		pic->arg			= NULL;
	}
}

void release_av_picture(av_picture * pic)
{
	if(pic)
	{
		if(pic->selfrelease)
		{
			if(pic->continuebuf)
			{
				free(pic->data[0]);
			}
			else
			{
				for(int i = 0; i < 4; i++)
				{
					if(pic->data[i])
						free(pic->data[i]);
				}
			}
		}
		reset_av_picture(pic);
	}
}

void free_av_picture(av_picture * pic)
{
	if(pic)
	{
		if(pic->selfrelease)
		{
			if(pic->continuebuf)
			{
				free(pic->data[0]);
			}
			else
			{
				for(int i = 0; i < 4; i++)
				{
					if(pic->data[i])
						free(pic->data[i]);
				}
			}
		}
		free(pic);
	}	
}

void release_av_picturebf(av_picture * pic)
{
	if(pic)
	{
		if(pic->selfrelease)
		{
			if(pic->continuebuf)
			{
				free(pic->data[0]);
			}
			else
			{
				for(int i = 0; i < 4; i++)
				{
					if(pic->data[i])
						free(pic->data[i]);
				}
			}
		}
	}	
}

av_picture * create_av_picture(int width,
					  		   int height,
					  		   int fourcc)
{
	int w;
	av_picture * pic = NULL;
	switch(fourcc)
	{
	case FCC_FIX_I420:
	case FCC_FIX_YV12:
		{
			pic = (av_picture *)malloc(sizeof(av_picture)); 
			pic->stride[0] = width;
			pic->stride[1] = (width >> 1);
			pic->stride[2] = pic->stride[1];
			pic->stride[3] = 0;
						
			w = width * height + ((pic->stride[1] * height + pic->stride[2] * height) >> 1);
			if(w == 0)
				return NULL;
				
			pic->data[0] = (unsigned char *)malloc(w + 32); // for valgrind check pass
			if(!pic->data[0])
			{
				printf("create_av_picture::malloc data fail, w=%d\n", w);
				return NULL;
			}
			///memset(pic->data[0], 0, w);
			
			pic->data[1]     = pic->data[0] + pic->stride[0] * height;
			pic->data[2]     = pic->data[1] + ((pic->stride[1] * height) >> 1);
			pic->data[3]     = NULL;

			memset(pic->data[0],  16, pic->stride[0] * height);
			memset(pic->data[1], 128, pic->stride[1] * height / 2);
			memset(pic->data[2], 128, pic->stride[2] * height / 2);

			pic->width          = width;
			pic->height         = height;
			pic->display_width  = -1;
			pic->display_height = -1;
			pic->fourcc 	    = fourcc;
			pic->framerate      = -1;
			pic->selfrelease    = 1;
			pic->continuebuf    = 1;
			pic->stamp          = 0;
			pic->flag 		    = 0;
			pic->index          = 0;
			pic->track          = 0;
			pic->arg            = NULL;
			return pic;
		}
		break;
	
	case FCC_FIX_I422:
		{
			pic = (av_picture *)malloc(sizeof(av_picture)); 
			pic->stride[0] = width;
			pic->stride[1] = (width >> 1);
			pic->stride[2] = pic->stride[1];
			pic->stride[3] = 0;
						
			w = width * height * 2;
			if(w == 0)
				return NULL;
				
			pic->data[0] = (unsigned char *)malloc(w + 32); // for valgrind check pass
			if(!pic->data[0])
			{
				printf("create_av_picture::malloc data fail, w=%d\n", w);
				return NULL;
			}
			///memset(pic->data[0], 0, w);
			
			pic->data[1]     = pic->data[0] + pic->stride[0] * height;
			pic->data[2]     = pic->data[1] + pic->stride[1] * height;
			pic->data[3]     = NULL;

			memset(pic->data[0],  16, pic->stride[0] * height);
			memset(pic->data[1], 128, pic->stride[1] * height);
			memset(pic->data[2], 128, pic->stride[2] * height);

			pic->width          = width;
			pic->height         = height;
			pic->display_width  = -1;
			pic->display_height = -1;
			pic->fourcc 	    = fourcc;
			pic->framerate      = -1;
			pic->selfrelease    = 1;
			pic->continuebuf    = 1;
			pic->stamp          = 0;
			pic->flag 		    = 0;
			pic->index          = 0;
			pic->track          = 0;
			pic->arg            = NULL;
			return pic;
		}
		break;
		
	case FCC_FIX_YUY2:
	case FCC_FIX_YUYV:
	case FCC_FIX_UYVY:
	case FCC_FIX_HDYC:
		{
			pic = (av_picture *)malloc(sizeof(av_picture)); 
			pic->stride[0] = width << 1;
			pic->stride[1] = 0;
			pic->stride[2] = 0;
			pic->stride[3] = 0;
			
			w = pic->stride[0] * height;
			if(w == 0)
				return NULL;
				
			pic->data[0] = (unsigned char *)malloc(w + 32);
			if(!pic->data[0])
			{
				printf("create_av_picture::malloc data fail, w=%d\n", w);
				return NULL;
			}
			
			///memset(pic->data[0], 0, w);
			{
				w = width << 1;
				int i, j;
				unsigned char * p = pic->data[0];
				for(j = 0; j < height; j++)
				{
					for(i = 0; i < w; i += 2)
					{
						switch(fourcc)
						{
						case OOK_FOURCC('U', 'Y', 'V', 'Y'):
						case OOK_FOURCC('H', 'D', 'Y', 'C'):
							p[i]     = 128;
							p[i + 1] =  16;
							break;						
						default:
							p[i]     =  16;
							p[i + 1] = 128;
							break;
						}
					}
					p += pic->stride[0];
				}
			}
						
			pic->data[1]     = NULL;
			pic->data[2]     = NULL;
			pic->data[3]     = NULL;
	
			pic->width          = width;
			pic->height         = height;
			pic->display_width  = -1;
			pic->display_height = -1;			
			pic->fourcc 	    = fourcc;
			pic->framerate      = -1;
			pic->selfrelease    = 1;
			pic->continuebuf    = 1;
			pic->stamp          = 0;
			pic->flag 		    = 0;
			pic->index          = 0;
			pic->track          = 0;
			pic->arg            = NULL;
			return pic;
		}
		break;

	case FCC_FIX_RGB8:
	case FCC_FIX_BGR8:
	case FCC_FIX_RGB555:
	case FCC_FIX_BGR555:
	case FCC_FIX_RGB565:
	case FCC_FIX_BGR565:
	case FCC_FIX_RGB24:
	case FCC_FIX_BGR24:
	case OOK_FOURCC('I', 'Y', 'U', '2'):
	case FCC_FIX_RGBA:
	case FCC_FIX_BGRA:
		{
			int pix = 3;
			switch(fourcc)
			{
			case FCC_FIX_RGB8:
			case FCC_FIX_BGR8:
				pix = 1;
				break;			
			case FCC_FIX_RGB555:
			case FCC_FIX_BGR555:
			case FCC_FIX_RGB565:
			case FCC_FIX_BGR565:
				pix = 2;
				break;
			case FCC_FIX_RGBA:
			case FCC_FIX_BGRA:
				pix = 4;
				break;
			default:
				break;
			}

			pic = (av_picture *)malloc(sizeof(av_picture)); 
			pic->stride[0] = width * pix;
			pic->stride[1] = 0;
			pic->stride[2] = 0;
			pic->stride[3] = 0;
			
			w = pic->stride[0] * height;
			if(w == 0)
				return NULL;

			pic->data[0] = (unsigned char *)malloc(w + 32);
			if(!pic->data[0])
			{
				printf("create_av_picture::malloc data fail, w=%d\n", w);
				return NULL;
			}
			memset(pic->data[0], 0, w);
			
			pic->data[1]     = NULL;
			pic->data[2]     = NULL;
			pic->data[3]     = NULL;
	
			pic->width          = width;
			pic->height         = height;
			pic->display_width  = -1;
			pic->display_height = -1;			
			pic->fourcc 	    = fourcc;
			pic->framerate      = -1;
			pic->selfrelease    = 1;
			pic->continuebuf    = 1;
			pic->stamp          = 0;
			pic->flag 		    = 0;
			pic->index          = 0;
			pic->track          = 0;
			pic->arg            = NULL;
			return pic;
		}
		break;
	default:
		break;
	}
	return NULL;
}

int clone_av_picture(av_picture       * dst, 
					 const av_picture * src, 
					 int height,
					 int selfrelease)
{
	int w;
	if(!dst)
		return -1;

	reset_av_picture(dst);
	if(!src || height < 1)
		return 0;

	switch(src->fourcc)
	{
	case FCC_FIX_I420:
	case FCC_FIX_YV12:
		{
			///printf("[%d: %d|%d|%d]\n", height, src->stride[0], src->stride[1], src->stride[2]);
			w = src->stride[0] * height + ((src->stride[1] * height + src->stride[2] * height) >> 1);
			if(w == 0)
			{
				printf("clone_av_picture::malloc data fail, w=%d\n", w);
				return -2;
			}
			
			dst->data[0] = (unsigned char *)malloc(w + 32);
			if(!dst->data[0])
			{
				printf("clone_av_picture::malloc data fail, w=%d\n", w);
				return -3;
			}

			dst->data[1]        = dst->data[0] + src->stride[0] * height;
			dst->data[2]        = dst->data[1] + ((src->stride[1] * height) >> 1);
			dst->data[3]        = NULL;
			dst->stride[0]      = src->stride[0];
			dst->stride[1]      = src->stride[1];
			dst->stride[2]      = src->stride[2];
			dst->stride[3]      = 0;

			dst->width          = src->width;
			dst->height         = height;
			dst->display_width  = src->display_width;
			dst->display_height = src->display_height;
			dst->fourcc 	    = src->fourcc;
			dst->framerate      = src->framerate;
			dst->selfrelease    = selfrelease;
			dst->continuebuf    = 1;
			dst->stamp          = src->stamp;
			dst->flag 		    = src->flag;
			dst->index          = src->index;
			dst->track          = src->track;
			dst->arg            = NULL;

			if(src->data[0] && dst->stride[0] > 0)
			{
				memcpy(dst->data[0], src->data[0], dst->stride[0] * height);
			}
			if(src->data[1] && dst->stride[1] > 0)
			{
				memcpy(dst->data[1], src->data[1], ((dst->stride[1] * height) >> 1));
			}
			if(src->data[2] && dst->stride[2] > 0)
			{
				memcpy(dst->data[2], src->data[2], ((dst->stride[2] * height) >> 1));
			}
		}
		break;
	
	case FCC_FIX_I422:
		{
			w = src->stride[0] * height * 2;
			if(w == 0)
			{
				printf("clone_av_picture::malloc data fail, w=%d\n", w);
				return -2;
			}
			
			dst->data[0] = (unsigned char *)malloc(w + 32);
			if(!dst->data[0])
			{
				printf("clone_av_picture::malloc data fail, w=%d\n", w);
				return -3;
			}

			dst->data[1]        = dst->data[0] + src->stride[0] * height;
			dst->data[2]        = dst->data[1] + src->stride[1] * height;
			dst->data[3]        = NULL;
			dst->stride[0]      = src->stride[0];
			dst->stride[1]      = src->stride[1];
			dst->stride[2]      = src->stride[2];
			dst->stride[3]      = 0;

			dst->width          = src->width;
			dst->height         = height;
			dst->display_width  = src->display_width;
			dst->display_height = src->display_height;
			dst->fourcc 	    = src->fourcc;
			dst->framerate      = src->framerate;
			dst->selfrelease    = selfrelease;
			dst->continuebuf    = 1;
			dst->stamp          = src->stamp;
			dst->flag 		    = src->flag;
			dst->index          = src->index;
			dst->track          = src->track;
			dst->arg            = NULL;

			if(src->data[0] && dst->stride[0] > 0)
			{
				memcpy(dst->data[0], src->data[0], dst->stride[0] * height);
			}
			if(src->data[1] && dst->stride[1] > 0)
			{
				memcpy(dst->data[1], src->data[1], dst->stride[1] * height);
			}
			if(src->data[2] && dst->stride[2] > 0)
			{
				memcpy(dst->data[2], src->data[2], dst->stride[2] * height);
			}
		}
		break;
			
	case FCC_FIX_10_YUV422:
		{
			///printf("[%d: %d|%d|%d]\n", height, src->stride[0], src->stride[1], src->stride[2]);
			w = src->stride[0] * height + (src->stride[1] * height + src->stride[2] * height);
			if(w == 0)
			{
				printf("clone_av_picture::malloc data fail, w=%d\n", w);
				return -2;
			}
			
			dst->data[0] = (unsigned char *)malloc(w + 32);
			if(!dst->data[0])
			{
				printf("clone_av_picture::malloc data fail, w=%d\n", w);
				return -3;
			}

			dst->data[1]     = dst->data[0] + src->stride[0] * height;
			dst->data[2]     = dst->data[1] + src->stride[1] * height;
			dst->data[3]     = NULL;
			dst->stride[0]   = src->stride[0];
			dst->stride[1]   = src->stride[1];
			dst->stride[2]   = src->stride[2];
			dst->stride[3]   = 0;

			dst->width          = src->width;
			dst->height         = height;
			dst->display_width  = src->display_width;
			dst->display_height = src->display_height;
			dst->fourcc 	    = src->fourcc;
			dst->framerate      = src->framerate;
			dst->selfrelease    = selfrelease;
			dst->continuebuf    = 1;
			dst->stamp          = src->stamp;
			dst->flag 		    = src->flag;
			dst->index          = src->index;
			dst->track          = src->track;
			dst->arg            = NULL;

			if(src->data[0] && dst->stride[0] > 0)
			{
				memcpy(dst->data[0], src->data[0], dst->stride[0] * height);
			}
			if(src->data[1] && dst->stride[1] > 0)
			{
				memcpy(dst->data[1], src->data[1], dst->stride[1] * height);
			}
			if(src->data[2] && dst->stride[2] > 0)
			{
				memcpy(dst->data[2], src->data[2], dst->stride[2] * height);
			}
		}
		break;
		
	case OOK_FOURCC('Y', 'U', 'Y', 'V'):
	case OOK_FOURCC('Y', 'U', 'Y', '2'):
	case OOK_FOURCC('U', 'Y', 'V', 'Y'):
	case OOK_FOURCC('H', 'D', 'Y', 'C'):
		{
			w = src->stride[0] * height;
			///printf("clone_av_picture::pic=%d/%d/%d\n", src->stride[0], height, w);
			if(w == 0)
			{
				printf("clone_av_picture::malloc data fail, w=%d\n", w);
				return -2;
			}
			
			dst->data[0] = (unsigned char *)malloc(w + 32);
			if(!dst->data[0])
			{
				printf("clone_av_picture::malloc data fail, w=%d\n", w);
				return -3;
			}
						
			dst->data[1]     = NULL;
			dst->data[2]     = NULL;
			dst->data[3]     = NULL;
			dst->stride[0]   = src->stride[0];
			dst->stride[1]   = 0;
			dst->stride[2]   = 0;
			dst->stride[3]   = 0;

			dst->width          = src->width;
			dst->height         = height;
			dst->display_width  = src->display_width;
			dst->display_height = src->display_height;			
			dst->fourcc 	    = src->fourcc;
			dst->framerate      = src->framerate;
			dst->selfrelease    = selfrelease;
			dst->continuebuf    = 1;
			dst->stamp          = src->stamp;
			dst->flag 		    = src->flag;
			dst->index          = src->index;
			dst->track          = src->track;
			dst->arg            = NULL;

			if(src->data[0] && dst->stride[0] > 0)
			{
				///printf("<<<\n");
				memcpy(dst->data[0], src->data[0], dst->stride[0] * height);
				///printf(">>>\n");
			}
		}
		break;
	
	case OOK_FOURCC('I', 'Y', 'U', '2'):
	case FCC_FIX_RGB8:
	case FCC_FIX_BGR8:
	case FCC_FIX_RGB555:
	case FCC_FIX_BGR555:	
	case FCC_FIX_RGB565:
	case FCC_FIX_BGR565:
	case FCC_FIX_RGB24:
	case FCC_FIX_BGR24:
	case FCC_FIX_RGBA:
	case FCC_FIX_BGRA:
		{
			w = src->stride[0] * height;
			if(w == 0)
			{
				printf("clone_av_picture::malloc data fail, w=%d\n", w);
				return -2;
			}
			
			dst->data[0] = (unsigned char *)malloc(w + 32);
			if(!dst->data[0])
			{
				printf("clone_av_picture::malloc data fail, w=%d\n", w);
				return -3;
			}
						
			dst->data[1]     = NULL;
			dst->data[2]     = NULL;
			dst->data[3]     = NULL;
			dst->stride[0]   = src->stride[0];
			dst->stride[1]   = 0;
			dst->stride[2]   = 0;
			dst->stride[3]   = 0;

			dst->width          = src->width;
			dst->height         = height;
			dst->display_width  = src->display_width;
			dst->display_height = src->display_height;			
			dst->fourcc 	    = src->fourcc;
			dst->framerate      = src->framerate;
			dst->selfrelease    = selfrelease;
			dst->continuebuf    = 1;
			dst->stamp          = src->stamp;
			dst->flag 		    = src->flag;
			dst->index          = src->index;
			dst->track          = src->track;
			dst->arg            = NULL;

			if(src->data[0] && dst->stride[0] > 0)
			{
				memcpy(dst->data[0], src->data[0], dst->stride[0] * height);
			}			
		}
		break;
	default:
		return -2;
	}
	return 0;
}

int clone_av_picture2(av_picture * dst, 
					  const av_picture * src, 
					  int height,
					  int selfrelease,
					  unsigned int miscmask)
{
	int i, w, h;
	if(!dst)
		return -1;
	reset_av_picture(dst);
	if(!src || height < 1)
		return 0;

	switch(src->fourcc)
	{
	case FCC_FIX_I420:
	case FCC_FIX_YV12:
		{
			w = src->stride[0] * height + ((src->stride[1] * height + src->stride[2] * height) >> 1);
			///printf("[%d:%d:%d : %d:%d] <\n", src->stride[0], src->stride[1], src->stride[2], height, w);
			if(w == 0)
			{
				printf("clone_av_picture::malloc data fail, w=%d\n", w);
				return -2;
			}
			
			dst->data[0] = (unsigned char *)malloc(w + 32);
			if(!dst->data[0])
			{
				printf("clone_av_picture::malloc data fail, w=%d\n", w);
				return -3;
			}

			if(miscmask & CLONE_AVPIC_MISCMASK_WES) // in this case stride = width
			{
				unsigned char * y_d;
				unsigned char * u_d;
				unsigned char * v_d;
				unsigned char * y_s;
				unsigned char * u_s;
				unsigned char * v_s;

				dst->stride[0] = src->width;
				dst->stride[1] = src->width >> 1;
				dst->stride[2] = dst->stride[1];
				dst->stride[3] = 0;
				
				dst->data[1]   = dst->data[0] + dst->stride[0] * height;
				dst->data[2]   = dst->data[1] + ((dst->stride[1] * height) >> 1);
				dst->data[3]   = NULL;
				
				y_d = dst->data[0];
				u_d = dst->data[1];
				v_d = dst->data[2];
				y_s = src->data[0];
				u_s = src->data[1];
				v_s = src->data[2];

				// Y
				w = src->width;
				h = height;
				for(i = 0; i < h; i++)
				{
					memcpy(y_d, y_s, w);
					y_d += dst->stride[0];
					y_s += src->stride[0];
				}
				// U/V
				w >>= 1;
				h >>= 1;
				for(i = 0; i < h; i++)
				{
					memcpy(u_d, u_s, w);
					memcpy(v_d, v_s, w);
					u_d += dst->stride[1];
					u_s += src->stride[1];
					v_d += dst->stride[2];
					v_s += src->stride[2];
				}
			}
			else
			{
				dst->data[1]   = dst->data[0] + src->stride[0] * height;
				dst->data[2]   = dst->data[1] + ((src->stride[1] * height) >> 1);
				dst->data[3]   = NULL;
				dst->stride[0] = src->stride[0];
				dst->stride[1] = src->stride[1];
				dst->stride[2] = src->stride[2];
				dst->stride[3] = 0;
			
				if(src->data[0] && dst->stride[0] > 0)
				{
					memcpy(dst->data[0], src->data[0], dst->stride[0] * height);
				}
				if(src->data[1] && dst->stride[1] > 0)
				{
					memcpy(dst->data[1], src->data[1], ((dst->stride[1] * height) >> 1));
				}
				if(src->data[2] && dst->stride[2] > 0)
				{
					memcpy(dst->data[2], src->data[2], ((dst->stride[2] * height) >> 1));
				}
			}

			dst->width          = src->width;
			dst->height         = height;
			dst->display_width  = src->display_width;
			dst->display_height = src->display_height;			
			dst->fourcc 	    = src->fourcc;
			dst->framerate      = src->framerate;
			dst->selfrelease    = selfrelease;
			dst->continuebuf    = 1;
			dst->stamp          = src->stamp;
			dst->flag 		    = src->flag;
			dst->index          = src->index;
			dst->track          = src->track;
			dst->arg            = NULL;
		}
		break;
		
	case FCC_FIX_I422:
		{
			w = src->stride[0] * height * 2;
			///printf("[%d:%d:%d : %d:%d] <\n", src->stride[0], src->stride[1], src->stride[2], height, w);
			if(w == 0)
			{
				printf("clone_av_picture::malloc data fail, w=%d\n", w);
				return -2;
			}
			
			dst->data[0] = (unsigned char *)malloc(w + 32);
			if(!dst->data[0])
			{
				printf("clone_av_picture::malloc data fail, w=%d\n", w);
				return -3;
			}

			if(miscmask & CLONE_AVPIC_MISCMASK_WES) // in this case stride = width
			{
				unsigned char * y_d;
				unsigned char * u_d;
				unsigned char * v_d;
				unsigned char * y_s;
				unsigned char * u_s;
				unsigned char * v_s;

				dst->stride[0] = src->width;
				dst->stride[1] = src->width >> 1;
				dst->stride[2] = dst->stride[1];
				dst->stride[3] = 0;
				
				dst->data[1]   = dst->data[0] + dst->stride[0] * height;
				dst->data[2]   = dst->data[1] + dst->stride[1] * height;
				dst->data[3]   = NULL;
				
				y_d = dst->data[0];
				u_d = dst->data[1];
				v_d = dst->data[2];
				y_s = src->data[0];
				u_s = src->data[1];
				v_s = src->data[2];

				// Y
				w = src->width;
				h = height;
				for(i = 0; i < h; i++)
				{
					memcpy(y_d, y_s, w);
					y_d += dst->stride[0];
					y_s += src->stride[0];
				}
				// U/V
				w >>= 1;
				for(i = 0; i < h; i++)
				{
					memcpy(u_d, u_s, w);
					memcpy(v_d, v_s, w);
					u_d += dst->stride[1];
					u_s += src->stride[1];
					v_d += dst->stride[2];
					v_s += src->stride[2];
				}
			}
			else
			{
				dst->data[1]   = dst->data[0] + src->stride[0] * height;
				dst->data[2]   = dst->data[1] + src->stride[1] * height;
				dst->data[3]   = NULL;
				dst->stride[0] = src->stride[0];
				dst->stride[1] = src->stride[1];
				dst->stride[2] = src->stride[2];
				dst->stride[3] = 0;
			
				if(src->data[0] && dst->stride[0] > 0)
				{
					memcpy(dst->data[0], src->data[0], dst->stride[0] * height);
				}
				if(src->data[1] && dst->stride[1] > 0)
				{
					memcpy(dst->data[1], src->data[1], dst->stride[1] * height);
				}
				if(src->data[2] && dst->stride[2] > 0)
				{
					memcpy(dst->data[2], src->data[2], dst->stride[2] * height);
				}
			}

			dst->width          = src->width;
			dst->height         = height;
			dst->display_width  = src->display_width;
			dst->display_height = src->display_height;			
			dst->fourcc 	    = src->fourcc;
			dst->framerate      = src->framerate;
			dst->selfrelease    = selfrelease;
			dst->continuebuf    = 1;
			dst->stamp          = src->stamp;
			dst->flag 		    = src->flag;
			dst->index          = src->index;
			dst->track          = src->track;
			dst->arg            = NULL;			
		}
		break;
		
	case OOK_FOURCC('Y', 'U', 'Y', '2'):
	case OOK_FOURCC('U', 'Y', 'V', 'Y'):
	case OOK_FOURCC('H', 'D', 'Y', 'C'):
		{
			w = src->stride[0] * height;
			if(w == 0)
			{
				printf("clone_av_picture::malloc data fail, w=%d\n", w);
				return -2;
			}
			
			dst->data[0] = (unsigned char *)malloc(w + 32);
			if(!dst->data[0])
			{
				printf("clone_av_picture::malloc data fail, w=%d\n", w);
				return -3;
			}
						
			dst->data[1]     = NULL;
			dst->data[2]     = NULL;
			dst->data[3]     = NULL;
			dst->stride[0]   = src->stride[0];
			dst->stride[1]   = 0;
			dst->stride[2]   = 0;
			dst->stride[3]   = 0;

			dst->width          = src->width;
			dst->height         = height;
			dst->display_width  = src->display_width;
			dst->display_height = src->display_height;			
			dst->fourcc 	    = src->fourcc;
			dst->framerate      = src->framerate;
			dst->selfrelease    = selfrelease;
			dst->continuebuf    = 1;
			dst->stamp          = src->stamp;
			dst->flag 		    = src->flag;
			dst->index          = src->index;
			dst->track          = src->track;
			dst->arg            = NULL;

			if(src->data[0] && dst->stride[0] > 0)
			{
				memcpy(dst->data[0], src->data[0], dst->stride[0] * height);
			}
		}
		break;
		
	case OOK_FOURCC('I', 'Y', 'U', '2'):
	case FCC_FIX_RGB8:
	case FCC_FIX_BGR8:
	case FCC_FIX_RGB555:
	case FCC_FIX_BGR555:	
	case FCC_FIX_RGB565:
	case FCC_FIX_BGR565:	
	case FCC_FIX_RGB24:
	case FCC_FIX_BGR24:
	case FCC_FIX_RGBA:
	case FCC_FIX_BGRA:
		{
			w = src->stride[0] * height;
			if(w == 0)
			{
				printf("clone_av_picture::malloc data fail, w=%d\n", w);
				return -2;
			}
			
			dst->data[0] = (unsigned char *)malloc(w + 32);
			if(!dst->data[0])
			{
				printf("clone_av_picture::malloc data fail, w=%d\n", w);
				return -3;
			}
						
			dst->data[1]     = NULL;
			dst->data[2]     = NULL;
			dst->data[3]     = NULL;
			dst->stride[0]   = src->stride[0];
			dst->stride[1]   = 0;
			dst->stride[2]   = 0;
			dst->stride[3]   = 0;

			dst->width          = src->width;
			dst->height         = height;
			dst->display_width  = src->display_width;
			dst->display_height = src->display_height;			
			dst->fourcc 	    = src->fourcc;
			dst->framerate      = src->framerate;
			dst->selfrelease    = selfrelease;
			dst->continuebuf    = 1;
			dst->stamp          = src->stamp;
			dst->flag 		    = src->flag;
			dst->index          = src->index;
			dst->track          = src->track;
			dst->arg            = NULL;

			if(src->data[0] && dst->stride[0] > 0)
			{
				memcpy(dst->data[0], src->data[0], dst->stride[0] * height);
			}			
		}
	default:
		return -2;
	}
	return 0;
}

int copy_av_picture(av_picture       * dst, 
					const av_picture * src, 
					int height)
{
	int i, w;
	int x = 0, y = 0;
	unsigned char * y1, * u1, * v1;
	unsigned char * y2, * u2, * v2;
	if(!dst || !src)
		return -1;
	if(dst->width < src->width)
		return -3;
	if(height < 1)
		return 0;

	w = dst->width;
	if(w > src->width)
	{
		x = (w - src->width) >> 1;
		w = src->width;
	}
	if(height > src->height)
		height = src->height;
	if(dst->height > height)
		y = (dst->height - height) >> 1;
		
	dst->display_width  = src->display_width;
	dst->display_height = src->display_height;
	dst->framerate      = src->framerate;
	dst->stamp          = src->stamp;
	dst->flag           = src->flag;
	dst->index          = src->index;
	dst->track          = src->track;
	dst->arg            = src->arg;			
	
	switch(src->fourcc)
	{
	case FCC_FIX_I420:
	case FCC_FIX_YV12:
		{
			y1 = dst->data[0];
			u1 = dst->data[1];
			v1 = dst->data[2];
			if(dst->fourcc == src->fourcc)
			{
				y2 = src->data[0];
				u2 = src->data[1];
				v2 = src->data[2];
			}
			else if(dst->fourcc == OOK_FOURCC('Y', 'V', '1', '2') || dst->fourcc == OOK_FOURCC('I', '4', '2', '0'))
			{
				y2 = src->data[0];
				u2 = src->data[2];
				v2 = src->data[1];				
			}
			else
			{
				return -2;
			}
			
			for(i = y; i < height; i++)
			{
				memcpy(y1 + x, y2, w);
				y1 += dst->stride[0];
				y2 += src->stride[0];
				if((i % 2) == 0)
				{
					memcpy(u1 + (x >> 1), u2, (w >> 1));
					memcpy(v1 + (x >> 1), v2, (w >> 1));
					u1 += dst->stride[1];
					u2 += src->stride[1];
					v1 += dst->stride[2];
					v2 += src->stride[2];										
				}
			}
		}
		break;
		
	case FCC_FIX_I422:
		{
			y1 = dst->data[0];
			u1 = dst->data[1];
			v1 = dst->data[2];

			y2 = src->data[0];
			u2 = src->data[1];
			v2 = src->data[2];
			
			for(i = y; i < height; i++)
			{
				memcpy(y1 + x, y2, w);
				y1 += dst->stride[0];
				y2 += src->stride[0];

				memcpy(u1 + (x >> 1), u2, (w >> 1));
				memcpy(v1 + (x >> 1), v2, (w >> 1));
				u1 += dst->stride[1];
				u2 += src->stride[1];
				v1 += dst->stride[2];
				v2 += src->stride[2];
			}			
		}
		break;
		
	case OOK_FOURCC('R', 'V', '2', '4'):
	case OOK_FOURCC('R', 'V', '3', '2'):
	case FCC_FIX_BGRA:
		{
			if(dst->fourcc != src->fourcc)
				return -2;
				
			switch(src->fourcc)
			{
			case OOK_FOURCC('R', 'V', '3', '2'):
			case FCC_FIX_BGRA:
				w *= 4;
				x *= 4;
				break;
			default:
				w *= 3;
				x *= 3;
				break;
			}
			y1 = dst->data[0];
			y2 = src->data[0];
			for(i = y; i < height; i++)
			{
				memcpy(y1 + x, y2, w);
				y1 += dst->stride[0];
				y2 += src->stride[0];
			}			
		}
		break;
	default:
		return -5;
	}	
	return 0;
}

int black_av_picture(av_picture * pic)
{
	if(!pic)
		return -1;
	switch(pic->fourcc)
	{
	case FCC_FIX_I420:
	case FCC_FIX_YV12:
		{
			if(pic->data[0] && pic->stride[0] > 0)
			{
				unsigned char * p = pic->data[0];
				for(int i = 0; i < pic->height; i++)
				{
					memset(p, 16, pic->stride[0]);
					p += pic->stride[0];
				}
			}
			if(pic->data[1] && pic->stride[1] > 0)
			{
				unsigned char * p = pic->data[1];
				for(int i = 0; i < pic->height; i += 2)
				{
					memset(p, 128, pic->stride[1]);
					p += pic->stride[1];
				}				
			}
			if(pic->data[2] && pic->stride[2] > 0)
			{
				unsigned char * p = pic->data[2];
				for(int i = 0; i < pic->height; i += 2)
				{
					memset(p, 128, pic->stride[2]);
					p += pic->stride[2];
				}
			}
		}
		break;
	
	case FCC_FIX_I422:
		{
			if(pic->data[0] && pic->stride[0] > 0)
			{
				unsigned char * p = pic->data[0];
				for(int i = 0; i < pic->height; i++)
				{
					memset(p, 16, pic->stride[0]);
					p += pic->stride[0];
				}
			}
			if(pic->data[1] && pic->stride[1] > 0)
			{
				unsigned char * p = pic->data[1];
				for(int i = 0; i < pic->height; i++)
				{
					memset(p, 128, pic->stride[1]);
					p += pic->stride[1];
				}				
			}
			if(pic->data[2] && pic->stride[2] > 0)
			{
				unsigned char * p = pic->data[2];
				for(int i = 0; i < pic->height; i++)
				{
					memset(p, 128, pic->stride[2]);
					p += pic->stride[2];
				}
			}			
		}
		break;
		
	case OOK_FOURCC('Y', 'U', 'Y', '2'):
	case OOK_FOURCC('Y', 'U', 'Y', 'V'):
	case OOK_FOURCC('U', 'Y', 'V', 'Y'):
	case OOK_FOURCC('H', 'D', 'Y', 'C'):
		if(pic->stride[0] > 0)
		{
			unsigned char * p = pic->data[0];
			if(p)
			{
				int w = pic->width << 1;
				int h = pic->height;
				for(int j = 0; j < h; j++)
				{
					for(int i = 0; i < w; i += 2)
					{
						switch(pic->fourcc)
						{
						case OOK_FOURCC('U', 'Y', 'V', 'Y'):
						case OOK_FOURCC('H', 'D', 'Y', 'C'):
							p[i]     = 128;
							p[i + 1] =  16;
							break;						
						default:
							p[i]     =  16;
							p[i + 1] = 128;
							break;
						}
					}
					p += pic->stride[0];
				}
			}
		}
		break;
	case OOK_FOURCC('I', 'Y', 'U', '2'):
	case FCC_FIX_RGB24:
	case FCC_FIX_BGR24:
	case FCC_FIX_RGBA:
	case FCC_FIX_BGRA:	
		if(pic->stride[0] > 0)
		{
			unsigned char * p = pic->data[0];
			if(p)
			{
				for(int i = 0; i < pic->height; i++)
				{
					memset(p, 0, pic->stride[0]);
					p += pic->stride[0];
				}
			}
		}
		break;
	default:
		return -2;
	}
	return 0;	
}

void attach_av_picture(av_picture       * dst, 
					   const av_picture * src)
{
	int i;
	if(dst && src)
	{
		for(i = 0; i < 4; i++)
		{
			dst->data[i] 	= src->data[i];
			dst->stride[i] 	= src->stride[i];
		}
		dst->width          = src->width;
		dst->height         = src->height;
		dst->display_width  = src->display_width;
		dst->display_height = src->display_height;		
		dst->fourcc 		= src->fourcc;
		dst->framerate      = src->framerate;
		dst->selfrelease 	= 0;
		dst->continuebuf	= src->continuebuf;
		dst->stamp          = src->stamp;
		dst->flag 			= src->flag;
		dst->index          = src->index;
		dst->track          = src->track;
		dst->arg			= src->arg;
	}
}

int extract_av_picture(av_picture       * dst, 
					   const av_picture * src,
					   int top_field)
{
	switch(src->fourcc)
	{
	case OOK_FOURCC('I', '4', '2', '0'):
		if(dst->fourcc == OOK_FOURCC('I', '4', '2', '0'))
		{
			int i;
			int wy = dst->width;
			int wu = wy >> 1;
			int sy = src->stride[0] << 1;
			int su = src->stride[1] << 1;
			int sv = src->stride[2] << 1;
			
			unsigned char * src_y = src->data[0];
			unsigned char * dst_y = dst->data[0];
			unsigned char * src_u = src->data[1];
			unsigned char * dst_u = dst->data[1];
			unsigned char * src_v = src->data[2];
			unsigned char * dst_v = dst->data[2];
			if(!top_field)
			{
				src_y += src->stride[0];
				src_u += src->stride[1];
				src_v += src->stride[2];
			}				
			
			for(i = 0; i < dst->height; i++)
			{
				// Y
				memcpy(dst_y, src_y, wy);
				src_y += sy;
				dst_y += dst->stride[0];
				if((i % 2) == 0)
				{
					// U
					memcpy(dst_u, src_u, wu);
					src_u += su;
					dst_u += dst->stride[1];
					// V
					memcpy(dst_v, src_v, wu);
					src_v += sv;
					dst_v += dst->stride[2];										
				}
			}
			return 0;
		}
		return -2;
	default:
		break;
	}
	return -1;
}

int combine_av_picture(av_picture * dst,
					   const av_picture * pic_t, 
					   const av_picture * pic_b)
{
	if(dst->fourcc != pic_t->fourcc || dst->fourcc != pic_b->fourcc)
		return -2;
	
	switch(dst->fourcc)
	{
	case OOK_FOURCC('I', '4', '2', '0'):
		{
			unsigned char * y   = dst->data[0];
			unsigned char * u   = dst->data[1];
			unsigned char * v   = dst->data[2];
												
			unsigned char * t_y = pic_t->data[0];
			unsigned char * t_u = pic_t->data[1];
			unsigned char * t_v = pic_t->data[2];

			unsigned char * b_y = pic_b->data[0];
			unsigned char * b_u = pic_b->data[1];
			unsigned char * b_v = pic_b->data[2];
			
			int i;
			int wy = dst->width;
			int wu = wy >> 1;
			for(i = 0; i < pic_t->height; i++)
			{
				// Y
				memcpy(y, t_y, wy);
				y   += dst->stride[0];
				t_y += pic_t->stride[0];
				memcpy(y, b_y, wy);
				y   += dst->stride[0];
				b_y += pic_b->stride[0];

				if((i % 2) == 0)
				{
					// U
					memcpy(u, t_u, wu);
					u   += dst->stride[1];
					t_u += pic_t->stride[1];
					memcpy(u, b_u, wu);
					u   += dst->stride[1];
					b_u += pic_b->stride[1];					
					// V
					memcpy(v, t_v, wu);
					v   += dst->stride[2];
					t_v += pic_t->stride[2];
					memcpy(v, b_v, wu);
					v   += dst->stride[2];
					b_v += pic_t->stride[2];														
				}
			}
			return 0;
		}
		return -2;
	default:
		break;
	}
	return -1;
}

void reset_av_package_s(av_package_s * s)
{
	s->enc_t   = -1;
	s->pkg_t   = -1;
	s->med_t   = -1;
	s->stamp   = 0;
	s->bits    = NULL;
	s->bitslen = 0;
	s->bitspos = 0;
	s->arg     = NULL;
}

#ifndef BASICAVDEF

struct __av_operator
{
	int opt;
	int param;
	
	void * scale1;
	void * scale2;
	
	av_picture * rgb_pic;
	av_picture * yuv_pic;
};

av_operator create_av_operator()
{
	__av_operator * opera = (__av_operator *)malloc(sizeof(__av_operator));
	memset(opera, 0, sizeof(__av_operator));
	return (av_operator)opera;
}

void release_av_operator(void * ptr)
{
	if(ptr)
	{
		__av_operator * opera = (__av_operator *)ptr;
		if(opera->scale1)
			video_scale_free(opera->scale1);
		if(opera->scale2)
			video_scale_free(opera->scale2);
		if(opera->rgb_pic)
		{
			release_av_picture(opera->rgb_pic);
			free(opera->rgb_pic);
		}
		if(opera->yuv_pic)
		{
			release_av_picture(opera->yuv_pic);
			free(opera->yuv_pic);
		}
		free(opera);
	}
}

int operate_av_picture(int opt,
					   int param,
					   int x,
					   int y,
					   unsigned int bgcolor,
					   unsigned int bgmask,
                       av_picture       * dst, 
                       const av_picture * src,
                       av_operator ptr)
{
	int i, j, k, l, w, h, xu = 0, wu = 0, xc = 0;
	unsigned int m, srcfourcc;
	unsigned char * y1, * u1, * v1;
	unsigned char * y2, * u2, * v2;
	
	if(opt == AVPICTURE_OPERATION_OPTMASK)
	{
		if(!dst || !src)
			return -1;

		///printf("[%d/%d %d/%d %d/%d %x/%x]\n", x, y, src->width, src->height, dst->width, dst->height, src->fourcc, dst->fourcc);
		if(x > dst->width || y > dst->height)
			return -2;

		w = (src->width >> 3) << 3; // fix @ 2015/12/12
		if(x + w > dst->width)
			w = dst->width - x;
		h = (src->height >> 1) << 1;
		if(y + h > dst->height)
			h = dst->height - y;
		///printf("x=%d, y=%d, w=%d, h=%d\n", x, y, w, h);
		
		#if 0
		{
			char sfourcc[5], dfourcc[5];
			memset(sfourcc, 0, 5);
			memset(dfourcc, 0, 5);
			memcpy(sfourcc, &src->fourcc, 4);
			memcpy(dfourcc, &dst->fourcc, 4);
			printf("[%s]-[%s]\n", sfourcc, dfourcc);
		}
		#endif
		
		if(src->fourcc == FCC_FIX_RGB24 ||
		   src->fourcc == FCC_FIX_BGR24 ||
		   src->fourcc == FCC_FIX_RGBA  ||
		   src->fourcc == FCC_FIX_BGRA)
		{
			if(dst->fourcc == OOK_FOURCC('I', '4', '2', '0') || 
			   dst->fourcc == OOK_FOURCC('Y', 'V', '1', '2') ||
			   dst->fourcc == OOK_FOURCC('Y', 'U', 'Y', '2') ||
			   dst->fourcc == OOK_FOURCC('Y', 'U', 'Y', 'V') ||
			   dst->fourcc == OOK_FOURCC('U', 'Y', 'V', 'Y') ||
			   dst->fourcc == OOK_FOURCC('R', 'V', '2', '4'))
			{
				__av_operator * opera = (__av_operator *)ptr;
				if(!opera)
					return -3;

				srcfourcc = OOK_FOURCC('R', 'V', '2', '4');
				
				if(opera->rgb_pic)
				{
					if(opera->rgb_pic->height < h)
					{
						release_av_picture(opera->rgb_pic);
						free(opera->rgb_pic);
						opera->rgb_pic = NULL;						
					}
				}
				if(!opera->rgb_pic)
					opera->rgb_pic = create_av_picture(w, h, srcfourcc);
				if(!opera->rgb_pic)
					return -4;

				xc = (int)srcfourcc != dst->fourcc ? 1 : 0;
				if(xc > 0)
				{
					if(opera->yuv_pic)
					{
						if(opera->yuv_pic->height < h)
						{
							release_av_picture(opera->yuv_pic);
							free(opera->yuv_pic);
							opera->yuv_pic = NULL;	
							
							if(opera->scale1)
							{
								video_scale_free(opera->scale1);
								opera->scale1 = NULL;
							}
							if(opera->scale2)
							{
								video_scale_free(opera->scale2);
								opera->scale2 = NULL;
							}												
						}
					}					
					if(!opera->yuv_pic)
					{
						opera->yuv_pic = create_av_picture(w, h, dst->fourcc);
					#if 0
						char sfourcc[5];
						sfourcc[4] = 0;
						memcpy(sfourcc, &dst->fourcc, 4);						
						printf("create av_picture %dX%d [%s]\n", w, h, sfourcc);
					#endif
					}
					if(!opera->yuv_pic)
						return -4;
					
					if(!opera->scale1)					
						opera->scale1 = video_scale_new(dst->fourcc, w, h,   srcfourcc, w, h, -1);
					if(!opera->scale2)
						opera->scale2 = video_scale_new(srcfourcc,   w, h, dst->fourcc, w, h, -1);						
					if(!opera->scale1 || !opera->scale2)
						return -5;
				}

				// copy the dst to yuv_pic
				if(dst->fourcc == OOK_FOURCC('R', 'V', '2', '4'))
				{
					x *= 3;
					w *= 3;
					y1 = dst->data[0] + dst->stride[0] * y;
					y2 = opera->rgb_pic->data[0];
					for(j = 0; j < h; j++)
					{
						memcpy(y2, y1 + x, w);
						y1 += dst->stride[0];
						y2 += opera->rgb_pic->stride[0];
					}
					w /= 3; // do not forget recover w					
				}	
				else if(dst->fourcc == OOK_FOURCC('I', '4', '2', '0') || 
				        dst->fourcc == OOK_FOURCC('Y', 'V', '1', '2'))
				{
					xu = x >> 1;
					wu = w >> 1;
					y1 = dst->data[0] + dst->stride[0] * y;
					if(dst->fourcc == OOK_FOURCC('Y', 'V', '1', '2'))
					{
						v1 = dst->data[1] + dst->stride[1] * (y >> 1);
						u1 = dst->data[2] + dst->stride[2] * (y >> 1);						
					}
					else
					{
						u1 = dst->data[1] + dst->stride[1] * (y >> 1);
						v1 = dst->data[2] + dst->stride[2] * (y >> 1);
					}
					y2 = opera->yuv_pic->data[0];
					u2 = opera->yuv_pic->data[1];
					v2 = opera->yuv_pic->data[2];
					for(j = 0; j < h; j++)
					{
						memcpy(y2, y1 + x, w );
						///memset(y2, 0xff, w);
						y1 += dst->stride[0];
						y2 += opera->yuv_pic->stride[0];
						if((j % 2) == 0)
						{
							memcpy(u2, u1 + xu, wu);
							memcpy(v2, v1 + xu, wu);
							u1 += dst->stride[1];
							v1 += dst->stride[2];
							u2 += opera->yuv_pic->stride[1];
							v2 += opera->yuv_pic->stride[2];
						}
					}
				}
				else
				{
					x <<= 1;
					w <<= 1;
					y1 = dst->data[0] + dst->stride[0] * y;
					y2 = opera->yuv_pic->data[0];
					for(j = 0; j < h; j++)
					{
						memcpy(y2, y1 + x, w);
						y1 += dst->stride[0];
						y2 += opera->yuv_pic->stride[0];
					}
					w >>= 1; // do not forget recover w
				}

			#if 1
				// change the yuv_pic to rgb_pic
				if(xc > 0 && opera->scale1)
					video_scale_call(opera->scale1, opera->yuv_pic, opera->rgb_pic);

			  #if 1
				// mask RGB
				if(src->fourcc == FCC_FIX_RGB24)
				{
					for(j = 0; j < h; j++)
					{
						y1 = opera->rgb_pic->data[0] + opera->rgb_pic->stride[0] * j;
						y2 = src->data[0] + src->stride[0] * j;						
						for(i = 0; i < w; i++)
						{
							if(param == 0)
							{
								*(y1 + 0) = *(y2 + 0);
								*(y1 + 1) = *(y2 + 1);
								*(y1 + 2) = *(y2 + 2);
							}
							else
							{
								m = ((unsigned int)*y2 | ((unsigned int)*(y2 + 1) << 8) | ((unsigned int)*(y2 + 2) << 16));
								if((m & bgmask) == (bgcolor & bgmask)) // transparant
								{
									*(y1 + 0) = *(y1 + 0);
									*(y1 + 1) = *(y1 + 1);
									*(y1 + 2) = *(y1 + 2);
								}
								else
								{
									*(y1 + 0) = *(y2 + 0);
									*(y1 + 1) = *(y2 + 1);
									*(y1 + 2) = *(y2 + 2);
								}
							}
							y1 += 3;
							y2 += 3;
						}
					}
				}
				else if(src->fourcc == FCC_FIX_BGR24)
				{
					for(j = 0; j < h; j++)
					{
						y1 = opera->rgb_pic->data[0] + opera->rgb_pic->stride[0] * j;
						y2 = src->data[0] + src->stride[0] * j;						
						for(i = 0; i < w; i++)
						{
							if(param == 0)
							{
								*(y1 + 0) = *(y2 + 2);
								*(y1 + 1) = *(y2 + 1);
								*(y1 + 2) = *(y2 + 0);
							}
							else
							{
								m = ((unsigned int)*y2 | ((unsigned int)*(y2 + 1) << 8) | ((unsigned int)*(y2 + 2) << 16));
								if((m & bgmask) == (bgcolor & bgmask)) // transparant
								{
									///*(y1 + 0) = *(y1 + 0);
									///*(y1 + 1) = *(y1 + 1);
									///*(y1 + 2) = *(y1 + 2);
								}
								else
								{
									*(y1 + 0) = *(y2 + 2);
									*(y1 + 1) = *(y2 + 1);
									*(y1 + 2) = *(y2 + 0);
								}
							}
							y1 += 3;
							y2 += 3;
						}
					}					
				}
				else if(src->fourcc == FCC_FIX_RGBA)
				{
					///float a, b;
					unsigned int A, B, C;
					for(j = 0; j < h; j++)
					{
						y1 = opera->rgb_pic->data[0] + opera->rgb_pic->stride[0] * j;
						y2 = src->data[0] + src->stride[0] * j;
						for(i = 0; i < w; i++)
						{
							if(param == 0)
							{
								*(y1 + 0) = *(y2 + 0);
								*(y1 + 1) = *(y2 + 1);
								*(y1 + 2) = *(y2 + 2);
							}
							else
							{
							#if 1
								A = *(y2 + 3);
								B = 255 - A;
								C = (unsigned int)*(y1 + 0) * B + (unsigned int)*(y2 + 0) * A;
								*(y1 + 0) = C / 255; /// >> 8;
								C = (unsigned int)*(y1 + 1) * B + (unsigned int)*(y2 + 1) * A;
								*(y1 + 1) = C / 255; /// >> 8;
								C = (unsigned int)*(y1 + 2) * B + (unsigned int)*(y2 + 2) * A;
								*(y1 + 2) = C / 255; /// >> 8;															
							#else
								// time consumed
								a = (float)*(y2 + 3) / 255;
								b = (float)1.0 - a; 
								*(y1 + 0) = (unsigned char)((float)*(y1 + 0) * b + (float)*(y2 + 0) * a);
								*(y1 + 1) = (unsigned char)((float)*(y1 + 1) * b + (float)*(y2 + 1) * a);
								*(y1 + 2) = (unsigned char)((float)*(y1 + 2) * b + (float)*(y2 + 2) * a);
							#endif
							}
							y1 += 3;
							y2 += 4;
						}
					}
				}
				else if(src->fourcc == FCC_FIX_BGRA)
				{
					unsigned int A, B, C;
					for(j = 0; j < h; j++)
					{
						y1 = opera->rgb_pic->data[0] + opera->rgb_pic->stride[0] * j;
						y2 = src->data[0] + src->stride[0] * j;
						for(i = 0; i < w; i++)
						{
							if(param == 0)
							{
								*(y1 + 0) = *(y2 + 2);
								*(y1 + 1) = *(y2 + 1);
								*(y1 + 2) = *(y2 + 0);
							}
							else
							{
								A = *(y2 + 3);
								B = 255 - A;
								C = (unsigned int)*(y1 + 0) * B + (unsigned int)*(y2 + 2) * A;
								*(y1 + 0) = C / 255;
								C = (unsigned int)*(y1 + 1) * B + (unsigned int)*(y2 + 1) * A;
								*(y1 + 1) = C / 255;
								C = (unsigned int)*(y1 + 2) * B + (unsigned int)*(y2 + 0) * A;
								*(y1 + 2) = C / 255;															
							}
							y1 += 3;
							y2 += 4;
						}
					}					
				}
			  #endif
			  
				// change the rgb_pic to yuv_pic
				if(xc > 0 && opera->scale2)
					video_scale_call(opera->scale2, opera->rgb_pic, opera->yuv_pic);
			#endif

				// copy the yuv_pic to dst
				if(dst->fourcc == OOK_FOURCC('R', 'V', '2', '4'))
				{
					w *= 3;
					y1 = dst->data[0] + dst->stride[0] * y;
					y2 = opera->rgb_pic->data[0];
					for(j = 0; j < h; j++)
					{
						memcpy(y1 + x, y2, w);
						y1 += dst->stride[0];
						y2 += opera->rgb_pic->stride[0];
					}
				}
				else if(dst->fourcc == OOK_FOURCC('I', '4', '2', '0') ||
				        dst->fourcc == OOK_FOURCC('Y', 'V', '1', '2'))
				{
					int dy = 0;
				#if 0 // aborted @ 2015/12/12
					if(dst->width > w && w > 4) // modify @ 2014/11/13
					{
						dy = 4;
					}
				#endif
					int du = dy >> 1;
					y1 = dst->data[0] + dst->stride[0] * y;
					if(dst->fourcc == OOK_FOURCC('Y', 'V', '1', '2'))
					{
						v1 = dst->data[1] + dst->stride[1] * (y >> 1);
						u1 = dst->data[2] + dst->stride[2] * (y >> 1);						
					}
					else
					{
						u1 = dst->data[1] + dst->stride[1] * (y >> 1);
						v1 = dst->data[2] + dst->stride[2] * (y >> 1);
					}
					y2 = opera->yuv_pic->data[0];
					u2 = opera->yuv_pic->data[1];
					v2 = opera->yuv_pic->data[2];
					for(j = 0; j < h; j++)
					{
						memcpy(y1 + x,  y2, w - dy);
						y1 += dst->stride[0];
						y2 += opera->yuv_pic->stride[0];
						if((j % 2) == 0)
						{
							memcpy(u1 + xu, u2, wu - du);
							memcpy(v1 + xu, v2, wu - du);
							u1 += dst->stride[1];
							v1 += dst->stride[2];
							u2 += opera->yuv_pic->stride[1];
							v2 += opera->yuv_pic->stride[2];
						}
					}
				}
				else
				{
					w <<= 1;
					y1 = dst->data[0] + dst->stride[0] * y;
					y2 = opera->yuv_pic->data[0];
					for(j = 0; j < h; j++)
					{
						memcpy(y1 + x, y2, w);
						y1 += dst->stride[0];
						y2 += opera->yuv_pic->stride[0];
					}
				}				
				return 0;
			}
			return -1;
		}

		if(dst->fourcc == OOK_FOURCC('I', '4', '2', '0'))
		{
			if(src->fourcc == OOK_FOURCC('I', '4', '2', '0') ||
			   src->fourcc == OOK_FOURCC('Y', 'V', '1', '2'))
			{
				y1 = dst->data[0] + dst->stride[0] * y;
				u1 = dst->data[1] + dst->stride[1] * (y >> 1);
				v1 = dst->data[2] + dst->stride[2] * (y >> 1);
				y2 = src->data[0];
				if(src->fourcc == OOK_FOURCC('I', '4', '2', '0'))
				{
					u2 = src->data[1];
					v2 = src->data[2];
				}
				else
				{
					u2 = src->data[2];
					v2 = src->data[1];					
				}
				xu = x >> 1;
				wu = w >> 1;
				if(param == 0)
				{
					for(j = 0; j < h; j++)
					{
						for(i = 0, k = 0; i < w; i += 2)
						{
							m = ((unsigned int)y2[i]) | ((unsigned int)u2[k] << 8) | ((unsigned int)v2[k] << 16);
							if((j % 2) == 0)
							{
								// u, v
								if((m & bgmask) == (bgcolor & bgmask))
								{
									u1[xu + k] = ((unsigned int)u1[xu + k] + (unsigned int)u2[k]) / 2;
									v1[xu + k] = ((unsigned int)v1[xu + k] + (unsigned int)v2[k]) / 2;
								}
								else
								{
									u1[xu + k] = u2[k];
									v1[xu + k] = v2[k];
								}
								k++;
							}								
							if((m & bgmask) == (bgcolor & bgmask))
								continue;
							// y
							y1[x + i]     = y2[i];
							y1[x + i + 1] = y2[i + 1];
						}
						y1 += dst->stride[0];
						y2 += src->stride[0];
						if((j % 2) == 1)
						{
							u1 += dst->stride[1];
							u2 += src->stride[1];
							v1 += dst->stride[2];
							v2 += src->stride[2];
						}
					}						
				}
				else
				{
					for(j = 0; j < h; j++)
					{
						for(i = 0, k = 0; i < w; i += 2)
						{
							m = ((unsigned int)y2[i]) | ((unsigned int)u2[k] << 8) | ((unsigned int)v2[k] << 16);
							if((j % 2) == 0)
							{
								// u, v
								u1[xu + k] = ((unsigned int)u1[xu + k] + (unsigned int)u2[k]) / 2;
								v1[xu + k] = ((unsigned int)v1[xu + k] + (unsigned int)v2[k]) / 2;
								k++;
							}
							if((m & bgmask) == (bgcolor & bgmask))
								continue;
							// y
							y1[x + i]     = ((unsigned int)y1[x + i]     + (unsigned int)y2[i]) / 2;
							y1[x + i + 1] = ((unsigned int)y1[x + i + 1] + (unsigned int)y2[i + 1]) / 2;
						}
						y1 += dst->stride[0];
						y2 += src->stride[0];
						if((j % 2) == 1)
						{
							u1 += dst->stride[1];
							u2 += src->stride[1];
							v1 += dst->stride[2];
							v2 += src->stride[2];
						}
					}
				}
				return 0;
			}
			else if(src->fourcc == OOK_FOURCC('Y', 'U', 'Y', 'V') ||
			        src->fourcc == OOK_FOURCC('Y', 'U', 'Y', '2')) // FIX @ 2016/05/16
			{
				y1 = dst->data[0] + dst->stride[0] * y;
				u1 = dst->data[1] + dst->stride[1] * (y >> 1);
				v1 = dst->data[2] + dst->stride[2] * (y >> 1);
				y2 = src->data[0];
					
				xu = x >> 1;
				wu = w >> 1;
				if(param == 0)
				{
					for(j = 0; j < h; j++)
					{
						for(i = 0, k = 0; i < w; i += 2)
						{
							l = i << 1;
							m = ((unsigned int)y2[l]) | ((unsigned int)y2[l + 1] << 8) | ((unsigned int)y2[l + 3] << 16);
							if((j % 2) == 0)
							{
								// u, v
								if((m & bgmask) == (bgcolor & bgmask))
								{
									u1[xu + k] = ((unsigned int)u1[xu + k] + (unsigned int)y2[l + 1]) / 2;
									v1[xu + k] = ((unsigned int)v1[xu + k] + (unsigned int)y2[l + 3]) / 2;
								}
								else
								{
									u1[xu + k] = y2[l + 1];
									v1[xu + k] = y2[l + 3];
								}
								k++;
							}
							if((m & bgmask) == (bgcolor & bgmask))
								continue;
							// y
							y1[x + i]     = y2[l];
							y1[x + i + 1] = y2[l + 2];
						}
						y1 += dst->stride[0];
						if((j % 2) == 1)
						{
							u1 += dst->stride[1];
							v1 += dst->stride[2];
						}
						y2 += src->stride[0];
					}						
				}
				else
				{
					for(j = 0; j < h; j++)
					{
						for(i = 0, k = 0; i < w; i += 2)
						{
							l = i << 1;
							m = ((unsigned int)y2[l]) | ((unsigned int)y2[l + 1] << 8) | ((unsigned int)y2[l + 3] << 16);
							if((j % 2) == 0)
							{
								// u, v
								u1[xu + k] = ((unsigned int)u1[xu + k] + (unsigned int)y2[l + 1]) / 2;
								v1[xu + k] = ((unsigned int)v1[xu + k] + (unsigned int)y2[l + 3]) / 2;
								k++;
							}
							if((m & bgmask) == (bgcolor & bgmask))
								continue;
							// y
							y1[x + i]     = ((unsigned int)y1[x + i]     + (unsigned int)y2[l]) / 2;
							y1[x + i + 1] = ((unsigned int)y1[x + i + 1] + (unsigned int)y2[l + 2]) / 2;
						}
						y1 += dst->stride[0];
						if((j % 2) == 1)
						{
							u1 += dst->stride[1];
							v1 += dst->stride[2];
						}
						y2 += src->stride[0];
					}
				}
				return 0;
			}
			return -1;
		}

		if(dst->fourcc == OOK_FOURCC('Y', 'U', 'Y', '2') ||
		   dst->fourcc == OOK_FOURCC('Y', 'U', 'Y', 'V') ||
		   dst->fourcc == OOK_FOURCC('U', 'Y', 'V', 'Y'))
		{
			if(src->fourcc == OOK_FOURCC('Y', 'U', 'Y', '2') ||
			   src->fourcc == OOK_FOURCC('Y', 'U', 'Y', 'V') ||
			   src->fourcc == OOK_FOURCC('U', 'Y', 'V', 'Y'))
			{
				y1 = dst->data[0] + dst->stride[0] * y;
				y2 = src->data[0];
				x <<= 1;
				w <<= 1;
				for(j = 0; j < h; j++)
				{
					if(param == 0)
					{
						for(i = 0, k = 0; i < w; i += 2)
						{
							m = ((unsigned int)y2[i]) | ((unsigned int)y2[i + 1] << 8);
							if((m & bgmask) == (bgcolor & bgmask))
								continue;
							y1[x + i]     = y2[i];
							y1[x + i + 1] = y2[i + 1];
						}																
						y1 += dst->stride[0];
						y2 += src->stride[0];
					}
					else
					{
						for(i = 0, k = 0; i < w; i += 2)
						{
							m = ((unsigned int)y2[i]) | ((unsigned int)y2[i + 1] << 8);
							if((m & bgmask) == (bgcolor & bgmask))
								continue;
							y1[x + i]     = ((unsigned int)y1[x + i]     + (unsigned int)y2[i]) / 2;
							y1[x + i + 1] = ((unsigned int)y1[x + i + 1] + (unsigned int)y2[i + 1]) / 2;
						}							
						y1 += dst->stride[0];
						y2 += src->stride[0];
					}
				}
				return 0;
			}
			return -1;
		}
		return -1;
	}
	
	if(opt == AVPICTURE_OPERATION_OPTCROP)
	{
		w = dst->width;
		h = dst->height;
		///printf("[%d,%d,%d,%d,%d,%d]\n", x, y, w, h, src->width, src->height);
		if(x + w > src->width)
			return -2;
		if(y + h > src->height)
			return -2;
		if(dst->fourcc != src->fourcc)
			return -3;
		if(dst->fourcc == OOK_FOURCC('I', '4', '2', '0') ||
		   dst->fourcc == OOK_FOURCC('Y', 'V', '1', '2'))
		{
			y1 = src->data[0] + src->stride[0] * y;
			u1 = src->data[1] + src->stride[1] * (y >> 1);
			v1 = src->data[2] + src->stride[2] * (y >> 1);
			y2 = dst->data[0];
			u2 = dst->data[1];
			v2 = dst->data[2];
			xu = x >> 1;
			wu = w >> 1;
			for(j = 0; j < h; j++)
			{
				// y
				memcpy(y2, y1 + x, w);
				y1 += src->stride[0];
				y2 += dst->stride[0];
				
				if((j % 2) == 0)
				{
					// u, v
					memcpy(u2, u1 + xu, wu);
					memcpy(v2, v1 + xu, wu);
					u1 += src->stride[1];
					u2 += dst->stride[1];
					v1 += src->stride[2];
					v2 += dst->stride[2];
				}
			}
			return 0;
		}
		else if(dst->fourcc == OOK_FOURCC('Y', 'U', 'Y', '2') || 
		        dst->fourcc == OOK_FOURCC('U', 'Y', 'V', 'Y') ||
		        dst->fourcc == OOK_FOURCC('H', 'D', 'Y', 'C'))
		{
			y1 = src->data[0] + src->stride[0] * y;
			y2 = dst->data[0];
			x <<= 1;
			w <<= 1;			
			for(j = 0; j < h; j++)
			{			
				memcpy(y2, y1 + x, w);
				y1 += src->stride[0];
				y2 += dst->stride[0];
			}
			return 0;
		}
	}
	return -1;
}

#endif

/*
	int sar_num, sar_den;
	caculate_aspect_ratio( 528,  576, 1, -1, -1, -1, -1, &sar_num, &sar_den);
	printf("[%d/%d]\n", sar_num, sar_den); // 16/11
	
	caculate_aspect_ratio( 720,  576, 1, -1, -1, -1, -1, &sar_num, &sar_den);
	printf("[%d/%d]\n", sar_num, sar_den); // 16/15
	
	caculate_aspect_ratio(1280,  720, 2, -1, -1, -1, -1, &sar_num, &sar_den);
	printf("[%d/%d]\n", sar_num, sar_den); // 11/11
	
	caculate_aspect_ratio(1920, 1080, 2, -1, -1, -1, -1, &sar_num, &sar_den);
	printf("[%d/%d]\n", sar_num, sar_den); // 11/11
 */				
int caculate_aspect_ratio(int frame_width,
					      int frame_height,
						  int aspect_ratio, 
						  int aspect_num, 
						  int aspect_den,
						  int display_width, 
						  int display_height,
						  int * sar_num,
						  int * sar_den)
{
	///printf("caculate_aspect_ratio::%d/%d %d/%d/%d %d/%d\n", frame_width, frame_height, aspect_ratio, aspect_num, aspect_den, display_width, display_height);
	float f = (float)frame_width / (float)frame_height;
	float r = (float)0.0;
	switch(aspect_ratio)
	{
	case 1:
		r = (float)4 / (f * 3);
		break;
	case 2:
		r = (float)16 / (f * 9);
		break;
	default:
		if(aspect_num > 0 && aspect_den > 0)
		{
			*sar_num = aspect_num;
			*sar_den = aspect_den;
			return (*sar_num == *sar_den ? 0 : 1);
		}
		if(aspect_ratio > 0x10000)
		{
			aspect_num = aspect_ratio >> 16;
			aspect_den = aspect_ratio & 0x7ff;
			r = (float)aspect_num / (f * aspect_den);
		}
		else if(display_width > 0 && display_height > 0)
		{
			r = (float)display_width / display_height / f;
		}
		break;
	}
	if(r > 0.1)
	{
		///printf("caculate_aspect_ratio::r=%f\n", r);
		if(sar_num && sar_den)
		{
			int i, max_d = 9999, d[22];
			for(i = 0; i < 22; i++)
				d[i] = -1;
			for(i = 11; i < 23; i++)
			{
				*sar_den = i;
				*sar_num = (int)(r * (*sar_den) + (float)0.5);
				f = (float)(*sar_num) / *sar_den;
				d[i - 11] = (int)((f > r ? f - r : r - f) * 1000);
				///printf("%d/%d/%f/%f %d\n", *sar_den, *sar_num, f, r, d[i - 11]);
				if(d[i - 11] == 0)
					break;
			}
			for(i = 11; i < 23; i++)
			{
				if(d[i - 11] < 0)
					break;
				if(d[i - 11] < max_d)
				{
					max_d = d[i - 11];
					*sar_den = i;
					///printf("d[%d]=%d\n", i, max_d);
				}
			}
			*sar_num = (int)(r * (*sar_den) + (float)0.5);
			///printf("sel[%d/%d]\n", *sar_num, *sar_den);
			return (*sar_num == *sar_den ? 0 : 1);
		}
	}
	return -1;
}
