/*
Copyright (C) 1998, 1999, 2000 Wabasoft

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.
*/

/*

This file is the starting point when porting the WabaVM to a new platform.

To port the WabaVM, you need to fill in platform dependent code in 3 files:

- nm<platform>_a.c
- nm<platform>_b.c
- nm<platform>_c.c

The nm stands for 'native methods'. The 3 files contain:

- code to deal with platform-specific type sizes and byte order
- code to allocate a few blocks of memory
- code to load a class from memory or disk
- the main application loop
- native functions

This file contains the code to deal with platform-specific types and
allocation of blocks of memory. To port the VM, read though the comments
and look for the 'WHEN PORTING' comments and fill in what you need to.

You'll probably want to take a look at waba.c as you go along to see
where the code is used. A WabaVM is comprised of the 3 native
function files and the VM code itself contained in waba.c

*/

// WHEN PORTING: define FREE_ON_EXIT if you want all object destructors
// to be called when the program exits. Under OS's that release system
// resources automatically, you don't need to do this. On OS's that don't
// automatically release system resources (images, memory, etc.) that
// native functions may allocate, you should set this to 1 so when
// the waba program exits, all native method object destructors get called
// to clean things up
#define FREE_ON_EXIT 1

// WHEN PORTING: You need to define the basic types of uchar, int32, etc.
// for the platform being ported to as follows:
//
// - uchar is an 8 bit unsigned value
// - int32 is a signed 32 bit value
// - uint32 is an unsigned 32 bit value
// - float32 is a signed 32 bit floating point value
// - int16 is a signed 16 bit value
// - uint16 is an unsigned 16 bit value
//
// below is an example from PalmOS

#undef FREE_ON_EXIT
#undef SECURE_CLASS_HEAP
#define LOCK_CLASS_HEAP
#define UNLOCK_CLASS_HEAP

#define uchar unsigned char
#define int32 int
#define uint32 unsigned int
#define float32 float
#define int16 short
#define uint16 unsigned short

#define MAX_CLASS    64
#define HASHTABLE_SIZE 100
#define AUTO_MALLOC  0
#define REWIND_CODE  1
#define COORDINATE   180
#define OUT_WINDOW   181
#define LEFT_DOWN    182
#define CENTER_DOWN  183
#define RIGHT_DOWN   184
#define LEFT_UP      185
#define CENTER_UP    186
#define RIGHT_UP     187
#define TIMER_SIGNAL 190
#define CLIP_LEFT    1
#define CLIP_RIGHT   2
#define CLIP_TOP     4
#define CLIP_BOTTOM  8
#define DRAW_OVER    1
#define DRAW_AND     2
#define DRAW_OR      3
#define DRAW_XOR     4
#define Font_PLAIN   0
#define Font_BOLD    1

#if defined(OSASK)
/* static */ struct LIB_WINDOW *window;
/* static */ struct LIB_TEXTBOX *wintitle;
#elif defined(MONA)
/* static */ _P<System::Drawing::Bitmap> bitmap;
#elif defined(MONACUI)
/* static */ Screen *screen;
/* static */ VirtualScreen *vscreen;
#elif defined(PEKOE)
/* static */ GSSTATUS gs;
/* static */ int globalTimerID = 0;
#elif defined(SDL)
/* static */ SDL_Surface *screen;
/* static */ SDL_TimerID globalTimerID;
#elif defined(NWSOS)
/* static */ HWND *g_hwndMain;
/* static */ BYTE *bitmap;
/* static */ int globalTimerID = 0;
#endif

// WHEN PORTING: You need to define functions that convert a string
// of 2 or 4 bytes in network byte order to a 16 or 32 bit value as follows:
//
// getUInt32() - converts 4 bytes to a unsigned 32 bit integer
// getUInt16() - converts 2 bytes to a unsigned 16 bit integer
// getInt32() - converts 4 bytes to a signed 32 bit integer
// getInt16() - converts 2 bytes to a signed 16 bit integer
//
// Here is an example from PalmOS, you can make these functions instead of
// a macro if you want the executable to be smaller

#define getUInt32(b) (uint32)( (uint32)((b)[0])<<24 | (uint32)((b)[1])<<16 | (uint32)((b)[2])<<8 | (uint32)((b)[3]) )
#define getUInt16(b) (uint16)(((b)[0]<<8)|(b)[1])
#define getInt32(b) (int32)( (uint32)((b)[0])<<24 | (uint32)((b)[1])<<16 | (uint32)((b)[2])<<8 | (uint32)((b)[3]) )
#define getInt16(b) (int16)(((b)[0]<<8)|(b)[1])
#define inGetUInt32(b) (uint32)( (uint32)((b)[3])<<24 | (uint32)((b)[2])<<16 | (uint32)((b)[1])<<8 | (uint32)((b)[0]) )
#define inGetUInt16(b) (uint16)( (uint16)((b)[1])<<8 | (uint16)((b)[0]) )

