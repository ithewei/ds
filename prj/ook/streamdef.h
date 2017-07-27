#ifndef __OOK_STREAMDEF_H__
#define __OOK_STREAMDEF_H__

enum media_stream_type
{
	MediaTypeUnknown,
	MediaTypeAudio,
	MediaTypeVideo,
	MediaTypeMetaData,
	MediaTypeCompound,
	MediaTypeText
};

enum media_video_frametype
{
	e_videoframe_Iframe = 0x01,
	e_videoframe_Bframe = 0x02,
	e_videoframe_Hframe = 0x08
};

enum media_video_fieldmode
{
	e_videofieldmode_unknow      = 0,
	e_videofieldmode_progressive = 0x01,
	e_videofieldmode_tff         = 0x02,
	e_videofieldmode_bff         = 0x04
};

enum media_frame_miscflag
{
	e_framemisc_mulaudiotrks = 0x00010000,
	e_framemisc_chkintegrity = 0x00020000,
	e_framemisc_encreseted   = 0x00040000,
	e_framemisc_spacer		 = 0x00080000,

	e_framemisc_ispcrpid     = 0x00100000,
	e_framemisc_medchanged   = 0x00200000,
	e_framemisc_clearleft    = 0x00400000,
	e_framemisc_newsegment   = 0x00800000,
	
	e_framemisc_tspackage    = 0x01000000,
	e_framemisc_backupstream = 0x02000000,
	
	e_framemisc_vipframe     = 0x10000000,
	e_framemisc_keyframe     = 0x20000000,
	e_framemisc_predef       = 0x40000000,
	e_framemisc_startf       = 0x80000000
};

enum media_picture_flag
{
	e_picture_flag_interlaced  = 0x00010000,
	e_picture_flag_updatestamp = 0x00020000,
	e_picture_flag_reverse     = 0x00040000,
	e_picture_flag_spacer	   = 0x00080000,
	e_picture_flag_indxaslabel = 0x00100000,
	e_picture_flag_aborted     = 0x00200000,
	e_picture_flag_picupdated  = 0x00400000
};

enum media_pcmbuff_flag
{
	e_pcmbuff_flag_spacer	  = 0x00080000,
	e_pcmbuff_flag_novideo    = 0x00100000
};

#define AUDIO_FORMAT_PCMBE 	0x01
#define AUDIO_FORMAT_PCM24	0x02

// for multi audio tracks reader
#define MP4_MAXTRACKS 16
#define MKV_MAXTRACKS 32

#define STREAM_TYPE_AUDIO_PCM		0x00
#define STREAM_TYPE_AUDIO_PCM_S16LE	STREAM_TYPE_AUDIO_PCM

// 0x01 ~ 0x7F Table 2.29 of ISO/IEC 13818-1
#define STREAM_TYPE_VIDEO_MPEG1     0x01
#define STREAM_TYPE_VIDEO_MPEG2     0x02
#define STREAM_TYPE_AUDIO_MPEG1     0x03
#define STREAM_TYPE_AUDIO_MPEG2     0x04

#define STREAM_TYPE_PRIVATE_SECTION 0x05
#define STREAM_TYPE_PRIVATE_DATA    0x06

#define STREAM_TYPE_AUDIO_AAC       0x0f
#define STREAM_TYPE_VIDEO_MPEG4     0x10
#define STREAM_TYPE_AUDIO_AAC_LATM  0x11

#define STREAM_TYPE_VIDEO_H264      0x1b
#define STREAM_TYPE_VIDEO_HEVC      0x24
#define STREAM_TYPE_VIDEO_H265      STREAM_TYPE_VIDEO_HEVC
#define STREAM_TYPE_VIDEO_AVS       0x42

#define STREAM_TYPE_AUDIO_AC3       0x81
#define STREAM_TYPE_AUDIO_DTS_HDMV  0x82
#define STREAM_TYPE_AUDIO_LPCM  	0x83
#define STREAM_TYPE_AUDIO_SDDS  	0x84
#define STREAM_TYPE_AUDIO_DTS_HD  	0x86
#define STREAM_TYPE_AUDIO_DTS       0x8a
#define STREAM_TYPE_AUDIO_EAC3		0x8b

// 0x86 ~ 0xBF Reserved

// 0xC0 ~ 0xFF User Private
#define STREAM_TYPE_USERPRIVATE	    0xC0

#define STREAM_TYPE_VIDEO_H263		0xC0
#define STREAM_TYPE_VIDEO_WMV		0xC1
#define STREAM_TYPE_VIDEO_VC1		0xC2
#define STREAM_TYPE_VIDEO_FLV1		0xC3
#define STREAM_TYPE_VIDEO_THEORA	0xC4
#define STREAM_TYPE_VIDEO_HIK 	 	0xC5 // HaiKang
#define STREAM_TYPE_VIDEO_MJPEG	 	0xC6
#define STREAM_TYPE_VIDEO_ProRes 	0xC7
#define STREAM_TYPE_VIDEO_TGA	 	0xC8

#define STREAM_TYPE_AUDIO_PCMU		0xE0
#define STREAM_TYPE_AUDIO_PCMA		0xE1
#define STREAM_TYPE_AUDIO_AMR		0xE2
#define STREAM_TYPE_AUDIO_AMWB		0xE3
#define STREAM_TYPE_AUDIO_WMA		0xE4
#define STREAM_TYPE_AUDIO_MP3		0xE5
#define STREAM_TYPE_AUDIO_VORBIS	0xE6

#define STREAM_TYPE_SUBTITLE		0xFE
#define STREAM_TYPE_EXUNKNOW		0xFF

#endif
