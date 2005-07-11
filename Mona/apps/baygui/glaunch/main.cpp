/*
Copyright (c) 2005 bayside

Permission is hereby granted, free of charge, to any person 
obtaining a copy of this software and associated documentation files 
(the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, 
publish, distribute, sublicense, and/or sell copies of the Software, 
and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:

The above copyright notice and this permission notice shall be 
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <baygui.h>
#include "glaunch.h"
#include "gshortcut.h"

/** メイン */
#if defined(MONA)
int MonaMain(List<char*>* pekoe)
{
	if (pekoe->size() == 0) {
		monapi_call_process_execute_file("/APPS/BAYGUI/GLAUNCH.EX5 DISKICON", MONAPI_FALSE);
		monapi_call_process_execute_file("/APPS/BAYGUI/GLAUNCH.EX5 TERMINALICON", MONAPI_FALSE);
		monapi_call_process_execute_file("/APPS/BAYGUI/GLAUNCH.EX5 TRASHBOXICON", MONAPI_FALSE);
	} else {
		char* arg0 = pekoe->get(0);
		if (strcmp(arg0, "DISKICON") == 0) {
			ShortCut *shortcut = new ShortCut(DISKICON);
			shortcut->run();
			delete(shortcut);
		} else if (strcmp(arg0, "TERMINALICON") == 0) {
			ShortCut *shortcut = new ShortCut(TERMINALICON);
			shortcut->run();
			delete(shortcut);
		} else if (strcmp(arg0, "TRASHBOXICON") == 0) {
			ShortCut *shortcut = new ShortCut(TRASHBOXICON);
			shortcut->run();
			delete(shortcut);
		} else if (strcmp(arg0, "FILE") == 0) {
			Glaunch *launch = new Glaunch();
			launch->run();
			delete(launch);
		}
	}
}
#else
int main(int argc, char** argv)
{
	Glaunch *launch = new Glaunch();
	launch->run();
	delete(launch);
	return 0;
}
#endif
