#ifdef OEM_MALLOC

#include <ook/mem.h>

#ifndef SYS_MEMSIZE
#define SYS_MEMSIZE (1024 * 2048)  // 2M
#endif

#ifndef SYS_LMEMSIZE
#define SYS_LMEMSIZE (1024 * 1024) // 1M
#endif

#ifndef SYS_MEMALIGNMENT
#define SYS_MEMALIGNMENT 16
#endif

#define SYS_MEMTHREADENABLED
#define SYS_MEMLONGLIFEENABLED

struct mem_s
{
	unsigned int next;
	unsigned int prev;
	unsigned int used;
};

#define SIZEOF_STRUCT_MEM  16

static unsigned char sysmem_ [SYS_MEMSIZE +  SIZEOF_STRUCT_MEM];
static struct mem_s * plast_  = NULL;
static struct mem_s * pfree_  = NULL;  /* pointer to the lowest free block */

#ifdef SYS_MEMLONGLIFEENABLED
static unsigned char lsysmem_[SYS_LMEMSIZE + SIZEOF_STRUCT_MEM]; // static mem for long life usage
static struct mem_s * pllast_ = NULL;
static struct mem_s * plfree_ = NULL;  /* pointer to the lowest free block */
#endif

#ifdef SYS_MEMTHREADENABLED
static pthread_mutex_t memlock_;
#endif

void sysmem_init__()
{
	struct mem_s * mem;

	// sysmem
	mem = (struct mem_s *)sysmem_;
	mem->next = SYS_MEMSIZE;
	mem->prev = 0;
	mem->used = 0;
	
	plast_ = (struct mem_s *)&sysmem_[SYS_MEMSIZE];
	plast_->used = 1;
	plast_->next = SYS_MEMSIZE;
	plast_->prev = SYS_MEMSIZE;

	pfree_ = (struct mem_s *)sysmem_;

	// long life sysmem
#ifdef SYS_MEMLONGLIFEENABLED
	mem = (struct mem_s *)lsysmem_;
	mem->next = SYS_MEMSIZE;
	mem->prev = 0;
	mem->used = 0;
	
	pllast_ = (struct mem_s *)&lsysmem_[SYS_LMEMSIZE];
	pllast_->used = 1;
	pllast_->next = SYS_LMEMSIZE;
	pllast_->prev = SYS_LMEMSIZE;

	plfree_ = (struct mem_s *)lsysmem_;
#endif

#ifdef SYS_MEMTHREADENABLED
	pthread_mutexattr_t mutexattr;	
	pthread_mutexattr_init(&mutexattr);
	pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_ERRORCHECK);
	pthread_mutex_init(&memlock_, &mutexattr);
	pthread_mutexattr_destroy(&mutexattr);
#endif	
}

void sysmem_info__()
{
	struct mem_s * mem;
	mem = (struct mem_s *)sysmem_;	
	printf("\nsys-mem @ %p\n", sysmem_);
	printf("freemem @ %p/%u\n", pfree_, (unsigned int)((unsigned char *)pfree_ - sysmem_));
	printf("lastmem @ %p/%u\n", plast_, (unsigned int)((unsigned char *)plast_ - sysmem_));
	while(mem != plast_)
	{
		printf("\nallmem @ %u\n", (unsigned int)((unsigned char *)mem - sysmem_));
		printf("next=%u\n", mem->next);
		printf("prev=%u\n", mem->prev);
		printf("used=%u\n", mem->used);
		mem = ((struct mem_s *)&sysmem_[mem->next]);
	}
}

/////////////////////////////////////////
// sysmem

