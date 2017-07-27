#if defined(FFMPEG_3_1)

	// avcodec
	#include <ook/codecs/ffmpeg/src/3.1.7/libavutil/intreadwrite.h> // noly for GDB usage
	#include <ook/codecs/ffmpeg/src/3.1.7/libavcodec/avcodec.h>
  
	// avfilter
	#include <ook/codecs/ffmpeg/src/3.1.7/libavfilter/avfilter.h>
  	#include <ook/codecs/ffmpeg/src/3.1.7/libavfilter/avfiltergraph.h>
	#include <ook/codecs/ffmpeg/src/3.1.7/libavfilter/buffersink.h>
    #include <ook/codecs/ffmpeg/src/3.1.7/libavfilter/buffersrc.h>

	// avformat   
    #include <ook/codecs/ffmpeg/src/3.1.7/libavformat/avformat.h>
   
    #define CodecID AVCodecID
	#define AVCODEC_DECAUDIO4
	#define AVCODEC_ENCAUDIO2
    #define AVCODEC_MAX_AUDIO_FRAME_SIZE CODEC_CAP_VARIABLE_FRAME_SIZE
    #define AVFILTER_GRAPH_PARSE avfilter_graph_parse_ptr

	#define AVCODEC_REGISTER 			avcodec_register
    #define AVCODEC_ALLOC_CONTEXT(a) 	avcodec_alloc_context3(a)
    #define AVCODEC_OPEN(a, b, c)    	avcodec_open2(a, b, c)
    
#elif defined(FFMPEG_V_56_60)
	
  #ifdef WIN32
	#define SRCPATH(a)         	<ook/codecs/ffmpeg/src/56_60/libavcodec/##a##>
	#define AVUTIL_SRCPATH(a)  	<ook/codecs/ffmpeg/src/56_60/libavutil/##a##>
	#define AVCODEC_SRCPATH(a) 	<ook/codecs/ffmpeg/src/56_60/libavcodec/##a##>
  #else
	#include <ook/codecs/ffmpeg/src/56_60/libavutil/intreadwrite.h> // noly for GDB usage
	#include <ook/codecs/ffmpeg/src/56_60/libavcodec/avcodec.h>
  #endif
  
	// avfilter
	#include <ook/codecs/ffmpeg/src/56_60/libavfilter/avcodec.h>
	#include <ook/codecs/ffmpeg/src/56_60/libavfilter/avfilter.h>
  	#include <ook/codecs/ffmpeg/src/56_60/libavfilter/avfiltergraph.h>
	#include <ook/codecs/ffmpeg/src/56_60/libavfilter/buffersink.h>
    #include <ook/codecs/ffmpeg/src/56_60/libavfilter/buffersrc.h>

	// avformat   
    #include <ook/codecs/ffmpeg/src/56_60/libavformat/avformat.h>
   
    #define CodecID AVCodecID
	#define AVCODEC_DECAUDIO4
	#define AVCODEC_ENCAUDIO2
    #define AVCODEC_MAX_AUDIO_FRAME_SIZE CODEC_CAP_VARIABLE_FRAME_SIZE
    #define AVFILTER_GRAPH_PARSE avfilter_graph_parse_ptr

	#define AVCODEC_REGISTER 			avcodec_register
	#define AVCODEC_ALLOC_CONTEXT(a) 	avcodec_alloc_context3(a)
	#define AVCODEC_OPEN(a, b, c)    	avcodec_open2(a, b, c)
    
#elif defined(FFMPEG_V_55_39)

  #ifdef WIN32
	#define SRCPATH(a)         	<ook/codecs/ffmpeg/src/55_39/libavcodec/##a##>
	#define AVUTIL_SRCPATH(a)  	<ook/codecs/ffmpeg/src/55_39/libavutil/##a##>
	#define AVCODEC_SRCPATH(a) 	<ook/codecs/ffmpeg/src/55_39/libavcodec/##a##>
  #else
	#include <ook/codecs/ffmpeg/src/55_39/libavcodec/avcodec.h>
  #endif

	// avfilter
	#include <ook/codecs/ffmpeg/src/55_39/libavfilter/avcodec.h>
	#include <ook/codecs/ffmpeg/src/55_39/libavfilter/avfilter.h>
  	#include <ook/codecs/ffmpeg/src/55_39/libavfilter/avfiltergraph.h>
	#include <ook/codecs/ffmpeg/src/55_39/libavfilter/buffersink.h>
    #include <ook/codecs/ffmpeg/src/55_39/libavfilter/buffersrc.h>
   
    #include <ook/codecs/ffmpeg/src/55_39/libavformat/avformat.h>
   
    #define CodecID AVCodecID
	#define AVCODEC_DECAUDIO4
	#define AVCODEC_ENCAUDIO2
    #define AVCODEC_MAX_AUDIO_FRAME_SIZE CODEC_CAP_VARIABLE_FRAME_SIZE
    #define AVFILTER_GRAPH_PARSE avfilter_graph_parse_ptr

	#define AVCODEC_REGISTER 			avcodec_register
	#define AVCODEC_ALLOC_CONTEXT(a) 	avcodec_alloc_context3(a)
	#define AVCODEC_OPEN(a, b, c)    	avcodec_open2(a, b, c)
    
#elif defined(FFMPEG_V_55_18)

  #ifdef WIN32
	#define SRCPATH(a)         	<ook/codecs/ffmpeg/src/55_18/libavcodec/##a##>
	#define AVUTIL_SRCPATH(a)  	<ook/codecs/ffmpeg/src/55_18/libavutil/##a##>
	#define AVCODEC_SRCPATH(a) 	<ook/codecs/ffmpeg/src/55_18/libavcodec/##a##>
  #else
	#include <ook/codecs/ffmpeg/src/55_18/libavcodec/avcodec.h>
  #endif
	// avfilter
	#include <ook/codecs/ffmpeg/src/55_18/libavfilter/avcodec.h>
	#include <ook/codecs/ffmpeg/src/55_18/libavfilter/avfilter.h>
  	#include <ook/codecs/ffmpeg/src/55_18/libavfilter/avfiltergraph.h>
	#include <ook/codecs/ffmpeg/src/55_18/libavfilter/buffersink.h>
    #include <ook/codecs/ffmpeg/src/55_18/libavfilter/buffersrc.h>
   
    #include <ook/codecs/ffmpeg/src/55_18/libavformat/avformat.h>
   
    #define CodecID AVCodecID
	#define AVCODEC_DECAUDIO4
	#define AVCODEC_ENCAUDIO2
    #define AVCODEC_MAX_AUDIO_FRAME_SIZE CODEC_CAP_VARIABLE_FRAME_SIZE
    #define AVFILTER_GRAPH_PARSE avfilter_graph_parse_ptr

	#define AVCODEC_REGISTER 			avcodec_register
	#define AVCODEC_ALLOC_CONTEXT(a) 	avcodec_alloc_context3(a)
	#define AVCODEC_OPEN(a, b, c)    	avcodec_open2(a, b, c)

#endif
