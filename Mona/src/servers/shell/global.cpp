#include	"compiler.h"
#include	"global.h"

// なんでここまでやる必要があるんだ？

// memmap_create	->	globalalloc
// memmap_map		->	globallock
// memmap_unmap		->	globalfree
// memmap_getsize	->	globalsize

#define	START_ADDRESS		0x90000000
#define	MAX_SIZE			0x10000000
#if defined(SYSTEM_PSE)						// 無理言うな…
#define	PAGE_SIZE			(1 << 22)
#elif defined(SYSTEM_PAE_PDE)				// 無茶言うな…
#define	PAGE_SIZE			(1 << 21)
#elif defined(SYSTEM_AMD64)					// 本気で助けてくれ…
#define	PAGE_SIZE			"Help Sakkyun!"
#else										// 動的振り分けだったらどーするの
#define	PAGE_SIZE			(1 << 12)
#endif
#define	MAX_PAGES			(MAX_SIZE / PAGE_SIZE)

// つか、SYSTEM_PSEだと 最低8MB確保するね
// 10年後にお会いしましょう、お元気で！

#define	PAGEALIGN(s)	(((s) + (PAGE_SIZE - 1)) & (~(PAGE_SIZE - 1)))
#define	PAGEPOSITION(s)	((s) / PAGE_SIZE)
#define	PAGECOUNT(s)	(((s) + (PAGE_SIZE - 1)) / PAGE_SIZE)


typedef struct _ghlist	_GHLIST;
typedef struct _ghlist	*GHLIST;

typedef struct {
	dword	hdl;
	dword	locked;
	void	*addr;
	dword	size;
} _GHITEM, *GHITEM;

struct _ghlist {
	GHLIST	next;
	dword	self;
	dword	count;
	dword	padding;
};

static	dword	__maphdl;
static	GHLIST	__ghlist;
#if defined(_MT)
static	int		__csec;
#endif


// ----

static bool mapset(void *ptr, dword size, bool set) {

	dword	pos;
	byte	*map;

	pos = (dword)(long)ptr - START_ADDRESS;
	size = pos + size;
	pos = PAGEPOSITION(pos);
	if (pos >= MAX_PAGES) {
		return(false);
	}
	size = PAGECOUNT(size);
	size = min(size, MAX_PAGES);
	size -= pos;
	if (size == 0) {
		return(false);
	}
	map = (byte *)START_ADDRESS;
	if (set) {
		do {
			map[pos >> 3] |= (1 << (pos & 7));
			pos++;
		} while(--size);
	}
	else {
		do {
			map[pos >> 3] &= ~(1 << (pos & 7));
			pos++;
		} while(--size);
	}
	return(true);
}

static void *mapsearchfree(dword size) {

	byte	*map;
	dword	pos;
	dword	nxt;

	size = PAGECOUNT(size);
	if (size == 0) {
		return(NULL);
	}
	map = (byte *)START_ADDRESS;
	pos = 0;
	while(1) {
		while((pos < MAX_PAGES) && ((map[pos >> 3] >> (pos & 7)) & 1)) {
			pos++;
		}
		if (pos >= MAX_PAGES) {
			return(NULL);
		}
		nxt = pos;
		do {
			nxt++;
		} while((nxt < MAX_PAGES) && (!((map[nxt >> 3] >> (nxt & 7)) & 1)));
		if ((nxt - pos) >= size) {
			break;
		}
		pos = nxt;
	}
	return(map + (pos * PAGE_SIZE));
}

static GHLIST ghlistnew(GHLIST ref) {

	dword	hdl;
	GHLIST	ret;

	hdl = syscall_memory_map_create(PAGE_SIZE);
	if (hdl == 0) {
		goto en_err1;
	}
	ret = (GHLIST)mapsearchfree(PAGE_SIZE);
	if (ret == NULL) {
		goto en_err2;
	}
	syscall_memory_map_map(hdl, (dword)ret);
	memset(ret, 0, PAGE_SIZE);
	mapset(ret, PAGE_SIZE, true);
	if (ref) {
		ref->next = ret;
	}
	ret->self = hdl;
	ret->count = (PAGE_SIZE - sizeof(_GHLIST)) / sizeof(_GHITEM);
	return(ret);

en_err2:
	syscall_memory_map_unmap(hdl);

en_err1:
	return(NULL);
}

