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

If you're looking here, you've probably already looked at (or worked on) the
nm<platform>_a.c file.

This file is the second step in porting the VM. In it, you need to place the
main application loop and class loader. If you get through this, you'll have
a basic VM running and from there you can implement the native functions
so you can actually see something on the screen->

The WabaVM, like other applications, needs some type of main application loop
where the OS sends it key clicks, mouse presses, etc. The VM also needs to
load classes from memory or disk or wherever.

Different platforms have different main application loops. For example, under
Win32, a program has a WinMain() function. Under PalmOS, applications have
a PilotMain() function.

You'll want to implement a main application loop for the platform you are
porting to. Then, when things start up (initialization), you need to:

- parse the launch command and then call VmInit()
- call VmStartApp passing the class name of the program's main window

You'll also need to figure out how programs will launch the VM. The VM
is normally passed parameters telling it what class heap, class name, etc.
to use. It's usually another program that does this but you could do it
by building a waba launcher that draws a tray of icons and when you click
one, it runs the WabaVM with various parameters as an example of something
different. Whatever you do for this is platform-specific.

Just before the program exits, you'll should call VmStopApp(mainWinObj);

That's it for initialization and exit, then you need to do some work to
hook up key presses, mouse presses, etc. to the VM. When a key is pressed,
you'll want to invoke the main window's onEvent() method. Here's an
example of how to do that:

	mainWinObj = globalMainWin;
	if (!mainWinObj)
		return;
	vclass = WOBJ_class(mainWinObj); // get runtime class
	method = getMethod(vclass, createUtfString("_postEvent"),
		createUtfString("(IIIIII)V"), &vclass);
	if (method != NULL)
		{
		params[0].obj = mainWinObj;
		params[1].intValue = type; // type
		params[2].intValue = key; // key
		params[3].intValue = 0; // x
		params[4].intValue = 0; // y
		params[5].intValue = 0; // modifiers
		params[6].intValue = 0; // timeStamp
		executeMethod(vclass, method, params, 7);
		}

This gets the runtime class of the main window object, gets a
reference to the _postEvent method, fills in an array of parameters
to pass and then calls executeMethod() to execute the method.

