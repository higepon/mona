#include	"compiler.h"
#include	"len.h"


// ---- lenのコールバック

typedef struct {
	UINT	draw;
	UINT32	tickbase;
} MVCTRL;

typedef struct {
	BYTE	*mem;
	UINT	pos;
	UINT	size;
} MVDATA;

typedef struct {
	MVCTRL	ctrl;
	MVDATA	data;
	BYTE	rgblimit[256+256+256];
} _MVARG, *MVARG;

static void setrgblimit(BYTE *rgblimit) {

	BYTE	*p;
	int		i;

	ZeroMemory(rgblimit, 256);
	p = rgblimit + 256;
	i = 0;
	do {
		*p++ = (BYTE)i;
	} while(++i < 256);
	FillMemory(p, 256, 0xff);
}

static MPEGERR mpegtask(void *hdl) {

#if 0
	SDL_Event	ev;

	if ((SDL_PollEvent(&ev)) && (ev.type == SDL_MOUSEBUTTONDOWN)) {
		return(MPEGERR_USERCANCEL);
	}
#endif
	(void)hdl;
	return(MPEGERR_SUCCESS);
}

static MonAPI::Screen surface;

static MPEGERR mpegdraw(void *hdl, const MPEGDRAW *drawhdl) {

	MVARG			mv;
	int				xalign, yalign;
	int				x, y;
	BYTE			*p[3], *q;
	int				width, height, sw, sh, ox, oy;
	BYTE			*baseptr;

	mv = (MVARG)hdl;
	xalign = surface.getBpp() >> 3;
	yalign = 0;
	width = (surface.getWidth() < drawhdl->width) ? surface.getWidth() : drawhdl->width;
	height = (surface.getHeight() < drawhdl->height) ? surface.getHeight() : drawhdl->height;
	sw = surface.getWidth();
	sh = surface.getHeight();
	ox = (sw - width) >> 1;
	oy = (sh - height) >> 1;
	baseptr = (BYTE *)surface.getVRAM();
	if (xalign == 2) {	// 16bpp
		for (y=0; y<height; y++) {
			p[0] = drawhdl->yptr + (drawhdl->yalign * y);
			p[1] = drawhdl->uptr + (drawhdl->uvalign * (y / 2));
			p[2] = drawhdl->vptr + (drawhdl->uvalign * (y / 2));
			//q = baseptr + (y * yalign);
			q = &baseptr[(ox + (y + oy) * sw) << 1];
			for (x=0; x<width; x++) {
				int yy, uu, vv;
				UINT32 col;
				yy = p[0][0] * 4096;
				uu = (p[1][0] - 128);
				vv = (p[2][0] - 128);
				col = mv->rgblimit[((yy + uu * 7257) >> 12) + 256] >> 3;
				col += (mv->rgblimit[((yy - uu * 1409 - vv * 2924) >> 12)
														+ 256] & 0xfc) << 3;
				col += (mv->rgblimit[((yy + vv * 5743) >> 12) + 256]
															& 0xf8) << 8;
				*(UINT16 *)q = (UINT16)col;
				q += 2;
				p[0]++;
				p[1] += (x & 1);
				p[2] += (x & 1);
			}
		}
	}
	if (!mv->ctrl.draw) {
		mv->ctrl.draw = TRUE;
		mv->ctrl.tickbase = syscall_get_tick() * 10;
	} else {
		while((UINT32)(syscall_get_tick() * 10 - mv->ctrl.tickbase) < drawhdl->tick) {
			MPEGERR r;
			r = mpegtask(hdl);
			if (r != MPEGERR_SUCCESS) {
				return(r);
			}
		}
	}
#if 0
	MVARG			mv;
	SDL_Surface		*surface;
	int				xalign, yalign;
	int				x, y;
	BYTE			*p[3], *q;
	int				width, height;
	BYTE			*baseptr;

	mv = (MVARG)hdl;
	surface = SDL_GetVideoSurface();
	if (surface == NULL) {
		return(MPEGERR_FAILURE);
	}
	xalign = surface->format->BytesPerPixel;
	yalign = surface->pitch;
	width = min(surface->w, drawhdl->width);
	height = min(surface->h, drawhdl->height);
	SDL_LockSurface(surface);
	baseptr = (BYTE *)surface->pixels +
			(((surface->w - width) >> 1) * xalign) +
			(((surface->h - height) >> 1) * yalign);
	if (xalign == 2) {	// 16bpp
		for (y=0; y<height; y++) {
			p[0] = drawhdl->yptr + (drawhdl->yalign * y);
			p[1] = drawhdl->uptr + (drawhdl->uvalign * (y / 2));
			p[2] = drawhdl->vptr + (drawhdl->uvalign * (y / 2));
			q = baseptr + (y * yalign);
			for (x=0; x<width; x++) {
				int yy, uu, vv;
				UINT32 col;
				yy = p[0][0] * 4096;
				uu = (p[1][0] - 128);
				vv = (p[2][0] - 128);
				col = mv->rgblimit[((yy + uu * 7257) >> 12) + 256] >> 3;
				col += (mv->rgblimit[((yy - uu * 1409 - vv * 2924) >> 12)
														+ 256] & 0xfc) << 3;
				col += (mv->rgblimit[((yy + vv * 5743) >> 12) + 256]
															& 0xf8) << 8;
				*(UINT16 *)q = (UINT16)col;
				q += 2;
				p[0]++;
				p[1] += (x & 1);
				p[2] += (x & 1);
			}
		}
	}
	else {				// 24,32bpp
		for (y=0; y<height; y++) {
			p[0] = drawhdl->yptr + (drawhdl->yalign * y);
			p[1] = drawhdl->uptr + (drawhdl->uvalign * (y / 2));
			p[2] = drawhdl->vptr + (drawhdl->uvalign * (y / 2));
			q = baseptr + (y * yalign);
			for (x=0; x<width; x++) {
				int yy, uu, vv;
				yy = p[0][0] * 4096;
				uu = (p[1][0] - 128);
				vv = (p[2][0] - 128);
				q[0] = mv->rgblimit[((yy + uu * 7257) >> 12) + 256];
				q[1] = mv->rgblimit[((yy - uu * 1409 - vv * 2924) >> 12)
																	+ 256];
				q[2] = mv->rgblimit[((yy + vv * 5743) >> 12) + 256];
				q += xalign;
				p[0]++;
				p[1] += (x & 1);
				p[2] += (x & 1);
			}
		}
	}
	SDL_UnlockSurface(surface);
	SDL_Flip(surface);
	if (!mv->ctrl.draw) {
		mv->ctrl.draw = TRUE;
		mv->ctrl.tickbase = SDL_GetTicks();
	}
	else {
		while((UINT32)(SDL_GetTicks() - mv->ctrl.tickbase) < drawhdl->tick) {
			MPEGERR r;
			r = mpegtask(hdl);
			if (r != MPEGERR_SUCCESS) {
				return(r);
			}
		}
	}
#endif
	return(MPEGERR_SUCCESS);
}