static GHITEM ghitemsearch(dword hdl) {

	GHLIST	ptr;
	GHITEM	item;
	dword	i;

	ptr = __ghlist;
	do {
		item = (GHITEM)(ptr + 1);
		for (i=0; i<ptr->count; i++) {
			if (item[i].hdl == hdl) {
				return(item);
			}
		}
		if ((ptr->next != NULL) || (hdl != 0)) {
			ptr = ptr->next;
		}
		else {
			ptr = ghlistnew(ptr);
		}
	} while(ptr);
	return(NULL);
}

BOOL globalinitialize(void) {

	dword	size;
	dword	hdl;
	byte	*ptr;
	GHLIST	list;

#if defined(_MT)
	__csec = syscall_mutex_create();
#endif
	size = PAGEALIGN(MAX_PAGES >> 3);
	hdl = syscall_memory_map_create(size);
	if (hdl == 0) {
		goto init_err1;
	}
	ptr = (byte *)(long)START_ADDRESS;
	syscall_memory_map_map(hdl, (dword)ptr);
	memset(ptr, 0, size);
	mapset(ptr, size, true);
	size = size - (MAX_PAGES >> 3);
	if (size >= (sizeof(_GHLIST) + sizeof(_GHITEM))) {
		list = (GHLIST)(ptr + (MAX_PAGES >> 3));
		list->count = (size - sizeof(_GHLIST)) / sizeof(_GHITEM);
	}
	else {
		list = ghlistnew(NULL);
	}
	if (list == NULL) {
		goto init_err2;
	}
	__maphdl = hdl;
	__ghlist = list;
	return(SUCCESS);

init_err2:
init_err1:
	return(FAILURE);
}

#if defined(_MT)
static void globalenter(void) {

	syscall_mutex_lock(__csec);
}

static void globalleave(void) {

	syscall_mutex_unlock(__csec);
}
#else
#define	globalenter()		((void)0)
#define	globalleave()		((void)0)
#endif


// ---- func

GLOBALH globalalloc(dword memhdl, dword size) {

	GHITEM	ret;

	if (memhdl) {
		size = syscall_memory_map_get_size(memhdl);
	}
	size = PAGEALIGN(size);
	if (!size) {
		return(NULL);
	}
	globalenter();
	ret = ghitemsearch(0);
	if (ret) {
		if (memhdl == 0) {
			memhdl = syscall_memory_map_create(size);
		}
		if (memhdl != 0) {
			ret->hdl = memhdl;
			ret->locked = 0;
			ret->addr = NULL;
			ret->size = size;
		}
		else {
			ret = NULL;
		}
	}
	globalleave();
	return((GLOBALH)ret);
}

void *globallock(GLOBALH hdl) {

	void	*ret;

	if (hdl == NULL) {
		return(NULL);
	}
	ret = NULL;
	globalenter();
	if (((GHITEM)hdl)->locked) {
		ret = ((GHITEM)hdl)->addr;
	}
	else {
		ret = mapsearchfree(((GHITEM)hdl)->size);
		if (ret == NULL) {
			goto gl_err;
		}
		syscall_memory_map_map(((GHITEM)hdl)->hdl, (dword)ret);
		((GHITEM)hdl)->addr = ret;
		mapset(ret, ((GHITEM)hdl)->size, true);
	}
	((GHITEM)hdl)->locked++;

gl_err:
	globalleave();
	return(ret);
}

BOOL globalunlock(GLOBALH hdl) {

	if (hdl == NULL) {
		return(FAILURE);
	}
	globalenter();
	if (((GHITEM)hdl)->locked) {
		((GHITEM)hdl)->locked--;
	}
	globalleave();
	return(SUCCESS);
}

BOOL globalfree(GLOBALH hdl) {

	if (hdl == NULL) {
		return(FAILURE);
	}
	globalenter();
	syscall_memory_map_unmap(((GHITEM)hdl)->hdl);
	((GHITEM)hdl)->hdl = 0;
	mapset(((GHITEM)hdl)->addr, ((GHITEM)hdl)->size, false);
	globalleave();
	return(SUCCESS);
}

dword globalsize(GLOBALH hdl) {

	if (hdl) {
		return(((GHITEM)hdl)->size);
	}
	else {
		return(0);
	}
}

