#ifndef __ANYBUF_S_H__
#define __ANYBUF_S_H__

typedef struct any_buf_s // 24 + 12 = 36
{
	unsigned int size;
	unsigned int len;
	unsigned int pos;
	unsigned int owner;
	unsigned int unknowi;
	unsigned int unknowl;
	
	void * unknowp;
	unsigned char * buf;

	/* inner usage, do NOT use by caller */
	void * raw;	// buffer malloced
		
} any_buf_s;

any_buf_s * create_any_buf(unsigned int owner, unsigned int size);
void release_any_buf(any_buf_s * s);

int attach_any_buf(any_buf_s * s,
				   const unsigned char * p, 
				   unsigned int len, 
				   unsigned int pos);
				   
int detach_any_buf(any_buf_s * s);

#endif
