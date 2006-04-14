
typedef char			MPEGERR;

enum {
	MPEGERR_USERCANCEL		= -6,
	MPEGERR_UNKNOWNFILE		= -5,
	MPEGERR_MEMORYERR		= -4,
	MPEGERR_NONFUNCTION		= -3,
	MPEGERR_NONINIT			= -2,
	MPEGERR_FAILURE			= -1,
	MPEGERR_SUCCESS			= 0,
	MPEGERR_NEXTSLICE		= 1,
	MPEGERR_NEXTPIC			= 2,
	MPEGERR_ENDOFCODE		= 3
};

typedef struct {
	int		width;
	int		height;
	BYTE	*yptr;
	BYTE	*uptr;
	BYTE	*vptr;
	int		yalign;
	int		uvalign;
	UINT32	tick;
} MPEGDRAW;

typedef struct {
	int		structsize;
	MPEGERR	(*task)(void *hdl);
	MPEGERR	(*draw)(void *hdl, const MPEGDRAW *drawhdl);
	UINT32	(*tick)(void *hdl);
	UINT	(*read)(void *hdl, void *buf, UINT size);
} MPEGFUNC;


#ifdef __cplusplus
extern "C" {
#endif

MPEGERR mpegdec_init(void);
MPEGERR mpegdec_term(void);
MPEGERR mpegdec(void *hdl, MPEGFUNC *func);

void *mpgaudio_open(void *hdl, UINT (*read)(void *hdl, void *buf, UINT size));
void mpgaudio_close(void *mp2);
UINT mpgaudio_read(void *mp2, void *buf, UINT size);

#ifdef __cplusplus
}
#endif

