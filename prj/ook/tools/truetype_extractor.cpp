#include <vector>
#include <ft2build.h>
#include <ook/thread>

#ifdef WIN32
#include <wtypes.h>
#include <winnls.h>
#include <freetype/freetype.h>
#else
#include <iconv.h>
#include FT_FREETYPE_H
#endif

#include <freetype/ftoutln.h>
#include <freetype/ftstroke.h>

#include <ook/tools/truetype_extractor.h>
#include <ook/tools/strtool>
#include <ook/tools/strconv>
#include <ook/separator>
#include <ook/printbuf.h>
#include <ook/predef.h>
#include <ook/trace>
DECTRACE

#ifndef TTF_MAXLINES
#define TTF_MAXLINES 3
#endif

//////////////////////////////////
// 
#ifdef _MSC_VER
#define MIN __min
#define MAX __max
#else
#define MIN std::min
#define MAX std::max
#endif

struct Vec2
{
	Vec2() { }
	Vec2(float a, float b) : x(a), y(b) { }
	float x, y;
};

struct Rect
{
	Rect() { }
	Rect(float left, float top, float right, float bottom)
		: xmin(left), xmax(right), ymin(top), ymax(bottom) { }

	void Include(const Vec2 &r)
	{
		xmin = MIN(xmin, r.x);
		ymin = MIN(ymin, r.y);
		xmax = MAX(xmax, r.x);
		ymax = MAX(ymax, r.y);
	}

	float Width () const { return xmax - xmin + 1; }
	float Height() const { return ymax - ymin + 1; }
	
	float xmin, xmax, ymin, ymax;
};

struct Span
{
	Span() { }
	Span(int _x, int _y, int _width, int _coverage)
		: x(_x), y(_y), width(_width), coverage(_coverage) { }
	
	int x, y, width, coverage;
};

typedef std::vector<Span > Spans;

/*
  for example: '-'
  cb::y=  4, c=  3
    cb::x=  1, w=  1, c= 85
    cb::x=  2, w=  6, c=112
    cb::x=  8, w=  1, c= 85
  cb::y=  5, c=  3
    cb::x=  1, w=  1, c=196
    cb::x=  2, w=  6, c=255
    cb::x=  8, w=  1, c=196
  cb::y=  6, c=  3
    cb::x=  1, w=  1, c= 45
    cb::x=  2, w=  6, c= 60
    cb::x=  8, w=  1, c= 45
 */
static void RasterCallback(int y,
		               	   int count,
		               	   const FT_Span * spans,
		               	   void          * user) 
{
	///printf("  cb::y=%3d, c=%3d\n", y, count);
  	Spans * sptr = (Spans *)user;
  	for(int i = 0; i < count; ++i) 
  	{
  		///printf("    cb::x=%3d, w=%3d, c=%3d\n", spans[i].x, spans[i].len, spans[i].coverage);
    	sptr->push_back(Span(spans[i].x, y, spans[i].len, spans[i].coverage));
    }
}

static void RenderSpans(FT_Library & library,
			            FT_Outline * const outline,
			            Spans      * spans) 
{
	FT_Raster_Params params;
	memset(&params, 0, sizeof(params));
	params.flags      = FT_RASTER_FLAG_AA | FT_RASTER_FLAG_DIRECT;
	params.gray_spans = RasterCallback;
	params.user       = spans;
	FT_Outline_Render(library, outline, &params);
}

#if (('1234' >> 24) == '1')
  // for X86 this is right
  ///#warning "#define LIT_ENDIAN in truetype_extractor.cpp"
#elif (('4321' >> 24) == '1')
  #define BIG_ENDIAN
  ///#warning "#define BIG_ENDIAN in truetype_extractor.cpp"
#else
  #error "Couldn't determine the endianness!"
#endif

union Pixel32
{
	Pixel32() : integer(0) { }
	Pixel32(unsigned char bi, unsigned char gi, unsigned char ri, unsigned char ai = 255)
	{
		b = bi;
		g = gi;
		r = ri;
		a = ai;
	}
	
	unsigned int integer;