// WHEN PORTING: You need to define a function that converts 4 bytes in
// network byte order to a 32 bit floating point value
//
// Here is an example from PalmOS:

/* static */ float32 getFloat32(uchar *buf) {
	uint32 i;
	float32 f;

	// we need to make sure we're aligned before casting
	i = ((uint32)buf[0] << 24) | ((uint32)buf[1] << 16) | ((uint32)buf[2] << 8) | (uint32)buf[3];
	f = *((float32 *)&i);
	return f;
}

/* static */ void waba_itoa(int n, char s[]) {
	int k, i = 0, j;
	char tmp;
	k = 0;

	if (n < 0) {
		n = -1 * n;
		i = 1;
	}
	if (n == 0) {
		s[k++] = 0 + '0';
	}
	while (n!=0) {
		s[k++] = n%10 + '0';
		n = n/10;
	}
	if (i == 1) s[k++] = '-';
	s[k] = '\0';

	i = j = 0;
	while (s[j]) ++j;
	while (i < --j) {
		tmp = s[i];
		s[i++] = s[j];
		s[j] = tmp;
	}
}

/* static */ double f_intpart(double d, int *sf, char **bufp) {
	double e, f;
	int s, i;

	e = d/10.0;
	if (e < 1.0) {
		**bufp = i = (int) d;
		*bufp = *bufp + 1;
		f = (double) i;
		return f;
	} else if(e > 10.0) {
		f = f_intpart(e, sf, bufp);
		f *= 10.0;
	} else {
		/*
		 * The most significant digit
		 */
		i = (int) e;
		*sf -= 1;
		**bufp = i;
		*bufp = *bufp + 1;
		f = ((double) i) * 10.0;
	}
	d -= f;
	if((s = *sf) == 0) {
		i = 0;
	} else {
		i = (int) d;
	}
	**bufp = i;
	*bufp = *bufp + 1;
	f += (double) i;
	if(--s >= 0) {
		*sf = s;
	}
	return f;
}

/* static */ void waba_ftoa(double d, char *bp, int prec) {
	int  sf;      /* significant figure */
	int  less1;   /* 1:if d < 1.0, 0:others */
	int  i, j, k;
	int  pow10;
	char *dp;     /* digit part */
	char *fp;     /* fraction part */
	char *tp;
	char digits[100];

	if(prec < 0) {
		prec = 6;
	}
	if(d < 0) {
		*bp++ = '-';
		d = -d;
	}
	sf = 17;
	fp = dp = digits;
	pow10 = 0;
	/*
	 * decimal
	 */
	if(d == 0.0) {
		less1 = 1;
	} else if(d < 1.0) {
		less1 = 1;
		--pow10;
		while(d < 0.1) {
			--pow10;
			d *= 10;
			i = (int) d;
			*dp++ = i;
			d -= (float) i;
		}
	} else {
		less1 = 0;
		d -= f_intpart(d, &sf, &dp);
		fp = dp;
		pow10 = fp - digits - 1;
	}
	/*
	 * fraction
	 */
	for(j = sf, k = 0; j > 0; --j, k++) {
		d *= 10;
		i = (int) d;
		*dp++ = i;
		d -= (float) i;
	}
	while(k++ < prec) {
		*dp++ = 0;
	}
	/*
	 * round to the nearest number in the given precision.
	 */
	if(*(fp + prec) > 4) {
		for(tp = fp + prec - 1; tp >= digits; --tp) {
			if((*tp += (char)1) == 10) {
			    *tp = 0;
			} else {
			    break;
			}
		}
	}

	if(less1) {
		*bp++ = '0';
	} else {
		dp = digits;
		while(dp < fp) {
		    *bp++ = '0' + *dp++;
		}
	}
	*bp++ = '.';
	for(i = 0; i < prec; i++) {
		*bp++ = '0' + *fp++;
	}
	*bp = 0;
}

#if defined(OSASK)
/* static */ int *signalbox0;
/* static */ int *sig_ptr;
/* static */ int getsignalw() {
/* 0は返らない */
	int signal;
	lib_waitsignal(0x0001, 0, 0); /* シグナルが来るまで待つ */
	if (*sig_ptr == REWIND_CODE) {
		/* REWINDシグナルを受け取った */
		/* 直後の値の分だけシグナルを処理したことにして、ポインタを先頭に戻す */
		lib_waitsignal(0x0000, *(sig_ptr + 1), 0);
		sig_ptr = signalbox0;
	}
	signal = *sig_ptr++;
	lib_waitsignal(0x0000, 1, 0);
	return signal;
}

/* static */ int getparam() {
/* 通常よりも長いシグナルを受け取るための関数 */
	int param = *sig_ptr++;
	lib_waitsignal(0x0000, 1, 0);
	return param;
}
#endif

#if defined(SDL)
/* static */ Uint32 sdl_timer_callback(Uint32 interval, void *param)
	{
	SDL_Event event;
	
	SDL_RemoveTimer(globalTimerID);
	event.type = SDL_USEREVENT;
	return SDL_PushEvent(&event);
	}
