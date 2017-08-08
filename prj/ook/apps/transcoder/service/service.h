#ifndef __OOK_TASK_SERVICE_H__
#define __OOK_TASK_SERVICE_H__

#include <stdlib.h>
#include <string>

#ifdef WIN32
#define SERVICE_STDCALL
///#define SERVICE_STDCALL __stdcall
#else
#define SERVICE_STDCALL
#endif

/*
	service callback interface, add @ 2017/03/19
 */
class ifservice_callback
{
public:
	enum service_cb
	{
		e_service_cb_unknow = -1,
		e_service_cb_null,
		
		e_service_cb_dura,
		e_service_cb_picxchg, // pic will be changed within so
		
		e_service_cb_samplerate,
		e_service_cb_channels,
		e_service_cb_width,
		e_service_cb_height,
		
		e_service_cb_pause,
		e_service_cb_stampcacu,
		e_service_cb_playratio,
		
		e_service_cb_pic = 0x80,
		e_service_cb_pcm,
		
		e_service_cb_chr = 0x100,
		e_service_cb_mediachr
	};
	
	virtual ~ifservice_callback() {}

	virtual int onservice_callback(int opt,
								   int chr,
								   int sub,
								   int wparam,
								   int lparam,
								   void * ptr) = 0;	
};

/*
	dll/so interface
 */
typedef int	(SERVICE_STDCALL * __service_libversion)();
typedef int	(SERVICE_STDCALL * __service_libchar)   ();
typedef int	(SERVICE_STDCALL * __service_libtrace)  (int);
typedef int	(SERVICE_STDCALL * __service_libinit)   (const char * xml_c, void * task, void ** ctx);
typedef int	(SERVICE_STDCALL * __service_liboper)   (int, int, int, void * ptr, void * ctx);
typedef int	(SERVICE_STDCALL * __service_libstop)   (void * ctx);

/*
	macro
 */

//
// data_type
//
#define SERVICE_DATATYPE_DEP		 	0x00 // misc type
#define SERVICE_DATATYPE_FRM   		 	0x01 // frame
#define SERVICE_DATATYPE_PIC   		 	0x02 // picture
#define SERVICE_DATATYPE_PCM   		 	0x03 // PCM data
#define SERVICE_DATATYPE_PKG   		 	0x04 // package
#define SERVICE_DATATYPE_CHR			0x08 // so private

//
// opt
//
#define SERVICE_OPT_ENCODEINFO 		 	0x01
#define SERVICE_OPT_DISPLAY			 	0x02
#define SERVICE_OPT_TASKSTATUS		 	0x03
#define SERVICE_OPT_FILEOPER		 	0x04
#define SERVICE_OPT_TASKSTATUSREQ		0x05

#define SERVICE_OPT_ENCSTARTING			0x10
#define SERVICE_OPT_ENCINPTOVER			0x11
#define SERVICE_OPT_ENCCOMPLETED		0x12

#define SERVICE_OPT_CMDUPDATE	 		0x80

#define SERVICE_OPT_PREVSTOP		 	0xF0
#define SERVICE_OPT_SESSION		 		0xF1
#define SERVICE_OPT_TASKCHAR 			0xF2
#define SERVICE_OPT_SESSNAME	 		0xF3
#define SERVICE_OPT_SRCLABEL 			0xF4
#define SERVICE_OPT_SPACERTYPE			0xF5
#define SERVICE_OPT_TASKBYCMD	 		0xF8

//
// pos
//
#define SERVICE_POSITION_NULL     	 	0x00

#define SERVICE_POSITION_VIDEO_BFDEC 	0x01
#define SERVICE_POSITION_VIDEO_AFDEC 	0x02
#define SERVICE_POSITION_VIDEO_BFENC 	0x04
#define SERVICE_POSITION_VIDEO_AFENC 	0x08

#define SERVICE_POSITION_AUDIO_BFDEC 	0x10
#define SERVICE_POSITION_AUDIO_AFDEC 	0x20
#define SERVICE_POSITION_AUDIO_BFENC 	0x40
#define SERVICE_POSITION_AUDIO_AFENC 	0x80

#define SERVICE_POSITION_MEDIA_AFPKG   0x100

/*
	struct
 */

/*
	task_info_detail_s
 */
struct task_info_detail_s
{	
	std::string TTID;
	std::string cur_path;
	std::string extinfo;
};

/*
	task_info_s
 */
struct task_info_s
{
	std::string src_label;
	std::string src_addr;
	std::string dst_label;
	std::string dst_addr;
	
	void * extra;
	ifservice_callback * ifcb; // add @ 2017/03/19
};

/*
	task_service_s
 */
struct task_service_s
{
	int chr;
	int pos;
	
	__service_libinit init;
	__service_liboper oper;
	__service_libstop stop;
	
	void * ctx;
	void * pri; // used by transcoder, do NOT modify this within so, add @ 2017/01/23 
};

inline void init_task_service_s(task_service_s * s)
{
	s->chr  = -1;   
	s->pos  = -1;
	s->init = NULL;
	s->oper = NULL;
	s->stop = NULL;
	s->ctx  = NULL;
	s->pri  = NULL;	
}

struct task_service_file_s
{
	int opt;
	int par;
	void * arg;
	std::string f_path;
};

inline void init_task_service_file_s(task_service_file_s * s)
{
	s->opt = -1;
	s->par = -1;
	s->arg = NULL;
}

//
// task service return struct
//

struct av_picture;

struct task_service_return_s
{
	int oper;
	int stat;
	int code;
	
	int x[4];
	int y[4];
	
	av_picture * pic;
	void       * ptr;
};

inline void init_task_service_return_s(task_service_return_s * s)
{
	s->oper = 0;
	s->stat = -1;
	s->code = 0;
	
	s->x[0] = s->x[1] = s->x[2] = s->x[3] = 0;
	s->y[0] = s->y[1] = s->y[2] = s->y[3] = 0;
	
	s->pic  = NULL;
	s->ptr  = NULL;
}

#endif
