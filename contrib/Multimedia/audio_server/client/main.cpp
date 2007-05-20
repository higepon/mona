#include <monapi.h>
#include <monalibc.h>
#include <servers/keyboard.h>
#include <servers/audio.h>
#include "../debug.h"

using namespace MonAPI;
void makeWave(monapi_cmemoryinfo *mi, int x);
void makeWave(void *p, size_t size, int x);
void connectToKeyboardServer();

int main(int argc, char *argv[])
{
	uint32_t audioServerID;
	uint32_t commanderID;
	int32_t ch;
	char str[128];
	uint8_t buf[65536];
	MessageInfo msg;
	FILE *fp;

	connectToKeyboardServer();

	fp = fopen("/APPS/TEST.WAV", "r");

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

	Message::sendReceive(&msg, commanderID, MSG_AUDIO_SERVER_COMMAND, CreateStream, ch);
	Stream *stream;
	stream = Stream::FromHandle(msg.arg2);

	makeWave(buf, 65536, 200);
/*
	int readsize;
	fread(buf, 1, 65536, fp);
	stream->write(buf, readsize);
	*/
	stream->write(buf, 65536);

	printf("Start\n");
	Message::sendReceive(&msg, commanderID, MSG_AUDIO_SERVER_COMMAND, StartChannel, ch);

	while(1)
	{
		//int readsize = fread(buf, 1, 65536, fp);
		stream->waitForWrite();
	//	stream->write(buf, readsize);
		stream->write(buf, 65536);
	}

END:

	Message::sendReceive(&msg, commanderID, MSG_AUDIO_SERVER_COMMAND, StopChannel, ch);

	Message::sendReceive(&msg, commanderID, MSG_AUDIO_SERVER_COMMAND, ReleaseChannel, ch);
	_printf("Channel was released.\n");

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

void makeWave(void *p, size_t size, int x)
{
	for( int i = 0 ; i < size/2 ; i++ )
	{
		if( i % x < 10 ) ((short*)p)[i] = 200;
		else ((short*)p)[i] = -200;
	}
}

void connectToKeyboardServer()
{
	MessageInfo msg;
	uint32_t tid;
	tid = Message::lookupMainThread("KEYBOARD.EX5");
	Message::sendReceive(&msg, tid, MSG_KEY_REGIST_TO_SERVER, syscall_get_tid());
}

