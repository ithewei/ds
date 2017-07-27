#include <ook/thread>

extern "C" {
	#define please_use_av_malloc malloc
	#include "__ffmpeg.c"
  #ifdef WIN32
	int _get_output_format(void) { return 0; }  // need by libmingwex.a
  #endif
}

static tmc_mutex_type avcodec_mu_;

void lock_avcodec_s()
{
	avcodec_mu_.lock();	
}

void unlock_avcodec_s()
{
	avcodec_mu_.unlock();	
}