There is also a quickbind version of this which is quicker than the
above. You can look at nmpalm_b.c or nmwin32_b.c to get an idea how
that works (look for the #ifdef QUICKBIND) but you should start
without using the quickbind method.

You'll want to hook up mouse or pen click, key presses, calls from
the OS to repaint the window (which should call the main window's
_doPaint routine).

Overall, you need to make sure the following main window functions
get called:

_onTimerTick()
_postEvent() for key presses and mouse/pen clicks and moves

You'll need to map special keys (escape, etc.) if the device has
them to suitable numbers for waba. To do this, you can see how the
other native implementations do key mapping (a big switch statement).

The _onTimerTick() one can be tricky for devices that don't have
a built in timer (like PalmOS). When using one of those devices,
copy the code from the nmpalm_b.c to deal with timers. Basically,
this consists of getting the current time and looking for an event
up until the time the next timer period expires.

The VM only needs a single timer, the waba core classes have the
code to support multiple timers even though the underlying system
only supports a single one.

The last code you need to implement is:

nativeLoadClass()

This is the function that load a class from memory, disk or whatever.
It looks like this:

 static uchar *nativeLoadClass(UtfString className, uint32 *size)

and it is passed a class name (not zero terminated, the UtfString
contains the len in:

className.len

and it returns a pointer to the class in memory after it is loaded
and fills in the *size variable with the size of the class in bytes.

The classes loaded are classes that are compiled by a java compiler
or other compiler that can generate code in the subset of java
bytecode that waba supports.

*/

#if defined(OSASK)
/* static */ struct LIB_WINDOW *window;
/* static */ struct LIB_GRAPHBOX *gbox;
/* static */ struct LIB_TEXTBOX *wintitle;
/* static */ int *signalbox0;
/* static */ int *sig_ptr;
#elif defined(MONA)
/* static */ //Screen screen;
/* static */ _P<System::Drawing::Bitmap> image;
/* static */ WObject mainWinObj;
/* static */ WClassMethod *method_event, *method_paint;
#endif
/* static */ int g_mainWinTimerId = 0;
/* static */ int g_mainWinCX0 = g_mainWinOffX;
/* static */ int g_mainWinCY0 = g_mainWinOffY;
/* static */ int g_mainWinCX1 = g_mainWinWidth;
/* static */ int g_mainWinCY1 = g_mainWinHeight;
/* static */ int numClassRecords1, numClassRecords2;
/* static */ WObject Hashtable_keys[HASHTABLE_SIZE];
/* static */ uint32 Hashtable_elements[HASHTABLE_SIZE];
/* static */ uint32 classRecordOfs1[MAX_CLASS], classRecordOfs2[MAX_CLASS];
/* static */ uint32 classRecordSize1[MAX_CLASS], classRecordSize2[MAX_CLASS];
/* static */ uchar *classesMap1, *classesMap2;
#ifdef QUICKBIND
/* static */ int32 postPaintMethodMapNum, postEventMethodMapNum;
#endif

#if defined(OSASK)
#ifndef USE_CONSOLE
	#define printf(str, ...)
#else
	#define	CONS_IBUFSIZ	256
	#define	CONS_EBUFSIZ	 64

	struct CONSOLE {
		int x_size, y_size, x_cur, y_cur, color;
		int ibuf_rptr, ibuf_wptr, ebuf_rptr, ebuf_wptr, curflag;
		unsigned char *cons_buf, *input_buf, *echo_buf;
		struct LIB_TEXTBOX *textbox;
	};

	static struct LIB_WINDOW *console;
	static struct LIB_TEXTBOX *contitle;
	static struct CONSOLE *stdout;
	static char strbuf[256];

	static struct CONSOLE *copen(const int x_size, const int y_size,
		struct LIB_WINDOW *window, const int x0, const int y0, const int color, const int backcolor)
	/* コンソール初期化 */
	{
		struct CONSOLE *cons = (struct CONSOLE *) malloc(sizeof (struct CONSOLE));
		int i, j;
		char *p;
		cons->textbox = lib_opentextbox((backcolor >= 0) ? 0x0001 : 0x0000, 0,
			(backcolor >= 0) ? backcolor : 0, x_size, y_size, x0, y0, window, 0x00c0, 0);
		cons->x_size = x_size;
		cons->y_size = y_size;
		cons->color = color;
		cons->cons_buf  = (char *) malloc((x_size + 1) * (y_size + 1));
		cons->input_buf = (char *) malloc(CONS_IBUFSIZ);
		cons->echo_buf  = (char *) malloc(CONS_EBUFSIZ);
		cons->x_cur = 0;
		cons->y_cur = 0;
		cons->ibuf_rptr = 0;
		cons->ibuf_wptr = 0;
		cons->ebuf_rptr = 0;
		cons->ebuf_wptr = 0;
		cons->curflag = 0;
		p = cons->cons_buf;
		for (i = 0; i < y_size + 1; i++) {
			for (j = 0; j < x_size; j++)
				*p++ = ' ';
			*p++ = '\0';
		}
		return cons;
	}

	static void cons_sub_putcursor(struct CONSOLE *cons)
	/* カーソルを出す */
	{
		lib_putstring_ASCII(0x0000, cons->x_cur, cons->y_cur, cons->textbox, cons->color, 0, "_");
		return;
	}

	static void cons_sub_erasecursor(struct CONSOLE *cons)
	/* カーソルを消す */
	{
		lib_putstring_ASCII(0x0000, cons->x_cur, cons->y_cur, cons->textbox, cons->color, 0, " ");
		return;
	}

	static void cons_sub_scroll(struct CONSOLE *cons)
	/* スクロール */
	{
		char *s, *d;
		int i, j, xsiz1 = cons->x_size + 1;
		d = cons->cons_buf;
		s = d + cons->x_size + 1;
		for (i = 0; i < cons->y_size; i++) {
			for (j = 0; j < xsiz1; j++)
				*d++ = *s++;
			lib_putstring_ASCII(0x0000, 0, i, cons->textbox, cons->color, 0, d);
		}
		cons->y_cur--;
		return;
	}

	static void cputc(int c, struct CONSOLE *cons)
	/* コンソール一文字出力 */
	{
		if (cons->curflag)
			cons_sub_erasecursor(cons);
		if (c == '\n') {
			cons->x_cur = 0;
			if (++cons->y_cur == cons->y_size)
				cons_sub_scroll(cons);
		} else {
			char s[2];
			cons->cons_buf[cons->y_cur * (cons->x_size + 1) + cons->x_cur] = s[0] = (unsigned char) c;
			s[1] = '\0';
			lib_putstring_ASCII(0x0000, cons->x_cur, cons->y_cur, cons->textbox, cons->color, 0, s);
			if (++cons->x_cur == cons->x_size) {
				cons->x_cur = 0;
				if (++cons->y_cur == cons->y_size)
					cons_sub_scroll(cons);
			}
		}
		if (cons->curflag)
			cons_sub_putcursor(cons);
		return;
	}

	static void cputs(const unsigned char *str, struct CONSOLE *cons)
	/* コンソールストリング出力 */
	{
		while (*str)
			cputc((int) *str++, cons);
		return;
	}

	static int printf(const char *format, ...)
	/* コンソール整形出力 */
	{
		int i;
		va_list ap;

		va_start(ap, format);
		i = vsprintf(strbuf, format, ap);
		va_end(ap);
		cputs(strbuf, stdout);
		return i;
	}
#endif

/* static */ int getsignalw()
/* 0は返らない */
{
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

/* static */ int getparam()
/* 通常よりも長いシグナルを受け取るための関数 */
{
	int param = *sig_ptr++;
	lib_waitsignal(0x0000, 1, 0);
	return param;
}
#endif

/* static */ int Hashtable_put(WObject key, uint32 *object){
/* Hashtableにキーを設定する */
	int i;
	for (i = 0; i < HASHTABLE_SIZE; i++) {
		if (Hashtable_keys[i] == 0) {
			Hashtable_keys[i] = key;
			Hashtable_elements[i] = (int)(&object[0]);
			printf("Hashtable_put(%d)\n", key);
			return 1;
		}
	}
	printf("Hashtable_put fault\n");
	return 0;
}

/* static */ int Hashtable_remove(WObject key){
/* Hashtableからオブジェクトを削除する */
	int i;
	uint32 *list;
	for (i = 0; i < HASHTABLE_SIZE; i++) {
		if (Hashtable_keys[i] == key) {
			Hashtable_keys[i] = 0;
			list = (uint32 *)Hashtable_elements[i];
			xfree(list);
			Hashtable_elements[i] = 0;
			printf("Hashtable_remove(%d)\n", key);
			return 1;
		}
	}
	printf("Hashtable_remove fault\n");
	return 0;
}

/* static */ int Hashtable_get(WObject key, uint32 **object){
/* Hashtableからオブジェクトを得る */
	int i;
	for (i = 0; i < HASHTABLE_SIZE; i++) {
		if (Hashtable_keys[i] == key) {
			*object = (uint32 *)Hashtable_elements[i];
			//printf("Hashtable_get(%d)\n", key);
			return 1;
		}
	}
	*object = NULL;
	printf("Hashtable_get fault\n");
	return 0;
}

/* static */ void setsystimer(int32 duration){
/* システムタイマーを設定する */
#if defined(OSASK)
	lib_settimer(0x01, 0x01c0); /* Kタン指摘 [OSASK 6496] ※バグがなくなればこれは不要 */
	lib_settimertime(0x32, 0x01c0, (duration % 1000) * 4294967, duration / 1000, 0);
#endif
}

/* static */ void wait(int32 duration){
/* 一定時間待つ */
#if defined(OSASK)
	int signal;

	setsystimer(duration);
	do {
		signal = getsignalw();
		/* この関数は0を返さないので0チェックは不要 */
		if (signal == COORDINATE) { /* マウス座標 */
			getparam(); /* xを読み捨てる */
			getparam(); /* yを読み捨てる */
		}
	} while (signal != TIMER_SIGNAL);
	/* TIMER_SIGNALが来たらタイマは自動的に休止状態になるのでsettimerの必要なし */
#endif
}

/* static */ void my_exit(int code)
/* アプリケーション終了 */
{
#if defined(OSASK)
	lib_close(code);
#elif defined(MONA)
	exit(code);
#endif
}

/* 24bit -> 24bit */
#define getRGB24(r, g, b) ((r & 0xFF) << 16 | (g & 0xFF) << 8 | (b & 0xFF))
/* 24bit -> 16bit */
#define getRGB16(r, g, b) ((r & 0xF8) << 8 | (g & 0xFC) << 3 | (b & 0xF8) >> 3)

#define getAlpha(c) (0x100 - ((c >> 24) & 0xFF))
#define getRed(c)   ((c >> 16) & 0xFF)
#define getGreen(c) ((c >> 8) & 0xFF)
#define getBlue(c)  (c & 0xFF)

/* static */ uint32 calc_alpha(uint32 src, uint32 dst, uchar a)
{
	int16 r1, g1, b1, r2, g2, b2, r3, g3, b3;

	r1 = getRed(src)   + 1;
	g1 = getGreen(src) + 1;
	b1 = getBlue(src)  + 1;
	r2 = getRed(dst)   + 1;
	g2 = getGreen(dst) + 1;
	b2 = getBlue(dst)  + 1;
	//r3 = (r2 * a + r1 * (256 - a))/256 - 1;
	//g3 = (g2 * a + g1 * (256 - a))/256 - 1;
	//b3 = (b2 * a + b1 * (256 - a))/256 - 1;
	r3 = ((r2 * a) >> 8) + r1  - ((a * r1) >> 8) - 1;
	g3 = ((g2 * a) >> 8) + g1  - ((a * g1) >> 8) - 1;
	b3 = ((b2 * a) >> 8) + b1  - ((a * b1) >> 8) - 1;
	return getRGB24(r3, g3, b3);
}

/* static */ int calc_seq_code(int x, int y)
/* 端点分類コードを求める */
{
	int code = 0;
	if (x < g_mainWinCX0) code += CLIP_LEFT;
	if (x > g_mainWinCX1) code += CLIP_RIGHT;
	if (y < g_mainWinCY0) code += CLIP_TOP;
	if (y > g_mainWinCY1) code += CLIP_BOTTOM;
	return code;
}

/* static */ int calc_clipped_point(int code, int x0, int y0, int x1, int y1, int *x, int *y)
/* クリッピング後の座標を求める */
{
	int cx, cy;
	/* ウィンドウの左端より外側にある */
	if ((code & CLIP_LEFT) != 0) {
		cy = ( y1 - y0 ) * ( g_mainWinCX0 - x0 ) / ( x1 - x0 ) + y0;
		if(( cy >= g_mainWinCY0 ) && ( cy <= g_mainWinCY1 )) {
			*x = g_mainWinCX0;
			*y = cy;
			return 1;  /* エリア内に収まったら終了 */
		}
	}
	/* ウィンドウの右端より外側にある */
	if ( ( code & CLIP_RIGHT ) != 0 ) {
		cy = ( y1 - y0 ) * ( g_mainWinCX1 - x0 ) / ( x1 - x0 ) + y0;
		if ( ( cy >= g_mainWinCY0 ) && ( cy <= g_mainWinCY1 ) ) {
			*x = g_mainWinCX1;
			*y = cy;
			return 1;  /* エリア内に収まったら終了 */
		}
	}
	/* ウィンドウの上端より外側にある */
	if ( ( code & CLIP_TOP ) != 0) {
		cx = ( x1 - x0 ) * ( g_mainWinCY0 - y0 ) / ( y1 - y0 ) + x0;
		if ( ( cx >= g_mainWinCX0 ) && ( cx <= g_mainWinCX1 ) ) {
			*x = cx;
			*y = g_mainWinCY0;	
			return 1;  /* エリア内に収まったら終了 */
		}
	}
	/* ウィンドウの下端より外側にある */
	if ( ( code & CLIP_BOTTOM ) != 0 ) {
		cx = ( x1 - x0 ) * ( g_mainWinCY1 - y0 ) / ( y1 - y0 ) + x0;
		if( ( cx >= g_mainWinCX0 ) && ( cx <= g_mainWinCX1 ) ) {
			*x = cx;
			*y = g_mainWinCY1;
			return 1;  /* エリア内に収まったら終了 */
		}
	}
	return -1;  /* クリッピングされなかった場合、線分は完全に不可視 */
}

/* static */ int clipping(int *x0, int *y0, int *x1, int *y1)
/* クリッピングメインルーチン
 >0 ... クリッピングされた
 0  ... クリッピングの必要なし
 <0 ... 線分は完全に不可視 */
{
	int code0, code1; /* 端点分類コード */
	code0 = calc_seq_code( *x0, *y0 );  /* 始点の端点分類コードを求める */
	code1 = calc_seq_code( *x1, *y1 );  /* 終点の端点分類コードを求める */
	/* 端点分類コードがどちらも0000ならばクリッピングは必要なし */
	if ((code0 == 0) && (code1 == 0)) return 0;
	/* 始点･終点の端点分類コードの論理積が非０ならば線分は完全に不可視 */
	if ((code0 & code1) != 0) return -1;
	/* 始点のクリッピング */
	if (code0 != 0)
		if (calc_clipped_point(code0, *x0, *y0, *x1, *y1, x0, y0 ) < 0)
			return -1;
	/* 終点のクリッピング */
	if (code1 != 0 )
		if (calc_clipped_point(code1, *x0, *y0, *x1, *y1, x1, y1 ) < 0)
			return -1;
	return 1;
}

#if defined(OSASK)
#if (!defined(LIB_GBOX_BUF))
	#define LIB_GBOX_BUF(gbox)   ((void *) ((char *) (gbox) + 64))
	#define LIB_GBOX_BUF8(gbox)  ((unsigned char *) LIB_GBOX_BUF(gbox))
	#define LIB_GBOX_BUF16(gbox) ((unsigned short *) LIB_GBOX_BUF(gbox))
	#define LIB_GBOX_BUF32(gbox) ((unsigned int *) LIB_GBOX_BUF(gbox))
#endif
#endif

/* static */ void my_flushgbox0(int x0, int y0, int x1, int y1){
#if defined(OSASK)
	int x, y, w, h;

	if (x0 == 0 && y0 == 0 && x1 == g_mainWinWidth && y1 == g_mainWinHeight) {
		lib_flushgraphbox(0x8004, window, 0, 0, g_mainWinWidth, g_mainWinHeight, 0, LIB_GBOX_BUF32(gbox));
	} else {
		if (x0 < 0) x0 = 0;
		if (y0 < 0) y0 = 0;
		if (x1 >= g_mainWinWidth)  x1 = g_mainWinWidth  - 1;
		if (y1 >= g_mainWinHeight) y1 = g_mainWinHeight - 1;
		x  = (x0 < x1) ? x0 : x1;
		y  = (y0 < y1) ? y0 : y1;
		w  = (x0 < x1) ? x1 - x0 + 1 : x0 - x1 + 1;
		h  = (y0 < y1) ? y1 - y0 + 1 : y0 - y1 + 1;
		lib_flushgraphbox(0x8004, window, x, y, w, h, 
			(g_mainWinWidth - w) * 4, LIB_GBOX_BUF32(gbox) + y * g_mainWinWidth + x);
	}
#endif
}

#define WOBJ_ImageWidth(o) (objectPtr(o))[1].intValue
#define WOBJ_ImageHeight(o) (objectPtr(o))[2].intValue

/* static */ int32 my_getpixel0(int x0, int y0){
#if defined(OSASK)
	return *(LIB_GBOX_BUF32(gbox) + (y0 + g_mainWinOffY) * g_mainWinWidth + (x0 + g_mainWinOffX));
#elif defined(MONA)
	return 0;
#endif
}

/* static */ void my_putpixel0(int x0, int y0, uint32 rgb24, int32 dop, int width, int height, uint32 *ibuff){
	if (x0 >= g_mainWinCX1 || y0 >= g_mainWinCY1) return;
	if (width == 0 && height == 0) {
#if defined(OSASK)
		uint32 src;
		
		if (rgb24 >= 0xFF000000) {
			// 透過色（描画しない）
		} else if (0x1000000 <= rgb24 && rgb24 < 0xFF000000) {
			src = (uint32)(*(LIB_GBOX_BUF32(gbox) + (y0 + g_mainWinOffY) * g_mainWinWidth + (x0 + g_mainWinOffX)));
			*(LIB_GBOX_BUF32(gbox) + (y0 + g_mainWinOffY) * g_mainWinWidth + (x0 + g_mainWinOffX)) = 
				calc_alpha(src, (uint32)rgb24, getAlpha(rgb24)); // アルファブレンディング
		} else if (dop == DRAW_AND) {
			*(LIB_GBOX_BUF32(gbox) + (y0 + g_mainWinOffY) * g_mainWinWidth + (x0 + g_mainWinOffX)) &= rgb24;
		} else if (dop == DRAW_OR) {
			*(LIB_GBOX_BUF32(gbox) + (y0 + g_mainWinOffY) * g_mainWinWidth + (x0 + g_mainWinOffX)) |= rgb24;
		} else if (dop == DRAW_XOR) {
			*(LIB_GBOX_BUF32(gbox) + (y0 + g_mainWinOffY) * g_mainWinWidth + (x0 + g_mainWinOffX)) ^= rgb24;
		} else {
			*(LIB_GBOX_BUF32(gbox) + (y0 + g_mainWinOffY) * g_mainWinWidth + (x0 + g_mainWinOffX)) = rgb24;
		}
#elif defined(MONA)
		if (rgb24 >= 0xFF000000) {
			// 透過色（描画しない）
		} else {
			//screen.putPixel16(x0 + g_mainWinOffX, y0 + g_mainWinOffY, rgb24);
			//screen.putPixel16(x0 + g_mainWinOffX, y0 + g_mainWinOffY, (0xffffff & rgb24));
			image->SetPixel(x0, y0, System::Drawing::Color::FromArgb((rgb24 >>16) & 0xFF, (rgb24 >> 8) & 0xFF, rgb24 & 0xFF));
		}
#endif
	} else {
		//width  = WOBJ_ImageWidth(image);
		//height = WOBJ_ImageHeight(image);
		//if (!Hashtable_get(image, &ibuff)) return;
		if (dop == DRAW_AND) {
			ibuff[width * y0 + x0] &= rgb24;
		} else if (dop == DRAW_OR) {
			ibuff[width * y0 + x0] |= rgb24;
		} else if (dop == DRAW_XOR) {
			ibuff[width * y0 + x0] ^= rgb24;
		} else {
			ibuff[width * y0 + x0] = rgb24;
		}
	}
}

/* static */ void my_drawstr0(int x0, int y0, char* str, int32 fstyle, uint32 rgb24, int32 dop, int iw, int ih, uint32 *ibuff) {
/* グラフィックボックスに文字を描く */
#if defined(OSASK)
	int i = 0, j = 0, k = 0, l = 0, n = 0, cnt = 0, x1 = 0, y1 = 0;
	uchar c1 = 0, c2 = 0, c3 = 0;
	uchar adder[4][3] = {
		{0,0,0},{8,0,8},{16,8,0},{24,8,8}
	};

	//printf("%s\n", str);
	//while (str[i] != 0 && i < xstrlen(str)){
	for (i = 0; i < xstrlen(str); i++) {
		// 文字列の終端
		if (str[i] == 0) break;
		// 1 バイト目
		c1 = (uchar)str[i];
		// 0aaa bbbb - > 0aaa bbbb (0x20-0x7F)
		if (0x20 <= c1 && c1 <= 0x7F) {
			//printf("n = %d\n", c1);
			n = c1;
		// 110a aabb 10bb cccc -> 0000 0aaa bbbb cccc (0xC280-0xDFBF)
		} else if (0xC2 <= c1 && c1 <= 0xDF) {
			c2 = (uchar)str[++i];
			n = ((c1 & 0x1F) >> 6) | (c2 & 0x3F);
		// 1110 aaaa 10bb bbcc 10cc dddd -> aaaa bbbb cccc dddd (0xE0A080-0xEFBFBF)
		} else if (0xE0 <= c1 && c1 <= 0xEF) {
			c2 = (uchar)str[++i];
			c3 = (uchar)str[++i];
			n = ((c1 & 0xF) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F);
		} else {
			n = 0;
		}
		n = getFontIndex(n);
		//printf("n = %d\n", n);
		// 半角フォント描画
		if (n >= 0x49C00){
			for (j = 0; j < 16; j++){
				for (k = 7; k >= 0; k--){
					x1 = cnt * 8 + (7 - k) + x0;
					y1 = j + y0;
					if (((jisx0208[n + j] >> k) & 0x1) == 1 && 
						x1 < g_mainWinCX1 && y1 < g_mainWinCY1)
					{
						my_putpixel0(x1, y1, rgb24, dop, iw, ih, ibuff);
						// 太字
						if (fstyle == Font_BOLD && x1 + 1 < g_mainWinCX1) {
							my_putpixel0(x1 + 1, y1, rgb24, dop, iw, ih, ibuff);
						}
					}
				}
			}
			cnt++;
		// 全角フォント描画
		} else {
			for (l = 0; l < 4; l++){
				for (j = 0; j < 8; j++){
					for (k = 7; k >= 0; k--){
						x1 = cnt * 8 + (7 - k) + x0 + adder[l][1];
						y1 = j + y0 + adder[l][2];
						if (((jisx0208[n + j + adder[l][0]] >> k) & 0x1) == 1 && 
							x1 < g_mainWinCX1 && y1 < g_mainWinCY1)
						{
							my_putpixel0(x1, y1, rgb24, dop, iw, ih, ibuff);
							// 太字
							if (fstyle == Font_BOLD && x1 + 1 < g_mainWinCX1) {
								my_putpixel0(x1 + 1, y1, rgb24, dop, iw, ih, ibuff);
							}
						}
					}
				}
			}
			cnt += 2;
		}
	}

	if (iw == 0 && ih == 0)
		//my_flushgbox0(0, 0, g_mainWinWidth, g_mainWinHeight);
		my_flushgbox0(x0, y0, x1, y0 + 16);
#elif defined(MONA)
	int i = 0, j, k, c, x1, y1;

	//printf("%s\n", str);
	while (str[i] != 0 && i <= 25){
		for (j = 0; j < 16; j++){
			for (k = 7; k >= 0; k--){
				c = (int)str[i];
				if (c > 128) c = 128;
				x1 = i * 8 + 7 - k + x0;
				y1 = j + y0;
				if (((asciiMap[c][j] >> k) & 0x1) == 1 && 
					x1 < g_mainWinWidth && 
					y1 < g_mainWinHeight)
				{
					my_putpixel0(x1, y1, rgb24, dop, 0, 0, NULL);
				}
			}
		}
		i++;
	}
#endif
}

/* static */ void my_fillrect0(int x0, int y0, int w, int h, uint32 rgb24, int32 dop, int iw, int ih, uint32 *ibuff) {
/* 矩形を塗りつぶす */
	int i, j;
	int x1 = x0 + w;
	int y1 = y0 + h;

	// クリッピング
	if (clipping(&x0, &y0, &x1, &y1) < 0) return;

	for (i = x0; i <= x1; i++) {
		for (j = y0; j <= y1; j++) {
			my_putpixel0(i, j, rgb24, dop, iw, ih, ibuff);
		}
	}

	if (iw == 0 && ih == 0)
		//my_flushgbox0(0, 0, g_mainWinWidth, g_mainWinHeight);
		my_flushgbox0(x0, y0, x1, y1);
}

/* static */ void my_drawline0(int x0, int y0, int x1, int y1, uint32 rgb24, int32 dop, int iw, int ih, uint32* ibuff) {
/* グラフィックボックスに直線を引く */
	int E,x,y,dx,dy,sx,sy,i;

	// クリッピング
	if (clipping(&x0, &y0, &x1, &y1) < 0) return;

	sx = ( x1 > x0 ) ? 1 : -1;
	dx = ( x1 > x0 ) ? x1 - x0 : x0 - x1;
	sy = ( y1 > y0 ) ? 1 : -1;
	dy = ( y1 > y0 ) ? y1 - y0 : y0 - y1;

	x = x0;
	y = y0;
	/* 傾きが1以下の場合 */
	if( dx >= dy ) {
		E = -dx;
		for( i = 0; i <= dx; i++ ) {
			my_putpixel0(x, y, rgb24, dop, iw, ih, ibuff);
			x += sx;
			E += 2 * dy;
			if( E >= 0 ) {
				y += sy;
				E -= 2 * dx;
			}
		}
	/* 傾きが1より大きい場合 */
	} else {
		E = -dy;
		for( i = 0; i <= dy; i++ ) {
			my_putpixel0(x, y, rgb24, dop, iw, ih, ibuff);
			y += sy;
			E += 2 * dx;
			if( E >= 0 ) {
				x += sx;
				E -= 2 * dy;
			}
		}
	}

	if (iw == 0 && ih == 0)
		//my_flushgbox0(0, 0, g_mainWinWidth, g_mainWinHeight);
		my_flushgbox0(x0, y0, x1, y1);
}

//static void dumpStackTrace(){
//}

/* static */ void loadWrpWarpFile(char* filename) {
/* WRPファイルをメモリーに読み込む */
#if defined(OSASK)
	int i, filesize1, filesize2, filesize3;
	uchar *fp1, *fp2, *fp3;

	//
	// Waba Class Load
	//

	// ファイルオープン
	lib_initmodulehandle0(0x0008, 0x0220); /* user-dirに初期化 */
	lib_steppath0(0, 0x0220 /* slot */, "WABA    .LIB", 16 /* sig */);
	do {
		i = getsignalw();
	} while (i < 16 || 31 < i);
	if (i != 16){
		printf("base class read error(%d)\n", i);
		return;
	}else{
		//printf("base class open succeed\n");
		filesize1 = lib_readmodulesize(0x0220);
		printf("filesize=%d\n", filesize1);
	}

	// クラスマップ用領域確保
	//classesMap1 = xmalloc(filesize1); // メモリ確保

	// ファイル読み込み
	fp1 = (char *) lib_readCSd(0x0010 /* mapping領域の始まり */);
	lib_mapmodule(0x0000 /* opt */, 0x0220 /* slot */, 0x5 /* R-mode */, filesize1 /* map-window size */, fp1, 0);
	classesMap1 = fp1;
	numClassRecords1 = getUInt32(classesMap1+4);
	//printf("numClassRecords=%d\n", numClassRecords1);
	for ( i=0 ; i<numClassRecords1 ; i++ ) {
		classRecordOfs1[i] = getUInt32(classesMap1+8+4*i);
		classRecordSize1[i] = getUInt32(classesMap1+12+4*i)-classRecordOfs1[i];
		//printf("classRedordOfs=%d,Size=%d\n", classRecordOfs1[i], classRecordSize1[i]);
	}
	printf("base class read complete\n");

	//
	// User Class Load
	//

	// クラスマップ用領域確保
	//classesMap2 = xmalloc(filesize2); // メモリ確保

	// ファイルオープン
	lib_initmodulehandle1(0x0230 /* slot */, 1 /* num */, 16 /* sig */);
	//lib_initmodulehandle0(0x0008, 0x0230); /* user-dirに初期化 */
	//lib_steppath0(0, 0x0230 /* slot */, "HELLO   .WRP", 16 /* sig */);
	do {
		i = getsignalw();
	} while (i < 16 || 31 < i);
	if (i != 16){
		printf("main class read error(%d)\n", i);
		return;
	}else{
		//printf("main class open succeed\n");
		filesize2 = lib_readmodulesize(0x0230);
		printf("filesize=%d\n", filesize2);
	}

	// ファイル読み込み
	//fp2 = (char *) lib_readCSd(0x0010 + filesize /* mapping領域の始まり */);
	fp2 = fp1 + ((filesize1 + 0xfff) & ~0xfff); // Kタン指摘 [OSASK 6479]
	lib_mapmodule(0x0000 /* opt */, 0x0230 /* slot */, 0x5 /* R-mode */, filesize2 /* map-window size */, fp2, 0);
	classesMap2 = fp2;
	numClassRecords2 = getUInt32(classesMap2+4);
	//printf("numClassRecords2=%d\n", numClassRecords2);
	for ( i=0 ; i<numClassRecords2 ; i++ ) {
		classRecordOfs2[i] = getUInt32(classesMap2+8+4*i);
		classRecordSize2[i] = getUInt32(classesMap2+12+4*i)-classRecordOfs2[i];
		//printf("classRedordOfs2=%d,Size=%d\n", classRecordOfs2[i], classRecordSize2[i]);
	}
	printf("main class read complete\n");

	//
	// JIS Font Load
	//

	// ファイルオープン
	lib_initmodulehandle0(0x0008, 0x0240); /* user-dirに初期化 */
	lib_steppath0(0, 0x0240 /* slot */, "JISX0208.FNT", 16 /* sig */);
	//lib_steppath0(0, 0x0240 /* slot */, "JPN16V00.FNT", 16 /* sig */);
	do {
		i = getsignalw();
	} while (i < 16 || 31 < i);
	if (i != 16){
		printf("font file read error(%d)\n", i);
		return;
	}else{
		//printf("font file open succeed\n");
		filesize3 = lib_readmodulesize(0x0240);
		printf("filesize=%d\n", filesize3);
	}

	// ファイル読み込み
	//fp3 = (char *) lib_readCSd(0x0010 + filesize /* mapping領域の始まり */);
	fp3 = fp2 + ((filesize2 + 0xfff) & ~0xfff); // Kタン指摘 [OSASK 6479]
	lib_mapmodule(0x0000 /* opt */, 0x0240 /* slot */, 0x5 /* R-mode */, filesize3 /* map-window size */, fp3, 0);
	jisx0208 = fp3;
	printf("font file read complete\n");
#elif defined(MONA)
	int i;
	/*volatile*/ dword filesize1 = 0, filesize2 = 0;
	dword readsize1 = 0, readsize2 = 0;
	uchar *fp1, *fp2;

	//
	// Waba Class Load
	//

	if(syscall_file_open("/APPS/WABA.LIB", 0, &filesize1) != 0){
		printf("%s\n", errorMessages[2]);
	}else{
		//printf("filesize=%dbyte.\n", filesize1);
		classesMap1 = (uchar*)xmalloc(filesize1);
		if(syscall_file_read((char*)classesMap1, filesize1, &readsize1) == 0){
			fp1 = classesMap1;
			numClassRecords1 = getUInt32(fp1+4);
			//printf("numClassRecords=%d\n", numClassRecords1);
			for (i = 0 ; i < numClassRecords1 ; i++) {
				classRecordOfs1[i] = getUInt32(fp1+8+4*i);
				classRecordSize1[i] = getUInt32(fp1+12+4*i)-classRecordOfs1[i];
			}
			printf("/APPS/WABA.LIB opened.\n");
		}
		syscall_file_close();
	}

	//
	// User Class Load
	//

	if(syscall_file_open(filename, 0, &filesize2) != 0){
		printf("%s\n", errorMessages[3]);
	}else{
		//printf("filesize=%dbyte.\n", filesize2);
		classesMap2 = (uchar*)xmalloc(filesize2);
		if(syscall_file_read((char*)classesMap2, filesize2, &readsize2) == 0){
			fp2 = classesMap2;
			numClassRecords2 = getUInt32(fp2+4);
			//printf("numClassRecords=%d\n", numClassRecords2);
			for (i = 0 ; i < numClassRecords2 ; i++) {
				classRecordOfs2[i] = getUInt32(fp2+8+4*i);
				classRecordSize2[i] = getUInt32(fp2+12+4*i)-classRecordOfs2[i];
			}
			printf("%s opened.\n", filename);
		}
		syscall_file_close();
	}
#endif
}

/* static */ uchar *nativeLoadClass(UtfString className, uint32 *size) {
/* WRPファイル中のリソースを取得する */
	int i, nameLen;
	uchar *fp1, *fp2;

	fp1 = classesMap1;
	for (i = 0; i < numClassRecords1; i++) {
		if ( xstrncmp(className.str, (char *)(fp1+classRecordOfs1[i]+2), className.len) == 0 ) {
			//printf("%s found\n", className.str);
			nameLen = getUInt16(fp1+classRecordOfs1[i]);
			//*size = classRecordSize1[i]-nameLen-2;
			return (fp1+classRecordOfs1[i]+2+nameLen);
		}
	}

	fp2 = classesMap2;
	for (i = 0; i < numClassRecords2; i++) {
		if (xstrncmp (className.str, (char *)(fp2+classRecordOfs2[i]+2), className.len) == 0 ) {
			//printf("%s found\n", className.str);
			nameLen = getUInt16(fp2+classRecordOfs2[i]);
			//*size = classRecordSize2[i]-nameLen-2;
			return (fp2+classRecordOfs2[i]+2+nameLen);
		}
	}

	return NULL;
}

#if defined(MONA)
/* static */ void processEvent(int* vlist){
	WClass *vclass;
	Var param7[7];

	// 範囲チェック
	if((vlist[0] == 200 || vlist[0] == 201 || vlist[0] == 202) && 
		(vlist[2] <= 0 || g_mainWinWidth <= vlist[2] || vlist[3] <= 0 || g_mainWinHeight <= vlist[3]))
	{
		//printf("position is out of bouds! %d,%d,%d\n", vlist[0], vlist[2], vlist[3]);
		return;
	}
	
	if(mainWinObj && method_event != NULL){
		vclass = (WClass *)WOBJ_class(mainWinObj);
		param7[0].obj = mainWinObj;
		param7[1].intValue = vlist[0]; // type
		param7[2].intValue = vlist[1]; // key
		param7[3].intValue = vlist[2]; // x
		param7[4].intValue = vlist[3]; // y
		param7[5].intValue = vlist[4]; // modifiers
		param7[6].intValue = vlist[5]; // timeStamp
#ifdef QUICKBIND
		// cache method map numbers for commonly called methods
		postEventMethodMapNum = getMethodMapNum(vclass, createUtfString("_postEvent"),
			createUtfString("(IIIIII)V"), SEARCH_ALL);
		if (postEventMethodMapNum == -1) return;
		method_event = getMethodByMapNum(vclass, &vclass, (uint16)postEventMethodMapNum);
#else
		method_event = getMethod(vclass, createUtfString("_postEvent"), createUtfString("(IIIIII)V"), &vclass);
#endif
		executeMethod(vclass, method_event, param7, 7);
	}
}
#endif

/* static */ void stopApp(WObject mainWinObj) {
/* VM 実行停止 */
	VmStopApp(mainWinObj);
	VmFree();
}

/* static */ WObject startApp(char* filename) {
/* VM 実行開始 */
	unsigned long vmStackSize, nmStackSize, classHeapSize, objectHeapSize;
	char *className = "Main";

	// 変数初期化
#ifdef QUICKBIND
	postPaintMethodMapNum = -1;
	postEventMethodMapNum = -1;
#endif
	//vmStackSize = 1500;
	//nmStackSize = 300;
	//classHeapSize = 14000;
	//objectHeapSize = 8000;
	vmStackSize = 15000;
	nmStackSize = 3000;
	classHeapSize = 140000;
	objectHeapSize = 80000;

	// 起動メッセージ
	printf("Waba for %s %s\n", VM_OS, WABA_VERSION);
	printf("mainWinWidth %d\n", g_mainWinWidth);
	printf("mainWinHeight %d\n", g_mainWinHeight);
	printf("vmStackSize %d\n", vmStackSize);
	printf("nmStackSize %d\n", nmStackSize);
	printf("classHeapSize %d\n", classHeapSize);
	printf("objectHeapSize %d\n", objectHeapSize);
	printf("className #%s#\n", className);

	// クラスファイル読み出し
	loadWrpWarpFile(filename);

	// VM初期化
	VmInit(vmStackSize, nmStackSize, classHeapSize, objectHeapSize);

	// VM実行開始
#if defined(OSASK)
	return VmStartApp(className);
#elif defined(MONA)
	WClass *vclass;
	Var param5[5];
	Var param7[7];
	mainWinObj =  VmStartApp(className);
	if(!mainWinObj){
		stopApp(mainWinObj);
		return (WObject)NULL;
	}else{
		vclass = (WClass *)WOBJ_class(mainWinObj);
	}

	// メソッド呼び出し高速化
#ifdef QUICKBIND
	// cache method map numbers for commonly called methods
	postPaintMethodMapNum = getMethodMapNum(vclass, createUtfString("_doPaint"),
		createUtfString("(IIII)V"), SEARCH_ALL);
	postEventMethodMapNum = getMethodMapNum(vclass, createUtfString("_postEvent"),
		createUtfString("(IIIIII)V"), SEARCH_ALL);
	method_paint = getMethodByMapNum(vclass, &vclass, (uint16)postPaintMethodMapNum);
	method_event = getMethodByMapNum(vclass, &vclass, (uint16)postEventMethodMapNum);
	if (postPaintMethodMapNum == -1 || postEventMethodMapNum == -1)
		return 0;
#else
	method_paint = getMethod(vclass, createUtfString("_doPaint"), createUtfString("(IIII)V"), &vclass);
	method_event = getMethod(vclass, createUtfString("_postEvent"), createUtfString("(IIIIII)V"), &vclass);
#endif

	// 描画開始
	if (method_paint != NULL){
		param5[0].obj = mainWinObj;
		param5[1].intValue = 0;
		param5[2].intValue = 0;
		param5[3].intValue = g_mainWinWidth;
		param5[4].intValue = g_mainWinHeight;
		printf("Main.doPaint()\n");
		executeMethod(vclass, method_paint, param5, 1);
	}

	// ダミーイベント発生
	if (method_event != NULL){
		param7[0].obj = mainWinObj;
		param7[1].intValue = 301; // ControlEvent.FOCUS_IN
		param7[2].intValue = 0; // key
		param7[3].intValue = 0; // x
		param7[4].intValue = 0; // y
		param7[5].intValue = 0; // modifiers
		param7[6].intValue = 0; // timeStamp
		executeMethod(vclass, method_event, param7, 7);
	}
	return mainWinObj;
#endif
}