	struct
	{
	#ifdef BIG_ENDIAN
		unsigned char a, r, g, b;
	#else // BIG_ENDIAN
		// x86 case here
		unsigned char b, g, r, a;
	#endif // BIG_ENDIAN
	};
};

///#define TGA_WRITING

#ifdef TGA_WRITING
#include <fstream>
#include <iostream>

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int   uint32;

#if defined(_MSC_VER) || defined(__GNUC__)
#pragma pack(push, 1)
#pragma pack(1)               // Dont pad the following struct.
#endif

struct TGAHeader
{
  uint8   idLength,           // Length of optional identification sequence.
          paletteType,        // Is a palette present? (1=yes)
          imageType;          // Image data type (0=none, 1=indexed, 2=rgb,
                              // 3=grey, +8=rle packed).
  uint16  firstPaletteEntry,  // First palette index, if present.
          numPaletteEntries;  // Number of palette entries, if present.
  uint8   paletteBits;        // Number of bits per palette entry.
  uint16  x,                  // Horiz. pixel coord. of lower left of image.
          y,                  // Vert. pixel coord. of lower left of image.
          width,              // Image width in pixels.
          height;             // Image height in pixels.
  uint8   depth,              // Image color depth (bits per pixel).
          descriptor;         // Image attribute flags.
};

#if defined(_MSC_VER) || defined(__GNUC__)
#pragma pack(pop)
#endif

static bool WriteTGA(const std::string & filename,
         			 const Pixel32 * pxl,
         			 uint16 width,
         			 uint16 height)
{
	std::ofstream file(filename.c_str(), std::ios::binary);
	if (file)
	{
		TGAHeader header;
		memset(&header, 0, sizeof(TGAHeader));
		header.imageType  = 2;
		header.width      = width;
		header.height     = height;
		header.depth      = 32;
		header.descriptor = 0x20;
	
		file.write((const char *)&header, sizeof(TGAHeader));
		file.write((const char *)pxl, sizeof(Pixel32) * width * height);
		return true;
	}
	return false;
}

#endif // TGA_WRITING

///////////////////////////////////////
// truetype_extractor

struct library_s
{
    FT_Library library;
    FT_Face face;	
};

void truetype_extractor::show_face(int t)
{
	if(!_library)
		return;
	char tmpbuf[128];
	library_s * s  = (library_s *)_library;	
	FT_Face & face = s->face;
	
	BTRACE(t, "ttf face[" << face->family_name << "] {")
	__snprintf(tmpbuf, 128, "style     =[%s]", face->style_name);
	FTRACE(t, tmpbuf)
	__snprintf(tmpbuf, 128, "faces     =[%d]", (int)face->num_faces);
	FTRACE(t, tmpbuf)
	__snprintf(tmpbuf, 128, "glyphs    =[%d]", (int)face->num_glyphs);
	FTRACE(t, tmpbuf)
	///__snprintf(tmpbuf, 128, "bbox      =[%d/%d/%d/%d]", (int)face->bbox.xMin, (int)face->bbox.xMax, (int)face->bbox.yMin, (int)face->bbox.yMax);
	///FTRACE(t, tmpbuf)
	__snprintf(tmpbuf, 128, "height    =[%d]", (int)face->height);
	FTRACE(t, tmpbuf)
	__snprintf(tmpbuf, 128, "max_width =[%d]", (int)face->max_advance_width);
	FTRACE(t, tmpbuf)
	__snprintf(tmpbuf, 128, "mxa_height=[%d]", (int)face->max_advance_height);
	FTRACE(t, tmpbuf)
	ETRACE(t, "}")
}

truetype_extractor::truetype_extractor()
	: _library(NULL)
{
	outline.working = true;
	outline.width   = 6;
	outline.color   = 0;
}

truetype_extractor::~truetype_extractor()
{
	unload_library();
}

void truetype_extractor::set_locale(const char * cur_locale)
{
#ifndef WIN32
	// locale -a | grep zh_CN*
	// cur_locale = "zh_CN";
	FTRACE(2, "truetype_extractor::set_locale::locale=[" << (cur_locale ? cur_locale : "") << "]")
	char * p = setlocale(LC_ALL, cur_locale);
	if(p)
		FTRACE(2, "locale=[" << p << "]")
	else
		FTRACE(2, "setlocale fail")
#endif
}

