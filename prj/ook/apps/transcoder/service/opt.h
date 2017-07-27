#ifndef __TASK_SERVICE_OPT_S__
#define __TASK_SERVICE_OPT_S__

#define TASK_SERVICE_OPT_FRAMERATE  0x01
#define TASK_SERVICE_OPT_FRAMEOPER  0x02

typedef struct service_opt_s
{
	unsigned int mask;
	unsigned int numb;
	unsigned int spef;
	int opt;
	int chr;
	int lparam;
	int wparam;
	void * ptr;
} service_opt_s;

inline void init_service_opt_s(service_opt_s * s)
{
	s->mask   = 0;
	s->numb   = 0;
	s->spef   = 0;
	s->chr    = 0;
	s->opt    = -1;
	s->lparam = 0;
	s->wparam = 0;
	s->ptr    = NULL;
}

#endif
