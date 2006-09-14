#include "compiler.h"
#include "len.h"

#include <baygui.h>

#define SCREEN_W 320
#define SCREEN_H 240

// ファイル名
char* filename;

// クラス宣言
class XMonapeg : public Frame
{
public:
	// コンストラクタ
	XMonapeg()
	{
		setBounds((800 - SCREEN_W - 12) / 2, (600 - SCREEN_H - 28) / 2, SCREEN_W + 12, SCREEN_H + 28);
		setTitle(filename);
	}

	// 内部描画領域のバッファーを返す
	dword* getImageData()
	{
		return this->_buffer->getSource();
	}
};

static XMonapeg* xmonapeg;

// ---- lenのコールバック

typedef struct
{
	UINT	draw;
	UINT32	tickbase;
}
MVCTRL;

typedef struct
{
	BYTE	*mem;
	UINT	pos;
	UINT	size;
}
MVDATA;

typedef struct
{
	MVCTRL	ctrl;
	MVDATA	data;
	BYTE	rgblimit[256+256+256];
}
_MVARG, *MVARG;

static void setrgblimit(BYTE *rgblimit)
{
	BYTE	*p;
	int		i;

	ZeroMemory(rgblimit, 256);
	p = rgblimit + 256;
	i = 0;
	do
	{
		*p++ = (BYTE)i;
	}
	while (++i < 256);
	FillMemory(p, 256, 0xff);
}

static MPEGERR mpegtask(void *hdl)
{
	(void)hdl;
	return(MPEGERR_SUCCESS);
}

static MPEGERR mpegdraw(void *hdl, const MPEGDRAW *drawhdl)
{
	BYTE	*p[3], *q;
	BYTE	*baseptr;

	MVARG mv = (MVARG)hdl;
	baseptr = (BYTE *) xmonapeg->getImageData();
	for (int y = 0; y < SCREEN_H; y++)
	{
		p[0] = drawhdl->yptr + (drawhdl->yalign * y);
		p[1] = drawhdl->uptr + (drawhdl->uvalign * (y / 2));
		p[2] = drawhdl->vptr + (drawhdl->uvalign * (y / 2));
		q = baseptr + (y * SCREEN_W * 4);
		for (int x = 0; x < SCREEN_W; x++)
		{
			int yy, uu, vv;
			yy = p[0][0] * 4096;
			uu = (p[1][0] - 128);
			vv = (p[2][0] - 128);
			q[0] = mv->rgblimit[((yy + uu * 7257) >> 12) + 256];
			q[1] = mv->rgblimit[((yy - uu * 1409 - vv * 2924) >> 12) + 256];
			q[2] = mv->rgblimit[((yy + vv * 5743) >> 12) + 256];
			q += 4; // 4byte
			p[0]++;
			p[1] += (x & 1);
			p[2] += (x & 1);
		}
	}
	xmonapeg->update();
	if (!mv->ctrl.draw)
	{
		mv->ctrl.draw = TRUE;
		mv->ctrl.tickbase = syscall_get_tick() * 10;
	}
	else
	{
		while ((UINT32)(syscall_get_tick() * 10 - mv->ctrl.tickbase) < drawhdl->tick)
		{
			MPEGERR r;
			r = mpegtask(hdl);
			if (r != MPEGERR_SUCCESS)
			{
				return(r);
			}
		}
	}
	return(MPEGERR_SUCCESS);
}

static UINT32 mpegtick(void *hdl)
{
	if (((MVARG)hdl)->ctrl.draw)
	{
		return(syscall_get_tick() * 10 - ((MVARG)hdl)->ctrl.tickbase);
	}
	return(0);
}

static UINT mpegread(void *hdl, void *buf, UINT size)
{

	MVARG	mv;
	UINT	remain;
	UINT	readsize;

	mv = (MVARG)hdl;
	remain = mv->data.size - mv->data.pos;
	readsize = (remain < size) ? remain : size;
	if (readsize)
	{
		CopyMemory(buf, mv->data.mem + mv->data.pos, readsize);
		mv->data.pos += readsize;
	}
	return(readsize);
}

static dword my_tid, draw_tid;

static void DrawLoop()
{
	UINT		filesize;
	BYTE		*filebuf;
	_MVARG		mv;
	MPEGFUNC	func;
	MPEGERR		r;

	MonAPI::Message::send(my_tid, MSG_SERVER_START_OK);

	printf("loading %s...\n", filename);
	monapi_cmemoryinfo* mi = monapi_file_read_all(filename);
	if (mi == NULL || mi->Size == 0)
	{
		printf("file read error\n");
		return;
	}
	filesize = mi->Size;
	filebuf = (BYTE *)malloc(mi->Size);
	if (filebuf == NULL)
	{
		printf("memory allocate error\n");
		monapi_cmemoryinfo_dispose(mi);
		monapi_cmemoryinfo_delete(mi);
		return;
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
	mv.ctrl.draw = FALSE;
	mv.data.pos = 0;
	r = mpegdec(&mv, &func);
	mpegdec_term();

	free(filebuf);

	syscall_kill_thread(draw_tid);
}

// ---- めいん
int MonaMain( List<char*>* pekoe )
{
	// check arguments
	if (pekoe->size() < 1)
	{
		printf("usage: XMONAPEG.EX5 [filename.mpg]\n");
		return(-1);
	}
	else
	{
		filename = pekoe->get(0);
	}

	// Create thread
	my_tid = syscall_get_tid();
	syscall_mthread_join(syscall_mthread_create((dword)DrawLoop));
	MessageInfo msg, src;
	src.header = MSG_SERVER_START_OK;
	MonAPI::Message::receive(&msg, &src, MonAPI::Message::equalsHeader);
	draw_tid = msg.from;

	// Create frame
	xmonapeg = new XMonapeg();
	xmonapeg->run();

	return 0;
}