#endif

/* static */ void waba_sleep(int32 duration) {
/* 一定時間待つ */
#if defined(OSASK)
	int signal;

	lib_settimer(0x01, 0x01c0); /* Kタン指摘 [OSASK 6496] ※バグがなくなればこれは不要 */
	lib_settimertime(0x32, 0x01c0, (duration % 1000) * 4294967, duration / 1000, 0);
	do {
		signal = getsignalw();
		/* この関数は0を返さないので0チェックは不要 */
		if (signal == COORDINATE) { /* マウス座標 */
			getparam(); /* xを読み捨てる */
			getparam(); /* yを読み捨てる */
		}
	} while (signal != TIMER_SIGNAL);
	/* TIMER_SIGNALが来たらタイマは自動的に休止状態になるのでsettimerの必要なし */
#elif defined(PEKOE)
	sys_sleep(duration);
#elif defined(SDL)
	SDL_Delay(duration);
	//globalTimerInterval = 0;
	//globalTimerID = SDL_AddTimer((uint32)duration, sdl_timer_callback, (void*)1);
#elif defined(NWSOS)
	SetTimer( g_hwndMain, 1, duration, NULL );
#endif
}

/* static */ void waba_exit(int code) {
/* アプリケーション終了 */
#if defined(OSASK)
	lib_close(code);
#elif (defined(MONA) || defined(MONACUI) || defined(PEKOE))
	exit(code);
#elif defined(SDL)
	SDL_Quit();
	exit(code);
#elif defined(NWSOS)
	DestroyWindow(g_hwndMain);
	PostMessage(g_hwndMain, WM_QUIT, 0, NULL);
	exit(code);
#endif
}

/* static */ void *waba_calloc(int size) {
	int i;
	char *p = (char*)malloc(size);
	if (p) {
		//memset(p, 0, size);
		for (i = 0; i < size; i++) p[i] = 0;
		return (void *)p; 
	}else{
		//malloc失敗
		waba_exit(1);
		return NULL;
	}
}

// WHEN PORTING: You need to define the following functions:
//
// xstrncmp(s1, s2, n) - compares 2 strings n bytes up to a 0 terminator
// xstrncpy(s1, s2, n) - copies 2 strings n bytes up to a 0 terminator
// xstrlen(s) - returns the length of a string
// xstrcat(dst, src) - concats one string to another
// xmemmove(dst, src, size) - copies size bytes from src to dst
// xmemzero(mem, len) - zeros out len bytes at location mem
//
// You could find the native functions that map to the above functions
// or (easier) just use the ones below. In any case, its probably best
// to use the ones below first and then you switch out after things
// are running in case the machines functions don't do exactly what
// is expected.

/* static */ int xstrncmp(char *s1, char *s2, uint32 n)
    {
    uint32 i;

    for (i = 0; i < n; i++)
        if (s1[i] != s2[i])
            return 1;
    return 0;
    }

/* static */ void xstrncpy(char *dst, char *src, uint32 n)
    {
    uint32 i;

    for (i = 0; i < n; i++)
        dst[i] = src[i];
    }

/* static */ uint32 xstrlen(char *s)
    {
    uint32 len;

    len = 0;
    while (*s != 0)
        {
        len++;
        s++;
        }
    return len;
    }

/* static */ void xstrcat(char *dst, char *src)
    {
    while (*dst != 0)
        dst++;
    while (*src != 0)
        *dst++ = *src++;
    *dst = 0;
    }

/* static */ void xmemmove(uchar *dst, uchar *src, uint32 size)
    {
    uint32 i;

    for (i = 0; i < size; i++)
        dst[i] = src[i];
    }

/* static */ void xmemzero(uchar *mem, uint32 len)
    {
    uint32 i;

    for (i = 0; i < len; i++)
        mem[i] = 0;
    }

// WHEN PORTING: You need to define a function that allocates a block
// of memory and a function that frees a block of memory. The WabaVM
// allocates 4 blocks of memory when it starts up and these methods
// are only used to allocate and free those 4 blocks of memory. When
// the VM is running, it does not call these functions. So, if you have
// a processor without any heap management, it should be rather easy
// to get the VM running. Just allocate 4 blocks of memory for it and
// return pointers to them below. You don't need a heap based malloc()
// implementation or anything to run with the WabaVM. The VM does its
// own memory management within the 4 blocks.
//
// The two functions you need to implement are:
//
// xmalloc(size) - allocates size bytes of memory and returns a pointer
// xfree(ptr) - frees the memory allocated previously
//
// On platforms that have malloc() and free, we can simply use them as
// follows:

#define xmalloc(size) waba_calloc(size);

#define xfree(ptr) free(ptr)

// WHEN PORTING: That's it for the types and memory allocation, the next
// step is to implement the class loader and native functions in the
// nm<platform>_b.c and nm<platform>_c.c files
