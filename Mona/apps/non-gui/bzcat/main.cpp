// This file is in the public domain.
// There are no restrictions on any sort of usage of this file.

#include <monapi.h>
#include <gui/messages.h>

using namespace MonAPI;

int MonaMain(List<char*>* pekoe)
{
	if (pekoe->size() < 1)
	{
		printf("usage: BZCAT FILENAME.BZ2\n");
		return 1;
	}
	
	MessageInfo msg;
	dword guisvr = Message::lookupMainThread("GUI.SVR");
	if (Message::sendReceive(&msg, guisvr, MSG_GUISERVER_DECOMPRESSBZ2FILE, 1, 0, 0, pekoe->get(0)) != 0)
	{
		printf("ERROR: can not connect to GUI.SVR\n");
		return 1;
	}
	char* data = (char*)MemoryMap::map(msg.arg2);
	if (data == NULL) return 1;
	
	printf(data);
	MemoryMap::unmap(msg.arg2);
	Message::send(guisvr, MSG_GUISERVER_DISPOSEHANDLE, msg.arg2);
	return 0;
}
