#ifndef __OOK_AVPICTUREDEF_H__
#define __OOK_AVPICTUREDEF_H__

#include <stdlib.h>
#ifdef WIN32
typedef signed __int64 int64_t;
#else
#include <stdint.h>
#endif

#define AVPICTURE_OPERATION_OPTMASK 	0x01
#define AVPICTURE_OPERATION_OPTCROP  	0x02
					 
// av_picture
#define CLONE_AVPIC_MISCMASK_WES 0x01 	// width equal stride

#define AVSTAMPRESET(stamp, stamplast, distO, distN) ( ((stamplast > stamp) && (stamplast - stamp > distN)) || ((stamp > stamplast) && (stamp - stamplast > distO)) )

typedef void * av_operator;

typedef struct pixel
{
	unsigned int fourcc;
	unsigned int bitmask[3];

} pixel_s;

typedef struct audio_desc_s
{
	int channels;
	int samplerate;
	int timescale;
	unsigned int stamp;
	unsigned int duration;
	unsigned int index;
	unsigned int bitspersample;
	
} audio_desc_s;

typedef struct picture_desc_s
{
	int w;
	int h;
	int frames;
	unsigned int ftype;
	unsigned int stamp;
	unsigned int index;
	
} picture_desc_s;

//
// PCM data
//
typedef struct av_pcmbuff
{
	int channels;
	int samplerate;
	int track;
	unsigned int stamp;
	unsigned int flag;
	
	unsigned int pcmlen;
	unsigned char * pcmbuf;

	void * arg;
	
} av_pcmbuff;

av_pcmbuff * create_av_pcmbuff(unsigned int assign_pcmbuf_len);
void reset_av_pcmbuff  (av_pcmbuff * frm, unsigned int assign_pcmlen);
void release_av_pcmbuff(av_pcmbuff * frm);
void free_av_pcmbuff   (av_pcmbuff * frm);
void clone_av_pcmbuff  (av_pcmbuff * dst, 
					    const av_pcmbuff * src,
					    int copy_pcmbuf);

//
// picture data
//
typedef struct av_picture
{
    unsigned char * data[4];
    int stride[4];
    
    int width;
    int height;
	int fourcc;
    int framerate;
    
    int selfrelease; // please free me after call
	int continuebuf; // is continue buffer
	
	unsigned int stamp;
	unsigned int flag;
    unsigned int index;

	void * arg;

    int display_width;
    int display_height;
	int track;
	
} av_picture;

void reset_av_picture    (av_picture * pic);
void release_av_picture  (av_picture * pic);
void free_av_picture     (av_picture * pic);
void release_av_picturebf(av_picture * pic);

av_picture * create_av_picture(int width,
					 		   int height,
					 		   int fourcc);

int clone_av_picture (av_picture * dst, 
					  const av_picture * src, 
					  int height,
					  int selfrelease);
int clone_av_picture2(av_picture * dst, 
					  const av_picture * src, 
					  int height,
					  int selfrelease,
					  unsigned int miscmask);
int copy_av_picture  (av_picture * dst, 
					  const av_picture * src, 
					  int height);
int black_av_picture (av_picture * pic);

void attach_av_picture(av_picture * dst, 
					   const av_picture * src);
int extract_av_picture(av_picture * dst, 
					   const av_picture * src,
					   int top_field);
int combine_av_picture(av_picture * dst,
					   const av_picture * pic_t, 
					   const av_picture * pic_b);

//
// av_frame_s
//
typedef struct av_frame_s
{
	unsigned char * bits;	/* frame buffer */
	unsigned int bitslen;	/* frame length */
	unsigned int bitspos;   /* frame start pos within buffer */
	unsigned int medtype;	/* media type such as audio or video */
	unsigned int stmtype;   /* stream type such as h264, aac etc */
	unsigned int frmtype;   /* frame type such as key frame */
	unsigned int frmmisc;	/* misc character */
	unsigned int tmscale;	/* time scale for pts and dts */
	unsigned int frmsequ;	/* frame's sequence */
	int64_t pcr;			/* frame pcr */
	int64_t pts;			/* frame pts */
	int64_t dts;			/* frame dts */
	int dur;				/* frame duration */
	int track;				/* frame track */

	void * arg;
	
	int language;			/* frame language, add @ 2017/06/17 */
	
} av_frame_s;

void reset_av_frame_s  (av_frame_s * frm);
void release_av_frame_s(av_frame_s * frm);
void copy_av_frame_s   (av_frame_s * dst, 
					    const av_frame_s * src); // only copy value do NOT malloc buffer
void clone_av_frame_s  (av_frame_s * dst, 
					    const av_frame_s * src,
					    unsigned int bitspos,
					    unsigned int extralen); // copy value and malloc buffer

//
// av_package_s
//
typedef struct av_package_s
{
	int enc_t; // e_encapsulation_type
	int pkg_t; // package type
	int med_t; // media type
	
	int64_t stamp;
	
	unsigned char * bits;
	unsigned int bitslen;
	unsigned int bitspos;
	
	void * arg;
	
} av_package_s;

void reset_av_package_s(av_package_s * pkg);

//
// av_operator
//
av_operator create_av_operator();
void release_av_operator(av_operator opera);

int operate_av_picture(int opt,
                       int param,
                       int x,
                       int y,
                       unsigned int bgcolor,
                       unsigned int bgmask,
                       av_picture * dst, 
					   const av_picture * src,
					   av_operator opera);					 		 

//
// misc
//
int caculate_aspect_ratio(int frame_width,
					      int frame_height,
						  int aspect_ratio, 
						  int aspect_num, 
						  int aspect_den,
						  int display_width, 
						  int display_height,
						  int * sar_num,
						  int * sar_den);

#endif
