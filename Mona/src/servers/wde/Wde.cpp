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
#include "Wde.h"

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

Wde::Wde(){
	return;
}

Wde::~Wde(){

	/* send message for KEYBDMNG.BN2 */
	if (Message::send(keysvrID, MSG_KEY_UNREGIST_FROM_SERVER, myID)) {
		printf("Shell: key unregist error\n");
	}

	/* send message for MOUSE.BN2 */
	if (Message::send(mousesvrID, MSG_MOUSE_UNREGIST_FROM_SERVER, myID)) {
		printf("Shell: mouse unregist error\n");
	}

	return;
}

bool Wde::initialize() {
	/* initialize parameter */
	hasExited = false, isMouseClick = false, isMouseMove = false;
	
	screen = new Screen();
	vscreen = new VirtualScreen();
	
	/* look up */
	myID = MonAPI::System::getThreadID();

	/* looking up keyboard server */
	keysvrID = Message::lookupMainThread("KEYBDMNG.BN2");
	if (keysvrID == 0xFFFFFFFF) {
		printf("WdeServer:KeyBoardServer not found\n");
		return false;
	}

	/* send message for KEYBDMNG.BN2 */
	if (Message::send(keysvrID, MSG_KEY_REGIST_TO_SERVER, myID)) {
		printf("WdeServer:key regist error\n");
		return false;
	}

	/* looking up mouse server */
	mousesvrID = Message::lookupMainThread("MOUSE.BN2");
	if (mousesvrID == 0xFFFFFFFF) {
		printf("WdeServer:MouseSever not found\n");
		return false;
	}

	/* send message for MOUSE.BN2 */
	if (Message::send(mousesvrID, MSG_MOUSE_REGIST_TO_SERVER, myID)) {
		printf("WdeServer:mouse regist error\n");
		return false;
	}
	
	/* server start ok */
	targetID = Message::lookupMainThread("MONITOR.BIN");
	if (targetID == THREAD_UNKNOWN) {
		printf("WdeServer:MONITER not found\n");
		return false;
	}

	/* send */
	if (Message::send(targetID, MSG_SERVER_START_OK)) {
		printf("WdeServer:server start send error\n");
		return false;
	}
	
	return true;
}

void Wde::service() {

	/* start desktop */
	if (startApp(NULL) == (WObject)NULL) {
	//if (startApp("/APPS/ADVDEMO.WRP") == (WObject)NULL) {
	//if (startApp("/APPS/GUITEST.WRP") == (WObject)NULL) {
	//if (startApp("/APPS/SCRIBBLE.WRP") == (WObject)NULL) {
		exit(1);
	}

	/* service loop */
	MessageInfo info;
	int vlist[6], key = 0;
	while(!hasExited){
#if 1
		if(!Message::receive(&info)){
			switch(info.header){
			case MSG_KEY_VIRTUAL_CODE:
				if (info.arg2 & KEY_MODIFIER_DOWN) {
					//printf("%d,", info.arg1);
					switch (info.arg1) {
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
						default : key = info.arg1; break;
					}
					// Shitt, Ctrl, Alt
					if (key < 160 || 164 < key) {
						vlist[0] = 100; // type
						vlist[1] = key; // key
						vlist[2] = 0; // x
						vlist[3] = 0; // y
						vlist[4] = 0; // modifiers
						vlist[5] = 0; // timeStamp
						processEvent(vlist);
					}
					break;
				}
			case MSG_MOUSE_INFO:
				if(info.arg3 != 0){
					if(isMouseClick == true){
						//printf("drag = %d, %d, %d\n", info.arg1, info.arg2, info.arg3);
						isMouseMove = true;
						vlist[0] = 201; // type
						vlist[1] = 0; // key
						vlist[2] = info.arg1 - g_mainWinOffX; // x
						vlist[3] = info.arg2 - g_mainWinOffY; // y
						vlist[4] = 0; // modifiers
						vlist[5] = 0; // timeStamp
						processEvent(vlist);
					}else{
						//printf("click = %d, %d, %d\n", info.arg1, info.arg2, info.arg3);
						isMouseClick = true;
						vlist[0] = 200; // type
						vlist[1] = 0; // key
						vlist[2] = info.arg1 - g_mainWinOffX; // x
						vlist[3] = info.arg2 - g_mainWinOffY; // y
						vlist[4] = 0; // modifiers
						vlist[5] = 0; // timeStamp
						processEvent(vlist);
					}
				}else{
					if(isMouseClick == true){
						//printf("up = %d, %d, %d\n", info.arg1, info.arg2, info.arg3);
						isMouseClick = false;
						isMouseMove  = false;
						vlist[0] = 202; // type
						vlist[1] = 0; // key
						vlist[2] = info.arg1 - g_mainWinOffX; // x
						vlist[3] = info.arg2 - g_mainWinOffY; // y
						vlist[4] = 0; // modifiers
						vlist[5] = 0; // timeStamp
						processEvent(vlist);
					}
				}
				break;
			default:
				break;
			}
		}
#endif
	}
	return;
}

int MonaMain(List<char*>* pekoe) {

	Wde* shell = new Wde();
	if (shell->initialize() == true) {
		shell->service();
	}

	return 0;
}
