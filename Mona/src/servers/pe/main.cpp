#include <monapi.h>
#include <monapi/CString.h>
#include <monapi/messages.h>
#include "PEServer.h"
#include "PEParser.h"

using namespace MonAPI;

static int CreateImage(monapi_cmemoryinfo** dest, dword* entryPoint, monapi_cmemoryinfo* mi, bool prompt)
{
	PEParser parser;
	if (!parser.Parse(mi->Data, mi->Size))
	{
		if (prompt) printf("%s: file type is not valid PE!\n", SVR);
		return 3;
	}
	
	monapi_cmemoryinfo* dst = monapi_cmemoryinfo_new();
	if (!monapi_cmemoryinfo_create(dst, parser.get_ImageSize(), prompt ? MONAPI_TRUE : MONAPI_FALSE))
	{
		monapi_cmemoryinfo_delete(dst);
		return 3;
	}

	if (!parser.Load(dst->Data))
	{
		if (prompt) printf("%s: load failed!\n", SVR);
		monapi_cmemoryinfo_delete(dst);
		return 3;
	}

	*dest = dst;
	*entryPoint = parser.get_EntryPoint();
	return 0;
}

static int CreateImage(monapi_cmemoryinfo** dest, dword* entryPoint, const CString& path, bool prompt)
{
	monapi_cmemoryinfo* mi = NULL;
	if (path.endsWith(".EX2"))
	{
		mi = monapi_call_file_decompress_bz2_file(path, prompt ? MONAPI_TRUE : MONAPI_FALSE);
	}
	else
	{
		mi = monapi_call_file_read_data(path, prompt ? MONAPI_TRUE : MONAPI_FALSE);
	}
	if (mi == NULL)
	{
		return 1;
	}
	monapi_cmemoryinfo* img;
	int result = CreateImage(&img, entryPoint, mi, prompt);
	monapi_cmemoryinfo_dispose(mi);
	monapi_cmemoryinfo_delete(mi);
	if (result == 0) *dest = img;
	return result;
}

static void MessageLoop()
{
	for (MessageInfo msg;;)
	{
		if (Message::receive(&msg) != 0) continue;

		switch (msg.header)
		{
			case MSG_DISPOSE_HANDLE:
				MemoryMap::unmap(msg.arg1);
				Message::reply(&msg);
				break;
			case MSG_PROCESS_CREATE_IMAGE:
			{
				monapi_cmemoryinfo* mi = NULL;
				dword entryPoint = 0;
				int result = CreateImage(&mi, &entryPoint, msg.str, msg.arg1 == MONAPI_TRUE);
				if (result == 0)
				{
					char buf[16];
					sprintf(buf, "%d", mi->Size);
					Message::reply(&msg, mi->Handle, entryPoint, buf);
					monapi_cmemoryinfo_delete(mi);
				}
				else
				{
					Message::reply(&msg, 0, result);
				}
				break;
			}
			default:
				break;
		}
	}
}

int MonaMain(List<char*>* pekoe)
{
	if (Message::send(Message::lookupMainThread("MONITOR.BIN"), MSG_SERVER_START_OK) != 0)
	{
		printf("%s: MONITOR error\n", SVR);
		exit(1);
	}

	MessageLoop();

	return 0;
}
