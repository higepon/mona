
typedef void *		GLOBALH;

BOOL globalinitialize(void);
GLOBALH globalalloc(dword memid = 0, dword size = 0);
void *globallock(GLOBALH hdl);
BOOL globalunlock(GLOBALH hdl);
BOOL globalfree(GLOBALH hdl);
dword globalsize(GLOBALH hdl);

#define	memmap_create(s)	globalalloc(s)
#define	memmap_map(h)		((byte *)globallock(h))
#define	memmap_unmap(h)		globalfree(h)
#define	memmap_getsize(h)	globalsize(h)

