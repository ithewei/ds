static const char * streamname_by_streamtype(int streamtype)
{
	static const char * __streamnameA__[] = 
	{
		"",
		"MP1A",
		"MP2A",
		"MP3",
		"AAC",
		"AC3",
		"EAC3",
		"DTS",
		"AMR",
		"WMA",
		"VORBIS",	// 10
		
		"PCMU",
		"PCMA",
		"LPCM",
		"PCM"
	};
	
	static const char * __streamnameV__[] = 
	{
		"",
		"MP1V",
		"MP2V",
		"MP4V",
		"H263",
		"H264",
		"HEVC",
		"AVS",
		"WMV",
		"VC1",
		"ProRes",	// 10
		
		"THEORA"
	};
	
	int i = 0;
	switch(streamtype)
	{
	case STREAM_TYPE_AUDIO_MPEG1:
		i = 1;
		break;
	case STREAM_TYPE_AUDIO_MPEG2:
		i = 2;
		break;
	case STREAM_TYPE_AUDIO_MP3:
		i = 3;
		break;
	case STREAM_TYPE_AUDIO_AAC:
	case STREAM_TYPE_AUDIO_AAC_LATM:
		i = 4;
		break;
	case STREAM_TYPE_AUDIO_AC3:
		i = 5;
		break;	
	case STREAM_TYPE_AUDIO_EAC3:
		i = 6;
		break;	
	case STREAM_TYPE_AUDIO_DTS_HDMV:
	case STREAM_TYPE_AUDIO_DTS_HD:
	case STREAM_TYPE_AUDIO_DTS:
		i = 7;
		break;
	case STREAM_TYPE_AUDIO_AMR:
	case STREAM_TYPE_AUDIO_AMWB:
		i = 8;
		break;
	case STREAM_TYPE_AUDIO_WMA:
		i = 9;
		break;
	case STREAM_TYPE_AUDIO_VORBIS:
		i = 10;
		break;
		
	case STREAM_TYPE_AUDIO_PCMU:
		i = 11;
		break;	
	case STREAM_TYPE_AUDIO_PCMA:
		i = 12;
		break;
	case STREAM_TYPE_AUDIO_LPCM:
		i = 13;
		break;
	case STREAM_TYPE_AUDIO_PCM:
		i = 14;
		break;
	default:
		break;
	}
	if(i > 0)
		return __streamnameA__[i];
		
	i = 0;
	switch(streamtype)
	{		
	case STREAM_TYPE_VIDEO_MPEG1:
		i = 1;
		break;
	case STREAM_TYPE_VIDEO_MPEG2:
		i = 2;
		break;	
	case STREAM_TYPE_VIDEO_MPEG4:
		i = 3;
		break;
	case STREAM_TYPE_VIDEO_H263:
		i = 4;
		break;
	case STREAM_TYPE_VIDEO_H264:
		i = 5;
		break;
	case STREAM_TYPE_VIDEO_H265:
		i = 6;
		break;
	case STREAM_TYPE_VIDEO_AVS:
		i = 7;
		break;
	case STREAM_TYPE_VIDEO_WMV:
		i = 8;
		break;
	case STREAM_TYPE_VIDEO_VC1:
		i = 9;
		break;
	case STREAM_TYPE_VIDEO_ProRes:
		i = 10;
		break;
	
	case STREAM_TYPE_VIDEO_THEORA:
		i = 11;
		break;
	default:
		break;
	}
	return __streamnameV__[i];
}
