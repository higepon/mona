#include <monapi.h>
using namespace MonAPI;

#define SVR "GUI Server"

enum
{
	MSG_GUISERVER_GETFONT = 0x4000,
	MSG_GUISERVER_RETURNFONT
};

static int SendMessage(dword to, dword header, dword arg1, dword arg2, dword arg3)
{
	MessageInfo msg;
	Message::create(&msg, header, arg1, arg2, arg3, NULL);
	return Message::send(to, &msg);
}

static int font_size = 0;
static dword font_handle;

static void ReadFont(const char* file)
{
	printf("%s: Reading %s....", SVR, file);
	FileInputStream* fis = new FileInputStream((char*)file);
	if (fis->open() != 0)
	{
		printf("ERROR\n");
	}
	else
	{
		font_size = fis->getFileSize();
		byte* buf = new byte[font_size];
		fis->read(buf, font_size);
		fis->close();
		printf("OK\n");
		
		font_handle = MemoryMap::create(font_size);
		if (font_handle == 0)
		{
			printf("%s: MemoryMap create error = %x\n", SVR, MemoryMap::getLastError());
			font_size = 0;
		}
		else
		{
			byte* font_data = MemoryMap::map(font_handle);
			if (font_data == NULL)
			{
				printf("%s: map error\n", SVR);
				font_size = 0;
			}
			else
			{
				memcpy(font_data, buf, font_size);
			}
		}
		delete [] buf;
	}
	delete fis;
}

int MonaMain(List<char*>* pekoe)
{
	ReadFont("/MONA-12.MNF");
	if (font_size == 0) exit(1);
	
	dword init = Message::lookupMainThread("INIT");
    if (init == 0xFFFFFFFF || SendMessage(init, MSG_SERVER_START_OK, 0, 0, 0) != 0)
	{
		printf("%s: INIT error\n", SVR);
		exit(1);
	}
	
	MessageInfo msg;
	for (;;)
	{
		if (Message::receive(&msg)) continue;
		
		switch (msg.header)
		{
			case MSG_GUISERVER_GETFONT:
				SendMessage(msg.from, MSG_GUISERVER_RETURNFONT, font_handle, font_size, 0);
				break;
		}
	}
	
	MemoryMap::unmap(font_handle);
	return 0;
}
