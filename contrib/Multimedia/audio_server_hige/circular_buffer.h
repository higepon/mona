#ifndef _CIRCULAR_BUFFER_H_
#define _CIRCULAR_BUFFER_H_

#define CB_DEFAULT_BLOCKSIZE 4
#define CB_DEFAULT_MAX_BLOCKS 10
#define CB_SYSTEM_MAX_MEMORY_SIZE (1024*16)

#define CB_OVERWRITE 0x01

typedef struct _circular_buffer
{
	int blocksize;
	int maxblocks;
	int fi;
	int ei;
	char *p;
	int mutex;
    int semaphore;
} CB;

int cb_index_inc(int x, int limit);
int cb_index_dec(int x, int limit);

CB *cb_alloc();
CB *cb_init(CB *cb, int blocksize, int maxblocks);
int cb_free(CB *cb);
int cb_write(CB *cb, void *p, int flag);
int cb_read(CB *cb, void *p);

#endif

