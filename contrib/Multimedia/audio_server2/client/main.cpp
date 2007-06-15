#include <servers/audio.h>
#include <stdio.h>
#include <monapi/Message.h>
#include <monapi/Stream.h>

using namespace MonAPI;

int main()
{
	uint32_t tid;
	uint32_t ch;
	MessageInfo msg;
	Stream *s;
	FILE *fp;
	uint8_t buf[4096];
	uint32_t readsize;

	fp = fopen("/APPS/TEST.RAW", "r");
	if( fp == NULL )
	{
		puts("fp is NULL");
		return 1;
	}

	tid = Message::lookupMainThread("AUDIO.EX5");

	Message::sendReceive(&msg, tid, MSG_AUDIO_NEW_CHANNEL, 0);
	ch = msg.arg2;
	s = Stream::FromHandle(msg.arg3);

	Message::sendReceive(&msg, tid, MSG_AUDIO_SET_FORMAT, 2, 16, 44100);

	readsize = fread(buf, 1, sizeof(buf), fp);
	s->write(buf, readsize);

	Message::sendReceive(&msg, tid, MSG_AUDIO_START, ch);

	while(1)
	{
		s->waitForWrite();
		readsize = fread(buf, 1, sizeof(buf), fp);
		if( readsize <= 1 ) break;
		s->write(buf, readsize);
	}

	Message::sendReceive(&msg, tid, MSG_AUDIO_STOP, ch);

	fclose(fp);

	return 0;
}
