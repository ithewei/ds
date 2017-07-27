#include "predef.h"
#include "buf_s.h"
#include "mem.h"

any_buf_s * create_any_buf(unsigned int owner, unsigned int size)
{
	void * raw = NULL;
	any_buf_s * s = NULL;
	raw = malloc__(sizeof(any_buf_s) + size + 4);
	if(raw)
	{
		s = (any_buf_s *)raw;
		s->size    = size;
		s->len     = 0;
		s->pos     = 0;
		s->owner   = owner;
		s->unknowi = 0;
		s->unknowl = 0;
		s->unknowp = NULL;
	
		s->raw = raw;
		s->buf = (unsigned char *)s->raw + sizeof(any_buf_s);
	}
	return s;
}

void release_any_buf(any_buf_s * s)
{
	if(s && s->raw)
		free__(s->raw);
}

int attach_any_buf(any_buf_s * s,
				   const unsigned char * p, 
				   unsigned int len, 
				   unsigned int pos)
{
	s->buf = (unsigned char *)p;
	s->len = len;
	s->pos = pos;
	return s->len;
}
				   
int detach_any_buf(any_buf_s * s)
{
	s->buf = NULL;
	s->len = 0;
	s->pos = 0;
	return 0;
}

