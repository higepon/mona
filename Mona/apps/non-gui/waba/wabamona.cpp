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

#include <monapi.h>
#include <monapi/messages.h>
#include "wabamona.h"

using namespace MonAPI;

#define  MONACUI         1
#undef   MONA
#undef   WIN32
#define  WABA_VERSION    "0.2.1"
#define  VM_OS           "MONA"
#define  VM_USER         "MONA"
#define  g_mainWinOffX   300
#define  g_mainWinOffY   200
#define  g_mainWinWidth  200
#define  g_mainWinHeight 200
extern "C" {
#include "waba.c"
}

Waba::Waba() {
	/* look up */
	myID = MonAPI::System::getThreadID();

	/* initalize field */
	screen = new Screen();
	vscreen = new VirtualScreen();

	/* looking up keyboard server */
	//keysvrID = Message::lookupMainThread("KEYBDMNG.BN2");
	if (keysvrID == 0xFFFFFFFF) {
		printf("WdeServer:KeyBoardServer not found\n");
		return;
	}

	/* send message for KEYBDMNG.BN2 */
	//if (Message::send(keysvrID, MSG_KEY_REGIST_TO_SERVER, myID)) {
	//	printf("WdeServer:key regist error\n");
	//	return;
	//}

	/* looking up mouse server */
	mousesvrID = Message::lookupMainThread("MOUSE.BN2");
	if (mousesvrID == 0xFFFFFFFF) {
		printf("WdeServer:MouseSever not found\n");
		return;
	}

	/* send message for MOUSE.BN2 */
	if (Message::send(mousesvrID, MSG_MOUSE_REGIST_TO_SERVER, myID)) {
		printf("WdeServer:mouse regist error\n");
		return;
	}
	return;
}

Waba::~Waba() {

	/* send message for KEYBDMNG.BN2 */
	if (Message::send(keysvrID, MSG_KEY_UNREGIST_FROM_SERVER, myID)) {
		printf("Shell: key unregist error\n");
	}

	/* send message for MOUSE.BN2 */
	if (Message::send(mousesvrID, MSG_MOUSE_UNREGIST_FROM_SERVER, myID)) {
		printf("Shell: mouse unregist error\n");
	}

}

int Waba::main(List<char*>* pekoe) {
	// アプリケーションスタート
	if(pekoe->size() != 1){
		startApp(NULL);
	}else{
		startApp(pekoe->get(0));
	}

	// イベント待ち
	run();

	return 0;
}

void Waba::onKeyDown(int keycode, int modifiers) {
/* キークリック */
	int vlist[6], key;

	
	//printf("%d:%d,", keycode, modifiers);
	switch (keycode) {
		case 105: key = 75000; break; // PAGEUP
		case  99: key = 75001; break; // PAGEDOWN
		case 103: key = 75002; break; // HOME
		case  97: key = 75003; break; // END
		case 104: key = 75004; break; // UP
		case  98: key = 75005; break; // DOWN
		case 100: key = 75006; break; // LEFT
		case 102: key = 75007; break; // RIGHT
		case  96: key = 75008; break; // INSERT
		case  13: key = 75009; break; // ENTER
		case   9: key = 75010; break; // TAB
		case   8: key = 75011; break; // BACKSPACE
		case  27: key = 75012; break; // ESC
		case 110: key = 75013; break; // DELETE
		default : key = keycode; break;
	}
	// Shitt, Ctrl, Alt
	if (key < 160 || 164 < key || modifiers != 2 || modifiers != 130) {
		vlist[0] = 100; // type
		vlist[1] = keycode; // key
		vlist[2] = 0; // x
		vlist[3] = 0; // y
		vlist[4] = 0; // modifiers
		vlist[5] = 0; // timeStamp
		processEvent(vlist);
	}
}

void Waba::onMouseClick(int x, int y) {
/* マウスクリック */
	int vlist[6];

	//printf("x, y %d, %d\n", x, y);
	vlist[0] = 200; // type
	vlist[1] = 0; // key
	vlist[2] = x - g_mainWinOffX; // x
	vlist[3] = y - g_mainWinOffY; // y
	vlist[4] = 0; // modifiers
	vlist[5] = 0; // timeStamp
	processEvent(vlist);

	//printf("x, y %d,%d\n", x, y);
	vlist[0] = 202; // type
	vlist[1] = 0; // key
	vlist[2] = x - g_mainWinOffX; // x
	vlist[3] = y - g_mainWinOffY; // y
	vlist[4] = 0; // modifiers
	vlist[5] = 0; // timeStamp
	processEvent(vlist);
}

int MonaMain(List<char*>* pekoe) {
	monaApp = new Waba();
	return monaApp->main(pekoe);
}