static void plug_holes(struct mem_s * mem)
{
	struct mem_s * nmem;
	struct mem_s * pmem;

	/* plug hole forward */
	nmem = (struct mem_s *)&sysmem_[mem->next];
  	if(mem != nmem && nmem->used == 0 && (unsigned char *)nmem != (unsigned char *)plast_) 
  	{
    	if(pfree_ == nmem)
      		pfree_ = mem;
    	mem->next = nmem->next;
    	((struct mem_s *)&sysmem_[nmem->next])->prev = (unsigned char *)mem - sysmem_;
  	}

	/* plug hole backward */
  	pmem = (struct mem_s *)&sysmem_[mem->prev];
  	if(pmem != mem && pmem->used == 0) 
  	{
    	if(pfree_ == mem)
      		pfree_ = pmem;
	    pmem->next = mem->next;
    	((struct mem_s *)&sysmem_[mem->next])->prev = (unsigned char *)pmem - sysmem_;
  	}
}

void * malloc__(int size)
{
  	unsigned int curr, next;
  	struct mem_s * pcur, * pnxt;

  	if(size == 0)
    	return NULL;

	/* Expand the size of the allocated memory region so that we can adjust for alignment. */
  	if((size % SYS_MEMALIGNMENT) != 0)
    	size += SYS_MEMALIGNMENT - ((size + SIZEOF_STRUCT_MEM) % SYS_MEMALIGNMENT);
  	
  	if(size > SYS_MEMSIZE)
    	return NULL;
  
#ifdef SYS_MEMTHREADENABLED
	pthread_mutex_lock(&memlock_);
#endif

  	for(curr = (unsigned char *)pfree_ - sysmem_; curr < SYS_MEMSIZE; curr = ((struct mem_s *)&sysmem_[curr])->next) 
  	{
    	pcur = (struct mem_s *)&sysmem_[curr];
    	
    	if(!pcur->used && pcur->next - (curr + SIZEOF_STRUCT_MEM) >= (unsigned int)size + SIZEOF_STRUCT_MEM) 
     	{
      		next = curr + SIZEOF_STRUCT_MEM + size;
      		pnxt = (struct mem_s *)&sysmem_[next];

      		pnxt->prev = curr;      
      		pnxt->next = pcur->next;
      		pcur->next = next; 
			
      		if(pnxt->next != SYS_MEMSIZE) 
        		((struct mem_s *)&sysmem_[pnxt->next])->prev = next;
      
      		pnxt->used = 0;      
      		pcur->used = 1;

			if(pcur == pfree_) 
			{
  				/* Find next free block after mem */
        		while(pfree_->used && pfree_ != plast_) 
        		{
    				pfree_ = (struct mem_s *)&sysmem_[pfree_->next];
        		}
      		}
      
		#ifdef SYS_MEMTHREADENABLED
			pthread_mutex_unlock(&memlock_);
		#endif
      		return (unsigned char *)pcur + SIZEOF_STRUCT_MEM;
      	}    
	}
#ifdef SYS_MEMTHREADENABLED
	pthread_mutex_unlock(&memlock_);
#endif	
	return NULL;
}

void free__(void * ptr)
{
	struct mem_s * mem;

	if(ptr == NULL) 
		return;

#ifdef SYS_MEMTHREADENABLED
	pthread_mutex_lock(&memlock_);
#endif

	if((unsigned char *)ptr < (unsigned char *)sysmem_ || (unsigned char *)ptr >= (unsigned char *)plast_) 
	{
	#ifdef SYS_MEMTHREADENABLED
		pthread_mutex_unlock(&memlock_);
	#endif
		return;
	}
	
	mem = (struct mem_s *)((unsigned char *)ptr - SIZEOF_STRUCT_MEM);
	mem->used = 0;

	if(mem < pfree_) 
		pfree_ = mem;

	plug_holes(mem);
#ifdef SYS_MEMTHREADENABLED
	pthread_mutex_unlock(&memlock_);
#endif
}

/////////////////////////////////////////
// sysmem

#ifdef SYS_MEMLONGLIFEENABLED