void truetype_extractor::release_bmp(bmp_s * bmp)
{
	if(bmp)
	{
		if(bmp->data)
			delete [] (Pixel32 *)bmp->data;
		delete bmp;
	}
}

void truetype_extractor::unload_library()
{
	if(_library)
	{
		library_s * s = (library_s *)_library;
		_library = NULL;
		
		FT_Done_Face(s->face);
		FT_Done_FreeType(s->library);
	}
}

bool truetype_extractor::load_library(const char * library_path,
									  unsigned int font_size)
{
	unload_library();
	
	library_s * s  = new library_s;
	
	if(FT_Init_FreeType(&s->library) == 0)
	{
		if(FT_New_Face(s->library, library_path, 0, &s->face) == 0)
		{
			if(font_size > 0)
			{
				FT_Set_Pixel_Sizes(s->face, 0, font_size);
				///FT_Set_Char_Size(s->face, 16 << 6, 16 << 6, 300, 300);
			}
			FT_Select_Charmap(s->face, FT_ENCODING_UNICODE);
			_library = s;
			return true;
		}
		FT_Done_FreeType(s->library);
	}

	delete s;
	return false;
}

truetype_extractor::bmp_s * 
truetype_extractor::select(unsigned int charcode   /* must be unicode */,
						   unsigned int font_size,
						   unsigned int font_color /* BRGA */,
						   unsigned int printmask  /* = 0 */)
{
	if(!_library)
		return NULL;
	
	///printf("truetype_extractor::select\n");
	
	///printmask = 0xffff;
	
	library_s * s = (library_s *)_library;
	
	if(font_size > 0)
		FT_Set_Pixel_Sizes(s->face, 0, font_size);
	
	FT_UInt gindex = FT_Get_Char_Index(s->face, charcode);
	if(printmask & 0x01)
		printf("charcode=%u, gindex=%u\n", charcode, gindex);
	if(gindex == 0)
		return NULL;
	
	if(FT_Load_Glyph(s->face, gindex, FT_LOAD_NO_BITMAP) != 0)
	{
		TRACE(3, "truetype_extractor::select::load glyph fail")
		return NULL;
	}

    if(s->face->glyph->format != FT_GLYPH_FORMAT_OUTLINE)
    {
    	TRACE(3, "truetype_extractor::select::glyph format error")
        return NULL;
    }
    
	if(printmask & 0x04)
		printf("Render outline {\n");
		
	Spans spans;
	RenderSpans(s->library, &s->face->glyph->outline, &spans);
	
	if(printmask & 0x04)
		printf("} %u\n", (unsigned int)spans.size());
	
	Spans outlineSpans;
	
	if(outline.working)
	{
		// Set up a stroker.
		FT_Stroker stroker;
		FT_Stroker_New(s->library, &stroker);
		FT_Stroker_Set(stroker,
		               outline.width * 64,
		               FT_STROKER_LINECAP_ROUND,
		               FT_STROKER_LINEJOIN_ROUND,
		               0);
              
		FT_Glyph glyph;
		if(FT_Get_Glyph(s->face->glyph, &glyph) == 0)
		{
			FT_Glyph_StrokeBorder(&glyph, stroker, 0, 1);
			if(glyph->format == FT_GLYPH_FORMAT_OUTLINE)
			{
				// Render the outline spans to the span list
				FT_Outline * o = &reinterpret_cast<FT_OutlineGlyph >(glyph)->outline;
				
				if(printmask & 0x04)
					printf("Render border {\n");
					
				RenderSpans(s->library, o, &outlineSpans);
				
				if(printmask & 0x04)
					printf("} %u\n", (unsigned int)outlineSpans.size());
			}

			// Clean up afterwards.
			FT_Stroker_Done(stroker);
			FT_Done_Glyph(glyph);
		}    
	}

	if(spans.empty())
	{
		if(charcode == 32) // is space
		{
			bmp_s * bmp = new bmp_s;
			bmp->width  = 0;
			bmp->height = 0;
			bmp->bottom = 0;
			bmp->stride = 0;
			bmp->data   = NULL;
			return bmp;
		}
		return NULL;
	}

	// Figure out what the bounding rect is for both the span lists.
	///printf("x=%d, y=%d\n", spans.front().x, spans.front().y);
	Rect rect((float)spans.front().x,
	          (float)spans.front().y,
	          (float)spans.front().x,
	          (float)spans.front().y);

	Spans::iterator ite;
	for(ite = spans.begin(); ite != spans.end(); ++ite)
	{
		rect.Include(Vec2((float)ite->x, (float)ite->y));
		rect.Include(Vec2((float)ite->x + ite->width - 1, (float)ite->y));
	}
	if(printmask & 0x01)
		printf("rc::x=%f~%f, y=%f~%f\n", rect.xmin, rect.xmax, rect.ymin, rect.ymax);
						
	if(outline.working)
	{
		for(ite = outlineSpans.begin(); ite != outlineSpans.end(); ++ite)
		{
			rect.Include(Vec2((float)ite->x, (float)ite->y));
			rect.Include(Vec2((float)ite->x + ite->width - 1, (float)ite->y));
		}
		if(printmask & 0x01)
			printf("rc::x=%f~%f, y=%f~%f\n", rect.xmin, rect.xmax, rect.ymin, rect.ymax);
	}
	
	// Get some metrics of our image.
	int imgWidth  = (int)rect.Width();
	int imgHeight = (int)rect.Height();
	int imgBottom = (int)rect.ymin;
	int imgSize   = imgWidth * imgHeight;
	if(printmask & 0x01)
		printf("img=%dX%d\n", imgWidth, imgHeight);
	
	// Allocate data for our image and clear it out to transparent.
	int pxsize = ((imgSize >> 2) << 2) + 128;
	Pixel32 * pxl = new Pixel32[pxsize];
	memset(pxl, 0, sizeof(Pixel32) * pxsize);

	if(printmask & 0x01)
		printf("font:%x b(%x) g(%x) r(%x) a(%x)\n", font_color, ((font_color >> 24) & 0xff), ((font_color >> 16) & 0xff), ((font_color >> 8) & 0xff), (font_color & 0xff));
	
	if(outline.working)
	{
		for(ite = outlineSpans.begin(); ite != outlineSpans.end(); ++ite)
		{
			for(int w = 0; w < ite->width; ++w)
			{
		    	pxl[(int)((imgHeight - 1 - (ite->y - rect.ymin)) * imgWidth + ite->x - rect.xmin + w)] =
		      		Pixel32((outline.color >> 24) & 0xff, 
		      				(outline.color >> 16) & 0xff, 
		      				(outline.color >>  8) & 0xff,
		      				ite->coverage);
			}
		}
	}

	///printf("font_color=%x: %x/%x/%x\n", font_color, (font_color >>  16) & 0xff, (font_color >> 8) & 0xff, (font_color >> 0) & 0xff);
    for(ite = spans.begin(); ite != spans.end(); ++ite)
    {
		for(int w = 0; w < ite->width; ++w)
		{
			Pixel32 & dst = pxl[(int)((imgHeight - 1 - (ite->y - rect.ymin)) * imgWidth + ite->x - rect.xmin + w)];
			Pixel32 src = Pixel32((font_color >> 24) & 0xff /* B */,
		      					  (font_color >> 16) & 0xff /* G */, 
		      					  (font_color >>  8) & 0xff /* R */, 
		      					  ite->coverage);
			dst.r = (int)(dst.r + ((src.r - dst.r) * src.a) / 255.0f);
			dst.g = (int)(dst.g + ((src.g - dst.g) * src.a) / 255.0f);
			dst.b = (int)(dst.b + ((src.b - dst.b) * src.a) / 255.0f);
			
			///dst.a = MIN(255, dst.a + src.a);
			dst.a = (font_color & 0xff);
			///printf("[%x]", dst.integer);
		}
    }
    ///printf("\n");
    
    #if 0
    {
	    for(int i = 0; i < imgSize; i++)
	    {
	    	printf("[%8x]", pxl[i].integer);
	    	//if(((i + 1) % 8) == 0)
	    	//	printf("\n");
	    }
	    printf("\n");
	}
	#endif
    
#ifdef TGA_WRITING    
	///WriteTGA("ook.tga", pxl, imgWidth, imgHeight);
#endif

	bmp_s * bmp = new bmp_s;
	{
		bmp->width      = imgWidth;
		bmp->height     = imgHeight;
		bmp->bottom     = imgBottom;
		bmp->stride     = bmp->width * 4;
		bmp->data       = (unsigned char *)pxl; // BGRA formated
	}
	return bmp;
}

