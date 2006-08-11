#include <monapi.h>
#include <monapi/CString.h>
#include <monapi/messages.h>
#include <monalibc.h>

#include "PowerManager.h"

using namespace MonAPI;

int MonaMain(List<char*>* pekoe)
{
	PowerManager *pm;
	dword targetID;

	targetID = Message::lookupMainThread("MONITOR.BIN");
	if( targetID == THREAD_UNKNOWN )
	{
		printf("%s: MONITOR error\n", SVR);
	}

	if( Message::send(targetID, MSG_SERVER_START_OK) )
	{
		printf("%s: MONITOR error\n", SVR);
	}

	pm = new PowerManager;
	pm->run();

	return 0;
}