static void lplug_holes(struct mem_s * mem)
{
	struct mem_s * nmem;
	struct mem_s * pmem;

	/* plug hole forward */
	nmem = (struct mem_s *)&lsysmem_[mem->next];
  	if(mem != nmem && nmem->used == 0 && (unsigned char *)nmem != (unsigned char *)pllast_) 
  	{
    	if(plfree_ == nmem)
      		plfree_ = mem;
    	mem->next = nmem->next;
    	((struct mem_s *)&lsysmem_[nmem->next])->prev = (unsigned char *)mem - lsysmem_;
  	}

	/* plug hole backward */
  	pmem = (struct mem_s *)&lsysmem_[mem->prev];
  	if(pmem != mem && pmem->used == 0) 
  	{
    	if(plfree_ == mem)
      		plfree_ = pmem;
	    pmem->next = mem->next;
    	((struct mem_s *)&lsysmem_[mem->next])->prev = (unsigned char *)pmem - lsysmem_;
  	}
}

void * lmalloc__(int size)
{
  	unsigned int curr, next;
  	struct mem_s * pcur, * pnxt;

  	if(size == 0)
    	return NULL;

	/* Expand the size of the allocated memory region so that we can adjust for alignment. */
  	if((size % SYS_MEMALIGNMENT) != 0)
    	size += SYS_MEMALIGNMENT - ((size + SIZEOF_STRUCT_MEM) % SYS_MEMALIGNMENT);
  	
  	if(size > SYS_LMEMSIZE)
    	return NULL;
  
#ifdef SYS_MEMTHREADENABLED
	pthread_mutex_lock(&memlock_);
#endif

  	for(curr = (unsigned char *)plfree_ - lsysmem_; curr < SYS_LMEMSIZE; curr = ((struct mem_s *)&lsysmem_[curr])->next) 
  	{
    	pcur = (struct mem_s *)&lsysmem_[curr];
    	
    	if(!pcur->used && pcur->next - (curr + SIZEOF_STRUCT_MEM) >= (unsigned int)size + SIZEOF_STRUCT_MEM) 
     	{
      		next = curr + SIZEOF_STRUCT_MEM + size;
      		pnxt = (struct mem_s *)&lsysmem_[next];

      		pnxt->prev = curr;      
      		pnxt->next = pcur->next;
      		pcur->next = next; 
			
      		if(pnxt->next != SYS_MEMSIZE) 
        		((struct mem_s *)&lsysmem_[pnxt->next])->prev = next;
      
      		pnxt->used = 0;      
      		pcur->used = 1;

			if(pcur == plfree_) 
			{
  				/* Find next free block after mem */
        		while(plfree_->used && plfree_ != pllast_) 
        		{
    				plfree_ = (struct mem_s *)&lsysmem_[plfree_->next];
        		}
      		}
      
		#ifdef SYS_MEMTHREADENABLED
			pthread_mutex_unlock(&memlock_);
		#endif
      		return (unsigned char *)pcur + SIZEOF_STRUCT_MEM;
      	}    
	}
#ifdef SYS_MEMTHREADENABLED
	pthread_mutex_unlock(&memlock_);
#endif	
	return NULL;
}

void lfree__(void * ptr)
{
	struct mem_s * mem;

	if(ptr == NULL) 
		return;

#ifdef SYS_MEMTHREADENABLED
	pthread_mutex_lock(&memlock_);
#endif

	if((unsigned char *)ptr < (unsigned char *)lsysmem_ || (unsigned char *)ptr >= (unsigned char *)pllast_) 
	{
	#ifdef SYS_MEMTHREADENABLED
		pthread_mutex_unlock(&memlock_);
	#endif
		return;
	}
	
	mem = (struct mem_s *)((unsigned char *)ptr - SIZEOF_STRUCT_MEM);
	mem->used = 0;

	if(mem < plfree_) 
		plfree_ = mem;

	lplug_holes(mem);
#ifdef SYS_MEMTHREADENABLED
	pthread_mutex_unlock(&memlock_);
#endif
}

#else

void * lmalloc__(int size) { return malloc__(size); }
void lfree__(void * ptr)   { free__(ptr); }

#endif // SYS_MEMLONGLIFEENABLED

#endif
