#ifndef __TGA_H__
#define __TGA_H__

#include <stdio.h>
#include <string.h>
#include <hglwidget.h>
 
typedef struct{
	unsigned char header[12];   // 文件头决定文件类型
	unsigned short width;
	unsigned short height;
	unsigned char bpp;
	unsigned char reserve;
}TGAHeader;

// 读取一个未压缩的文件
bool loadUncompressedTGA(FILE* fp, Texture* tex);
// 读取一个压缩的文件
bool loadCompressedTGA(FILE* fp, Texture* tex);

bool loadTGA(const char* fname, Texture* tex);
 
#endif // __TGA_H__
