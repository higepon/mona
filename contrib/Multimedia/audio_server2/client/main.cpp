#include <servers/audio.h>
#include <stdio.h>
#include <monapi.h>
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
	monapi_cmemoryinfo *cmi;
	uint8_t buf[4096];
	uint32_t readsize;
	cmi = monapi_cmemoryinfo_new();
	monapi_cmemoryinfo_create(cmi, 4096, 0);
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
	fp = fopen("/APPS/TEST.RAW", "r");
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
	if( fp == NULL )
	{
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
		puts("fp is NULL");
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
		return 1;
	}

    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
	tid = Message::lookupMainThread("AUDIO.EX5");
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
	Message::sendReceive(&msg, tid, MSG_AUDIO_NEW_CHANNEL, 0);
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
	ch = msg.arg2;
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
	s = Stream::FromHandle(msg.arg3);

    _logprintf("### ch=%x handle=%x\n", msg.arg2, msg.arg3);

    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
	Message::sendReceive(&msg, tid, MSG_AUDIO_SET_FORMAT, 2, 16, 44100);
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
	readsize = fread(buf, 1, sizeof(buf), fp);
    _logprintf("readsize = %d %s %s:%d\n",readsize, __func__, __FILE__, __LINE__);

	s->write(buf, readsize);
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);
	Message::send(tid, MSG_AUDIO_START, ch);
//    return 0;
    _logprintf("%s %s:%d\n", __func__, __FILE__, __LINE__);

	while(1)
	{
		int readsize;
		if( Message::receive(&msg) ) continue;
		if( msg.header == 0xFFFFFFFF )
		{
			readsize = fread(cmi->Data, 1, msg.arg1, fp);
			puts("Received a message from the AudioServer.");
			Message::reply(&msg, cmi->Handle, readsize);
		}
	/*
		s->waitForWrite();
		readsize = fread(buf, 1, sizeof(buf), fp);
		if( readsize <= 1 ) break;
		s->write(buf, readsize);
	*/
	}

	Message::sendReceive(&msg, tid, MSG_AUDIO_STOP, ch);

	fclose(fp);

	return 0;
}
