#include "tga.h"

// 未压缩的TGA头
static unsigned char uTGAcompare[12] = {0,0, 2,0,0,0,0,0,0,0,0,0};
// 压缩的TGA头
static unsigned char cTGAcompare[12] = {0,0,10,0,0,0,0,0,0,0,0,0};

bool loadTGA(const char* fname, Texture* tex){
	if (!fname || !tex)
		return false;
	
	FILE* fp = fopen(fname, "rb");
	if (!fp)
		return false;
	
	bool bRet;
	do{
		TGAHeader th;
		int iRead = fread(&th.header, 1, sizeof(th.header), fp);
		if (iRead != sizeof(th.header)){
			bRet = false;
			break;
		}

		unsigned char tmp[6];
		iRead = fread(tmp, 1, 6, fp);
		if (iRead != 6){
			bRet = false;
			break;
		}
		tex->width = th.width = ((int)tmp[1]) << 8 | tmp[0];
		tex->height = th.height = ((int)tmp[3]) << 8 | tmp[2];
		tex->bpp = th.bpp = tmp[4];

		int bytes = th.bpp/8;
		if (bytes == 3){
            tex->type = GL_BGR;
		}else if(bytes == 4){
			tex->type = GL_BGRA;
		}else{
			bRet = false;
			break;
		}

		glGenTextures(1, &tex->texID);
		glBindTexture(GL_TEXTURE_2D, tex->texID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		tex->data = (GLubyte*)malloc(bytes * tex->width * tex->height);
		if (!tex->data){
			bRet = false;
			break;
		}
		
		if (memcmp(th.header, uTGAcompare, sizeof(th.header)) == 0)
			bRet = loadUncompressedTGA(fp, tex);
		else if (memcmp(th.header, cTGAcompare, sizeof(th.header)) == 0)
			bRet = loadCompressedTGA(fp, tex);
		else{
			bRet = false;
			break;
		}
	}while(0);
	
	fclose(fp);
	if (!bRet){
		if (tex->texID != 0){
			glDeleteTextures(1, &tex->texID);
			tex->texID = 0;
		}
		if (tex->data){
			free(tex->data);
			tex->data = NULL;
		}
	}
	return bRet;
}

// 读取一个未压缩的文件
bool loadUncompressedTGA(FILE* fp, Texture* tex){
	if (!fp || !tex || !tex->data)
		return false;

	int imgSize = tex->bpp*tex->width*tex->height/8;
	int iRead = fread(tex->data, 1, imgSize, fp);
	if (iRead != imgSize)
		return false;

	// BGR转RGB
	//int bytes = tex->bpp/8;
	//GLubyte* data = tex->data;
	//for (int i = 0; i < imgSize; i = i+bytes){
	//	GLubyte tmp = data[i];
	//	data[i] = data[i+2];
	//	data[i+2] = tmp;
	//}

	return true;
}

// 读取一个压缩的文件
bool loadCompressedTGA(FILE* fp, Texture* tex){
	if (!fp || !tex)
		return false;
	
	return false;
}
