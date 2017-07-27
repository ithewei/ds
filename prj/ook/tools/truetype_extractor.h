#ifndef __TRUETYPE_EXTRACTOR_H__
#define __TRUETYPE_EXTRACTOR_H__

#include <stdlib.h>
#include <string>

class truetype_extractor
{
public:
	struct outline_s
	{
		bool working;   
		unsigned int width;
		unsigned int color;
	} outline;
				   	
	struct bmp_s
	{
		int width;
		int height;
		int bottom;
		int stride;
		unsigned char * data; // BGRA formated
	};
	typedef bmp_s * PBMP;
	
	truetype_extractor();
	~truetype_extractor();
	
	static void set_locale(const char * cur_locale);
	static void release_bmp(bmp_s * bmp);
	
	bool load_library(const char * library_path, 
					  unsigned int font_size);
	void unload_library();
	
	void show_face(int t);
	
	bmp_s * select(unsigned int charcode,
				   unsigned int font_size,
				   unsigned int font_color, /* BGRA */
				   unsigned int printmask = 0);
				   
	bmp_s * select(const char * str, 
				   bool unicode, 
				   unsigned int font_size, 
				   unsigned int font_color,
				   unsigned int space,
				   unsigned int h_space, 
				   unsigned int v_space,
				   int left,
				   int fixed_width,
				   unsigned int printmask);
	
private:
	void * _library;
};

#endif
