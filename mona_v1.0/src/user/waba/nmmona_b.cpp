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
so you can actually see something on the screen.

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

/* static */ uint32 classRecordOfs1[MAX_CLASS], classRecordOfs2[MAX_CLASS];
/* static */ uint32 classRecordSize1[MAX_CLASS], classRecordSize2[MAX_CLASS];
/* static */ int numClassRecords1, numClassRecords2;
/* static */ uchar *classesMap1, *classesMap2;
/* static */ Screen screen;
#ifdef QUICKBIND
/* static */ int32 postPaintMethodMapNum = -1;
/* static */ int32 postEventMethodMapNum = -1;
#endif

static void my_drawstr0(int x0, int y0, char* str, int32 rgb24) {
/* グラフィックボックスに文字を描く */
	int i = 0, j, k;
	while(str[i] != 0 && i <= 25){
		for(j = 0; j < 16; j++){
			for(k = 7; k >= 0; k--){
				if(((asciiMap[(int)(str[i])][j] >> k) & 0x1) == 1){
					screen.putPixel16(i * 8 + 7 - k + x0 + g_mainWinOffX, j + y0 + g_mainWinOffY, Color::rgb(0,0,0));
				}else{
					
				}
			}
		}
		i++;
	}
}

static void my_drawline0(int x0, int y0, int x1, int y1, int32 rgb24) {
/* グラフィックボックスに直線を引く */
	int E,x,y,dx,dy,sx,sy,i;

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
			/*if( cx0 <= x && x <= cx1 && cy0 <= y && y <= cy1 ) {
				if( ibox_ptr == NULL ) {
					set_4bit_pixel(dop, &gbox_pixel( x, y ), rgb4);
				} else {
					set_24bit_pixel(dop, &ibox_pixel( x, y, w ), rgb24);
				}
			}*/
			screen.putPixel16(x + g_mainWinOffX, y + g_mainWinOffY, rgb24);
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
			/*if( cx0 <= x && x <= cx1 && cy0 <= y && y <= cy1 ) {
				if( ibox_ptr == NULL ) {
					set_4bit_pixel(dop, &gbox_pixel( x, y ), rgb4);
				} else {
					set_24bit_pixel(dop, &ibox_pixel( x, y, w ), rgb24);
				}
			}*/
			screen.putPixel16(x + g_mainWinOffX, y + g_mainWinOffY, rgb24);
			y += sy;
			E += 2 * dx;
			if( E >= 0 ) {
				x += sx;
				E -= 2 * dy;
			}
		}
	}
}

//static void dumpStackTrace(){
//}

static void loadWrpWarpFile() {
/* WRPファイルをメモリーに読み込む */
	int i;
	volatile dword filesize1 = 0, filesize2 = 0;
	dword readsize1 = 0, readsize2 = 0;
	uchar *fp1, *fp2;

	//
	// Waba Class Load
	//

	if(syscall_file_open("/APP/WABA.LIB", 0, &filesize1) != 0){
		printf("%s\n", errorMessages[2]);
	}else{
		printf("filesize=%dbyte.\n", filesize1);
		classesMap1 = (uchar*)xmalloc(filesize1);
		if(syscall_file_read((char*)classesMap1, filesize1, &readsize1) == 0){
			fp1 = classesMap1;
			numClassRecords1 = getUInt32(fp1+4);
			printf("numClassRecords=%d\n", numClassRecords1);
			for (i = 0 ; i < numClassRecords1 ; i++) {
				classRecordOfs1[i] = getUInt32(fp1+8+4*i);
				classRecordSize1[i] = getUInt32(fp1+12+4*i)-classRecordOfs1[i];
			}
			printf("/APP/WABA.LIB opened.\n");
		}
		syscall_file_close();
	}

	//
	// User Class Load
	//

	if(syscall_file_open("/APP/HELLO.WRP", 0, &filesize2) != 0){
		printf("%s\n", errorMessages[3]);
	}else{
		printf("filesize=%dbyte.\n", filesize2);
		classesMap2 = (uchar*)xmalloc(filesize2);
		if(syscall_file_read((char*)classesMap2, filesize2, &readsize2) == 0){
			fp2 = classesMap2;
			numClassRecords2 = getUInt32(fp2+4);
			printf("numClassRecords=%d\n", numClassRecords2);
			for (i = 0 ; i < numClassRecords2 ; i++) {
				classRecordOfs2[i] = getUInt32(fp2+8+4*i);
				classRecordSize2[i] = getUInt32(fp2+12+4*i)-classRecordOfs2[i];
			}
			printf("/APP/HELLO.WRP opened.\n");
		}
		syscall_file_close();
	}
}

static uchar *nativeLoadClass(UtfString className, uint32 *size){
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

static WObject startApp() {
/* VM 実行開始 */
	unsigned long vmStackSize, nmStackSize, classHeapSize, objectHeapSize;
	char *className = "Main";

	loadWrpWarpFile();

	vmStackSize = 1500;
	nmStackSize = 300;
	classHeapSize = 14000;
	objectHeapSize = 8000;

	printf("mainWinWidth %d\n", g_mainWinWidth);
	printf("mainWinHeight %d\n", g_mainWinHeight);
	printf("vmStackSize %d\n", vmStackSize);
	printf("nmStackSize %d\n", nmStackSize);
	printf("classHeapSize %d\n", classHeapSize);
	printf("objectHeapSize %d\n", objectHeapSize);
	printf("className #%s#\n", className);

	VmInit(vmStackSize, nmStackSize, classHeapSize, objectHeapSize);
	return VmStartApp(className);
}

static void stopApp(WObject mainWinObj) {
/* VM 実行停止 */
	VmStopApp(mainWinObj);
	VmFree();
}