static UINT32 mpegtick(void *hdl) {

#if 1
	if (((MVARG)hdl)->ctrl.draw) {
		return(syscall_get_tick() * 10 - ((MVARG)hdl)->ctrl.tickbase);
	}
#endif
	return(0);
}

static UINT mpegread(void *hdl, void *buf, UINT size) {

	MVARG	mv;
	UINT	remain;
	UINT	readsize;

	mv = (MVARG)hdl;
	remain = mv->data.size - mv->data.pos;
	readsize = (remain < size) ? remain : size;
	if (readsize) {
		CopyMemory(buf, mv->data.mem + mv->data.pos, readsize);
		mv->data.pos += readsize;
	}
	return(readsize);
}


// ---- めいん
int MonaMain( List<char*>* pekoe ) {
	UINT			filesize;
	BYTE			*filebuf;
	_MVARG			mv;
	MPEGFUNC		func;
	MPEGERR			r;
	monapi_cmemoryinfo* mi = NULL;

	// check arguments
	if (pekoe->size() < 1) {
		printf("usage: MONAPEG.EX2 [filename.mpg]\n");
		return(-1);
	}

	printf("loading %s...\n", pekoe->get(0));
	mi = monapi_file_read_all(pekoe->get(0));
	if (mi == NULL || mi->Size == 0) {
		printf("file read error\n");
		return(-1);
	}
	filesize = mi->Size;
	filebuf = (BYTE *)malloc(mi->Size);
	if (filebuf == NULL) {
		printf("memory allocate error\n");
		monapi_cmemoryinfo_dispose(mi);
		monapi_cmemoryinfo_delete(mi);
		return(-1);
	}
	memcpy(filebuf, mi->Data, mi->Size);
	monapi_cmemoryinfo_dispose(mi);
	monapi_cmemoryinfo_delete(mi);

	mv.data.mem = filebuf;
	mv.data.pos = 0;
	mv.data.size = filesize;

	setrgblimit(mv.rgblimit);

	func.structsize = sizeof(func);
	func.task = mpegtask;
	func.draw = mpegdraw;
	func.tick = mpegtick;
	func.read = mpegread;

	mpegdec_init();
	//loop playing
	//do {
		mv.ctrl.draw = FALSE;
		mv.data.pos = 0;
		r = mpegdec(&mv, &func);
	//} while(r > 0);
	mpegdec_term();

	free(filebuf);
	return(0);
}
#if 0
int SDL_main(int argc, char **argv) {

	HANDLE			fh;
const SDL_VideoInfo	*vinfo;
	SDL_Surface		*surface;
	UINT			filesize;
	BYTE			*filebuf;
	UINT			readsize;
	_MVARG			mv;
	MPEGFUNC		func;
	MPEGERR			r;
	char			path[40];			// MAX_PATH = 32 !

	// check arguments
	if (argc < 2) {
		printf("usage: MONAPEG.ELF [filename.mpg]\n");
		return(-1);
	}

	if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
		printf("Error: SDL_Init: %s\n", SDL_GetError());
		return(-1);
	}
	vinfo = SDL_GetVideoInfo();
	if (vinfo == NULL) {
		printf("Error: SDL_GetVideoInfo: %s\n", SDL_GetError());
		return(FAILURE);
	}
	if (vinfo->vfmt->BitsPerPixel < 16) {
		printf("sorry, require 16/24/32bpp Video mode\n");
		return(-1);
	}
	surface = SDL_SetVideoMode(640, 480, vinfo->vfmt->BitsPerPixel,
		    SDL_HWSURFACE | SDL_ANYFORMAT | SDL_DOUBLEBUF | SDL_FULLSCREEN);
	if (surface == NULL) {
		printf("Error: SDL_SetVideoMode: %s\n", SDL_GetError());
		return(-1);
	}

	printf("loading %s...\n", argv[1]);
	fh = file_open(argv[1]);
	if (fh == INVALID_HANDLE) {
		strcpy(path, argv[1]);
		strcat(path, ".MPG");
		fh = file_open(path);
	}
	if (fh == INVALID_HANDLE) {
		printf("%s: couldn't open.\n", argv[1]);
		return(-1);
	}

	filesize = file_getsize(fh);
	filebuf = (BYTE *)malloc(filesize);
	if (filebuf == NULL) {
		printf("memory allocate error\n");
		file_close(fh);
		return(-1);
	}
	readsize = file_read(fh, filebuf, filesize);
	file_close(fh);
	if (readsize != filesize) {
		printf("file read error\n");
		free(filebuf);
		return(-1);
	}

	mv.data.mem = filebuf;
	mv.data.pos = 0;
	mv.data.size = filesize;

	setrgblimit(mv.rgblimit);

	func.structsize = sizeof(func);
	func.task = mpegtask;
	func.draw = mpegdraw;
	func.tick = mpegtick;
	func.read = mpegread;

	mpegdec_init();
	do {
		mv.ctrl.draw = FALSE;
		mv.data.pos = 0;
		r = mpegdec(&mv, &func);
	} while(r > 0);
	mpegdec_term();

	free(filebuf);
	return(0);
}
#endif
