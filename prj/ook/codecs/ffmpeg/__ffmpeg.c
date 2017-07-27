#include <ook/codecs/ffmpeg/__ffmpeg.h>

int av_log_level_ = AV_LOG_QUIET;

#if defined(FFMPEG_3_1)

  #ifdef WIN32
	#pragma message("#define FFMPEG_3_1")
	
	#pragma comment(lib, "libavcodec_3.1.7.a")
	#pragma comment(lib, "libavfilter_3.1.7.a")
	#pragma comment(lib, "libavformat_3.1.7.a")
	#pragma comment(lib, "libavutil_3.1.7.a")
	#pragma comment(lib, "libswscale_3.1.7.a")
  #else
	#warning "#define FFMPEG_3_1"
  #endif
  
	int avcodec_init() { return 0; }
	
#elif defined(FFMPEG_V_56_60)

  #ifdef WIN32
	#pragma message("#define FFMPEG_V_56_60")
	
	#pragma comment(lib, "libavcodec_56_60.a")
	#pragma comment(lib, "libavfilter_56_60.a")
	#pragma comment(lib, "libavformat_56_60.a")
	#pragma comment(lib, "libavutil_56_60.a")
	#pragma comment(lib, "libswscale_56_60.a")
  #else
	#warning "#define FFMPEG_V_56_60"
  #endif
  
	int avcodec_init() { return 0; }
	
#elif defined(FFMPEG_V_56_13)

  #ifdef WIN32
	#pragma message("#define FFMPEG_V_56_13")
	
	#pragma comment(lib, "libavcodec_56_13.a")
	#pragma comment(lib, "libavfilter_56_13.a")
	#pragma comment(lib, "libavformat_56_13.a")
	#pragma comment(lib, "libavutil_56_13.a")
	#pragma comment(lib, "libswscale_56_13.a")
  #else
	#warning "#define FFMPEG_V_56_13"
  #endif
  
	int avcodec_init() { return 0; }

#elif defined(FFMPEG_V_55_52) // used by android

  #ifdef WIN32
  #else
	#warning "#define FFMPEG_V_55_52"
  #endif
  
  int avcodec_init() { return 0; }
  	
#elif defined(FFMPEG_V_55_39)

  #ifdef WIN32
	#pragma message("#define FFMPEG_V_55_39")
	
	// ###
	///#pragma comment(lib, "libavcodec.a")
	///#pragma comment(lib, "libavutil.a")
	
	#pragma comment(lib, "libavcodec_55_39.a")
	#pragma comment(lib, "libavfilter_55_39.a")
	#pragma comment(lib, "libavformat_55_39.a")
	#pragma comment(lib, "libavutil_55_39.a")
	#pragma comment(lib, "libswscale_55_39.a")
  #else
	#warning "#define FFMPEG_V_55_39"
  #endif
  
	int avcodec_init() { return 0; }
	
#elif defined(FFMPEG_V_55_18)

  #ifdef WIN32
	#pragma message("#define FFMPEG_V_55_18")

	///#pragma comment(lib, "libavcodec.a")
	#pragma comment(lib, "libavcodec_55_18.a")
	#pragma comment(lib, "libavfilter_55_18.a")
	///#pragma comment(lib, "libavformat.a")
	///#pragma comment(lib, "libavutil.a")
	#pragma comment(lib, "libavformat_55_18.a")
	#pragma comment(lib, "libavutil_55_18.a")
	#pragma comment(lib, "libswscale_55_18.a")
	
	///#pragma comment(lib, "libswscale.a")
  #else
	#warning "#define FFMPEG_V_55_18"
  #endif
  
	int avcodec_init() { return 0; }

#elif defined(FFMPEG_V_54_59)

  #ifdef WIN32
	#pragma message("#define FFMPEG_V_54_59")
	#pragma comment(lib, "libavcodec_54_59.a")
	#pragma comment(lib, "libavfilter_54_59.a")
	#pragma comment(lib, "libavutil_54_59.a")
	#pragma comment(lib, "libswscale_54_59.a")
  #else
	#warning "#define FFMPEG_V_54_59"
  #endif
  
	int avcodec_init() { return 0; }

#elif defined(FFMPEG_V_53_7)

  #ifdef WIN32
	#pragma message("#define FFMPEG_V_53_7")
	#pragma comment(lib, "libavcodec_53_7.a")
	#pragma comment(lib, "libavutil_53_7.a")
	#pragma comment(lib, "libswscale_53_7.a")
  #else
	#warning "#define FFMPEG_V_53_7"
  #endif

#endif

#if defined(FFMPEGSRC) || defined(FFMPEGSRC_ARM) || defined(__SYMBIAN32__)
extern AVCodec h264_decoder;
#endif

static int avcodec_init_ = 0;

void init_avcodec_context_s()
{
	if(avcodec_init_ == 0)
	{
		avcodec_init_ = 1;
		
		///TRACE(3, "init codecs <<<")
		
		avcodec_init();
		av_log_set_level(av_log_level_);

	#if defined(FFMPEGSRC) || defined(FFMPEGSRC_ARM)
		AVCODEC_REGISTER(&h264_decoder);
	#elif defined(__SYMBIAN32__)
		avcodec_register(&h264_decoder);
	#elif defined(__ANDROID__)
		avcodec_register_all();
	#else
		avcodec_register_all();

		#if LIBAVCODEC_VERSION_MAJOR > 53
		  #ifndef NO_AVFILTER
			avfilter_register_all();
		  #endif
		#endif
	    #if LIBAVCODEC_VERSION_MAJOR > 55
			av_register_all();
		#endif
	#endif
		///TRACE(3, ">>>")
	}	
}

void init_avcodec_s(avcodec_s * s)
{
	s->txt       = NULL;
	s->codec     = NULL;
	s->frm       = NULL;
	s->extradata = NULL;
	s->status    = 0;
}

void release_avcodec_s(avcodec_s * s)
{
	if(s->frm)
	{
		av_frame_free(&s->frm); // modify @ 2016/01/29
	}

	// do not call this
	///if(s->codec)
	///	free(s->codec);

	if(s->txt)
	{
		if(s->status & 0x01)
		{
			///printf("fc\n");
			avcodec_close(s->txt); // do NOT call this if avcodec_open() fail
			///printf("FC\n");
		}

		av_free(s->txt);
	}

	if(s->extradata)
		free(s->extradata);

	delete s;
}

void printmask_avcodec_s(avcodec_s * s, unsigned int printmask)
{
	if(s && s->txt)
	{
		///printf("avcodec::printmask=%x\n", printmask);
		// ###
		///s->txt->printmask = printmask;
	}
}

void shutdown_avcodec_s()
{
	av_lockmgr_register(NULL);
}
