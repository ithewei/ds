#ifndef __OOK_WINDEF_H__
#define __OOK_WINDEF_H__

typedef struct
{
	uint16_t wFormatTag; 
    uint16_t nChannels; 
    uint32_t nSamplesPerSec; 
    uint32_t nAvgBytesPerSec; 
    uint16_t nBlockAlign; 
    uint16_t wBitsPerSample; 
    uint16_t cbSize;
    
} WAVEFORMATEX;

/*
      0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15
     |Samples|dwChannelMask  |Data1          |Data2
 0:  10   0   f   6   0   0   0  20   0   0   0   0  10   0  80   0
 1:   0  aa   0  38  9b  71
 */
typedef struct 
{
	WAVEFORMATEX Format;
	
	union 
	{
		uint16_t wValidBitsPerSample;
		uint16_t wSamplesPerBlock;
		uint16_t wReserved;
	} Samples;
	
	uint32_t dwChannelMask;
	
	struct 
	{
	    uint32_t Data1;
	    uint16_t Data2;
	    uint16_t Data3;
	    unsigned char Data4[8];
	} SubFormat;
	
} WAVEFORMATEXTENSIBLE;

typedef struct
{ 
  uint32_t biSize; 
  int32_t  biWidth; 
  int32_t  biHeight; 
  uint16_t biPlanes; 
  uint16_t biBitCount;
  uint32_t biCompression; 
  uint32_t biSizeImage; 
  int32_t  biXPelsPerMeter; 
  int32_t  biYPelsPerMeter; 
  uint32_t biClrUsed; 
  uint32_t biClrImportant;

} BITMAPINFOHEADER; 

#endif
