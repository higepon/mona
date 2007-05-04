#include <monapi.h>
#include <monalibc.h>
#include <servers/keyboard.h>
#include <servers/audio.h>
#include "../debug.h"

using namespace MonAPI;
void makeWave(monapi_cmemoryinfo *mi, int x);
void connectToKeyboardServer();

int main(int argc, char *argv[])
{
	uint32_t audioServerID;
	uint32_t commanderID;
	int32_t ch;
	char str[128];
	MessageInfo msg;

	connectToKeyboardServer();

	audioServerID = Message::lookupMainThread("AUDIO.EX5");
	if( audioServerID == THREAD_UNKNOWN )
		exit(printf("Couldn't find an audio server."));
	dprintf("AUDIO.EX5 : %x\n", audioServerID);

	Message::sendReceive(&msg, audioServerID, MSG_AUDIO_SERVER_COMMAND, GetThreadID, COMMAND_THREAD);
	commanderID = msg.arg2;
	dprintf("AUDIO.EX5 : %x\n", commanderID);

#if 1
	Message::sendReceive(&msg, commanderID, MSG_AUDIO_SERVER_COMMAND, GetServerVersion);
	printf("Server version: %x:%x\n", msg.arg2, msg.arg3);
	Message::sendReceive(&msg, commanderID, MSG_AUDIO_SERVER_COMMAND, AllocateChannel);
	ch = msg.arg2;
	printf("Channel: %d\n", ch);
	struct audio_server_channel_info ci;
	ci.channel = ch;
	ci.samplerate = 44100;
	ci.bitspersample = 16;
	ci.channels = 2;
	memcpy(str, &ci, sizeof(ci));
	Message::sendReceive(&msg, commanderID, MSG_AUDIO_SERVER_COMMAND, PrepareChannel, 0, 0, str);
	printf("Result: %d\n", msg.arg2);

	struct audio_server_buffer_info bi;
	int usingbuf = 1;
	monapi_cmemoryinfo *mi;
	monapi_cmemoryinfo *mi2;
	mi = monapi_cmemoryinfo_new();
	mi2 = monapi_cmemoryinfo_new();
	if( !monapi_cmemoryinfo_create(mi, 44100, MONAPI_TRUE) )
	{
		printf("cmi_create\n");
		monapi_cmemoryinfo_delete(mi); return -1;
	}
	if( !monapi_cmemoryinfo_create(mi2, 44100, MONAPI_TRUE) )
	{
		printf("cmi_create\n");
		monapi_cmemoryinfo_delete(mi2); return -1;
	}
	makeWave(mi, 20);
	makeWave(mi2, 20);
	bi.handle = mi->Handle;
	bi.size = mi->Size;
	memcpy(str, &bi, sizeof(bi));
	printf("Client: Handle: %x\n", bi.handle);
	printf("Client: P: %x\n", mi->Data);
	printf("I will set a buffer...\n");
	Message::sendReceive(&msg, commanderID, MSG_AUDIO_SERVER_COMMAND, RegisterTID, ch);
	Message::sendReceive(&msg, commanderID, MSG_AUDIO_SERVER_COMMAND, SetBuffer, ch, 0, str);
	Message::sendReceive(&msg, commanderID, MSG_AUDIO_SERVER_COMMAND, StartChannel, ch);

//	Message::sendReceive(&msg, commanderID, MSG_AUDIO_SERVER_COMMAND, StopChannel, ch);
	Message::sendReceive(&msg, commanderID, MSG_AUDIO_SERVER_COMMAND, StartChannel, ch);

	while(1)
	{
		if( Message::receive(&msg) ) continue;
		printf("%x\n", msg.header);
		if( msg.header == MSG_AUDIO_SERVER_MESSAGE ) switch( msg.arg1 )
		{
			case BufferIsEmpty:
			{
		//		Message::sendReceive(&msg, commanderID, MSG_AUDIO_SERVER_COMMAND, StopChannel, ch);
				if( usingbuf == 1 )
				{
					bi.handle = mi2->Handle;
					bi.size = mi2->Size;
					usingbuf = 2;
				}
				else
				{
					bi.handle = mi->Handle;
					bi.size = mi->Size;
					usingbuf = 1;
				}
				memcpy(str, &bi, sizeof(bi));
				Message::sendReceive(&msg, commanderID, MSG_AUDIO_SERVER_COMMAND, SetBuffer, ch, 0, str);
		//		Message::sendReceive(&msg, commanderID, MSG_AUDIO_SERVER_COMMAND, StartChannel, ch);
				break;
			}
			default: break;
		}
		else if( msg.header == MSG_KEY_VIRTUAL_CODE )
		{
			goto END;
		}
	}
END:

	Message::sendReceive(&msg, commanderID, MSG_AUDIO_SERVER_COMMAND, StopChannel, ch);

	Message::sendReceive(&msg, commanderID, MSG_AUDIO_SERVER_COMMAND, ReleaseChannel, ch);
	_printf("Channel was released.\n");

	monapi_cmemoryinfo_dispose(mi);
	monapi_cmemoryinfo_delete(mi);

#else
	Message::sendReceive(&msg, commanderID, MSG_AUDIO_SERVER_COMMAND, CreateChannelObject);
	ch = msg.arg2;
	Message::sendReceive(&msg, commanderID, MSG_AUDIO_SERVER_COMMAND, BindChannelObject, ch, 0, "es1370");
#endif

	return 0;
}

void makeWave(monapi_cmemoryinfo *mi, int x)
{
	for( int i = 0 ; i < mi->Size/2 ; i++ )
	{
		if( i % x < 10 ) ((short*)mi->Data)[i] = 200;
		else ((short*)mi->Data)[i] = -200;
	}
}

void connectToKeyboardServer()
{
	MessageInfo msg;
	uint32_t tid;
	tid = Message::lookupMainThread("KEYBOARD.EX5");
	Message::sendReceive(&msg, tid, MSG_KEY_REGIST_TO_SERVER, syscall_get_tid());
}

