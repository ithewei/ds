#ifdef WIN32

static int __loadbmpfrmbuffer(const unsigned char * buffer, 
			  		   		  unsigned int buflen,
			  		   		  av_picture * pic, 
			  		   		  BITMAPINFOHEADER * pBmpHd)
{	
	if(!buffer || buflen < sizeof(BITMAPFILEHEADER))
		return -1;

	BYTE * lpBitmap = (BYTE *)buffer;
	BYTE * lpBits;
	BITMAPINFOHEADER * pInfo;

    int nWidth, nHeight, nBitCount;
    	
	if(lpBitmap[0] != 'B' && lpBitmap[1] != 'M')
		return -2;
	
	pInfo     = (BITMAPINFOHEADER *)(lpBitmap + sizeof(BITMAPFILEHEADER));
	if(pBmpHd)
		memcpy(pBmpHd, pInfo, sizeof(BITMAPFILEHEADER));

	lpBits    = (BYTE *)pInfo + sizeof(BITMAPINFOHEADER);
	nWidth    = pInfo->biWidth;
	nHeight   = pInfo->biHeight;
    nBitCount = pInfo->biBitCount;
    ///printf("loadbmp::%d, %d, %d\n", nWidth, nHeight, nBitCount);
    
    switch(nBitCount)
   	{
   	///case 16:
   	case 24:
   	///case 32:
   		if(pic)
   		{
	   		pic->stride[0] = nWidth * (nBitCount / 8);
	   		pic->data[0]   = (unsigned char *)malloc(pic->stride[0] * nHeight + 32);
	   		
	   		unsigned char * dst = pic->data[0];
	   		unsigned char * src = lpBits + pic->stride[0] * (nHeight - 1);
	   		for(int i = 0; i < nHeight; i++)
	   		{
	   			memcpy(dst, src, pic->stride[0]);
	   			dst += pic->stride[0];
	   			src -= pic->stride[0];
	   		}

	   		pic->width       = nWidth;
	   		pic->height      = nHeight;
	   		pic->selfrelease = 1;
	   		pic->continuebuf = 1;
	   		switch(nBitCount)
	   		{
	   		case 16:
	   			pic->fourcc = OOK_FOURCC('R', 'V', '1', '6');
	   			break;
	   		case 24:
	   			pic->fourcc = OOK_FOURCC('R', 'V', '2', '4');
	   			break;
	   		case 32:
	   			pic->fourcc = OOK_FOURCC('R', 'V', '3', '2');
	   			break;
	   		} 
	   	}
   		delete [] lpBitmap;
   		break;
   	default:
   		delete [] lpBitmap;
   		return -3;
   	}
	return 0;
}

int __loadbmp(const char * file_path, 
			  const unsigned char * buffer,
			  unsigned int buflen,
			  av_picture * pic, 
			  BITMAPINFOHEADER * pBmpHd)
{
	FILE * fp = NULL;
	BYTE * lpBitmap = NULL;

	if(file_path && strlen(file_path) > 0)
	{
		fp = fopen(file_path, "rb");
		if(!fp)
			return -1;
		
		fseek(fp, 0L, SEEK_END);
		buflen = (unsigned int)ftell(fp);
		fseek(fp, 0L, SEEK_SET);

		lpBitmap = new BYTE[buflen];
		fread(lpBitmap, 1, buflen, fp);
		fclose(fp);
		
		buffer = lpBitmap;
	}

	int r = __loadbmpfrmbuffer(buffer, buflen, pic, pBmpHd);
	
	if(lpBitmap)
		delete [] lpBitmap;
	return r;
}

#endif

int __loadtga(const char * file_path,
			  const unsigned char * buffer,
			  unsigned int buflen,
			  av_picture * pic)
{
	int r = -1;
	tga_reader tga;
	if(file_path && strlen(file_path) > 0)
		r = tga.load(file_path, &pic->data[0], 0);
	else if(buffer)
		r = tga.load(buffer, buflen, &pic->data[0], 0);
	if(r < 0)
	{
		FTRACE(3, "loadtga::load fial fail, err=" << r)
		return r;
	}

	FTRACE(4, "loadtga::colorMode=" << tga.colorMode)
	switch(tga.colorMode)
	{
	case 3:
	case 4:
   		if(pic)
   		{
	   		pic->stride[0]   = tga.imageWidth * tga.colorMode;
	   		pic->width       = tga.imageWidth;
	   		pic->height      = tga.imageHeight;
	   		pic->selfrelease = 1;
	   		pic->continuebuf = 1;
	   		switch(tga.colorMode)
	   		{
	   		case 3:
	   			pic->fourcc  = OOK_FOURCC('R', 'V', '2', '4');
	   			break;
	   		case 4:
	   			pic->fourcc  = OOK_FOURCC('R', 'V', '3', '2');
	   			break;
	   		} 
	   		return 0;
	   	}
		break;
	default:
		if(pic->data[0])
		{
			free(pic->data[0]);
			pic->data[0] = NULL;
		}
		FTRACE(3, "loadtga::load tag fail, colorMode=" << tga.colorMode)
		break;
	}
	return -1;
}

#ifndef WIN32
#ifndef NOPNG_LIB
int __loadpng(const char * file_path,
			  const unsigned char * buffer,
			  unsigned int buflen,
			  av_picture * pic)
{
	int r = -1;
	png_reader png;
	if(file_path && strlen(file_path) > 0)
		r = png.load(file_path, (pic ? &pic->data[0] : NULL));
	if(r < 0)
	{
		FTRACE(3, "loadpng::load fial fail, err=" << r)
		return r;
	}
	if(pic)
	{
   		pic->stride[0]   = png.imageWidth * 4;
   		pic->width       = png.imageWidth;
   		pic->height      = png.imageHeight;
   		pic->selfrelease = 1;
   		pic->continuebuf = 1;
		pic->fourcc      = OOK_FOURCC('R', 'V', '3', '2');
   		return 0;
	}
	return -1;	
}
#endif
#endif
