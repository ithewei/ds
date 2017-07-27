#ifndef __OOK_MEM_H__
#define __OOK_MEM_H__

#ifdef OEM_MALLOC

void sysmem_init__();
void sysmem_info__();

extern void * malloc__(int size);
extern void free__(void * ptr);

extern void * lmalloc__(int size);
extern void lfree__(void * ptr);

#else

#define malloc__ malloc
#define free__   free

#define lmalloc__ malloc
#define lfree__   free

#endif

#endif