truetype_extractor::bmp_s * 
truetype_extractor::select(const char * str, 
						   bool unicode,
						   unsigned int font_size, 
						   unsigned int font_color,
						   unsigned int space,
						   unsigned int h_space,
						   unsigned int v_space, 
						   int left,
						   int fixed_width,
						   unsigned int printmask)
{
	bmp_s * bmp = NULL;
	typedef std::vector<bmp_s * > BMPS;
	BMPS bmps[TTF_MAXLINES];
	bool is_ascii[TTF_MAXLINES];
	
	///printf("truetype_extractor::select\n");
	///printmask = 0xffffffff;
	
	separator sept(str, "\r\n");
	int line, lines = sept.size();
	if(lines > TTF_MAXLINES)
		lines = TTF_MAXLINES;
	if(printmask & 0x01)
		printf("lines=%d\n", lines);
	
	for(line = 0; line < lines; line++)
	{
		const char * s_line = sept[line];
		if(!s_line)
			continue;

		int l_line = strlen(s_line);
		if(l_line < 1)
			continue;
		
		is_ascii[line] = is_ascii_string(s_line);
		if(printmask & 0x01)
			printf("is_ascii=%d [%s]\n", is_ascii[line], s_line);
		
	#ifdef WIN32
		int uni_len = 0;
		wchar_t * uni_buf = NULL;
		if(unicode)
		{
			uni_len = l_line >> 1;
			uni_buf = (wchar_t *)s_line;
		}
		else
		{
			uni_len = MultiByteToWideChar(CP_ACP, 0, s_line, -1, NULL, 0);
			if(uni_len < 1)
				return NULL;
			uni_buf = new wchar_t[uni_len];
			MultiByteToWideChar(CP_ACP, 0, s_line, -1, uni_buf, uni_len);
		}
		for(int i = 1; i < uni_len; i++)
		{
			unsigned int charcode = uni_buf[i - 1];
			bmp = select(charcode, font_size, font_color, printmask);
			if(bmp)
				bmps[line].push_back(bmp);
		}
		if(!unicode)
			delete [] uni_buf;
	#else
		size_t uni_len = 0;
		wchar_t * uni_buf = NULL;
		if(unicode)
		{
			uni_len = l_line >> 1;
			uni_buf = (wchar_t *)s_line;		
		}
		else
		{
		#if 1 // change to UTF8
			ANSICODE2UTF8 a2u(s_line);
			if(!a2u.c_str())
				return NULL;
			uni_len = mbstowcs(NULL, a2u.c_str(), 0);
			///printf("uni_len=%d\n", (int)uni_len);
			if(uni_len < 1)
				return NULL;
			uni_buf = new wchar_t[uni_len + 8];
			mbstowcs(uni_buf, a2u.c_str(), uni_len + 8);				
		#else
			uni_len = mbstowcs(NULL, s_line, 0);
			///printf("uni_len=%d\n", (int)uni_len);
			if(uni_len < 1)
				return NULL;
			uni_buf = new wchar_t[uni_len + 1];
			mbstowcs(uni_buf, s_line, uni_len + 1);
		#endif
		}
		for(int i = 0; i < (int)uni_len; i++)
		{
			unsigned int charcode = uni_buf[i];
			bmp = select(charcode, font_size, font_color, printmask);
			///printf("charcode[%u]=%p\n", charcode, bmp);
			if(bmp)
				bmps[line].push_back(bmp);
		}
		if(!unicode)
			delete [] uni_buf;
	#endif
	}

	int width [TTF_MAXLINES];	
	int height[TTF_MAXLINES];
	int bottom[TTF_MAXLINES];
	BMPS::const_iterator ite;
	for(line = 0; line < lines; line++)
	{
		width [line] = 0;
		height[line] = 0;
		bottom[line] = 0;
		for(ite = bmps[line].begin(); ite != bmps[line].end(); ite++)
		{
			bmp_s * b = *ite;
			///printf("1:%d/%d/%d\n", b->width, b->height, b->bottom);
			if(b->data && b->bottom < bottom[line])
				bottom[line] = b->bottom;
		}
		if(bottom[line] != 0)
		{
			///printf("bottom[%d]=%d\n", line, bottom[line]);
			for(ite = bmps[line].begin(); ite != bmps[line].end(); ite++)
			{
				bmp_s * b  = *ite;
				if(b->data)
					b->bottom += (-bottom[line]);
				///printf("2:%d/%d/%d\n", b->width, b->height, b->bottom);
			}
		}		
		for(ite = bmps[line].begin(); ite != bmps[line].end(); ite++)
		{
			bmp_s * b = *ite;
			if(b->height + b->bottom > height[line])
				height[line] = b->height + b->bottom;
			if(b->width > 0)
				width[line] += b->width + h_space;
			else
				width[line] += space;
		}
		if(printmask & 0x01)
			printf("line[%d]=%dX%d\n", line, width[line], height[line]);
	}

	int w = 0;
	int h = 0;
	for(line = 0; line < lines; line++)
	{
		if(width[line] > w)
			w = width[line];
		h += height[line];
		if(line > 0)
			h += v_space;
	}
	
	{
		int w0 = w;
		w = (w0 >> 2) << 2;
		if(w < w0)
			w += 4;
		if(fixed_width > 0 && w < fixed_width)
			w = fixed_width;
		int h0 = h;
		h = (h0 >> 2) << 2;
		if(h < h0)
			h += 4;
	}

	if(printmask & 0x01)
		printf("img=%dX%d, space=%d/%d/%d\n", w, h, space, h_space, v_space);
		
	if(w > 0 && h > 0)	
	{
		bmp = new bmp_s;
		bmp->width  = w;
		bmp->height = h;
		bmp->stride = bmp->width * 4;
			
		if(printmask & 0x01)
			TRACE(3, "bmp::width=" << bmp->width << ", height=" << bmp->height << ", stride=" << bmp->stride)
				
		bmp->data = new unsigned char[bmp->stride * bmp->height];
		memset(bmp->data, 0, bmp->stride * bmp->height);
		
		unsigned int pos = 0;
		for(line = 0; line < lines; line++)
		{
			unsigned char * start = bmp->data + pos;
			///printf("start[%d]::w=%d h=%d @ %u\n", line, width[line], height[line], pos);
			
			for(int j = 0; j < height[line]; j++)
			{
				unsigned int * dst = (unsigned int *)(start + bmp->stride * j);
				if(left < 0 && fixed_width < 1)
					dst += (w - width[line]) >> 1;
				
				for(ite = bmps[line].begin(); ite != bmps[line].end(); ite++)
				{
					bmp_s * b = *ite;
					if(b->data)
					{
						int d = height[line] - b->height;
						int m = b->bottom > 0 ? b->bottom : 0;
						if(j >= (d - m) && j < height[line] - m)
						{
							unsigned int * src = (unsigned int *)(b->data + b->stride * (j - d + m));
							for(int i = 0; i < b->width; i++)
							{
								*dst = *src;
								///printf("<%8x>", *dst);
								src++;
								dst++;
							}
						}
						else
						{
							dst += b->width;
						}
						dst += h_space;
					}
					else // space
					{
						dst += space;
					}
				}
			}
			///printf("\n");
			pos += bmp->stride * (height[line] + v_space);
		}
	}
	for(line = 0; line < lines; line++)
	{	
		for(ite = bmps[line].begin(); ite != bmps[line].end(); ite++)
			release_bmp(*ite);
	}
	return bmp;
}
