#include <stdio.h>
#include <monapi/Message.h>
#include <monapi/syscall.h>
#include <stdlib.h>
#include "servers/audio.h"
#include "audio_server.h"
#include "debug.h"

int audio_message_reply(int (*iserror)(uint32_t),
	MessageInfo *info, uint32_t arg2 = 0,
	uint32_t arg3 = 0, const char *str = NULL)
{
	dputs(__func__);
	/*
	if( iserror(arg2) )
	{
		dputs("Error");
		return MonAPI::Message::replyError(info, arg2, arg3, str);
	}
	else
	{
	*/
	//	dputs("Non Error");
		return MonAPI::Message::reply(info, arg2, arg3, str);
//	}
	dputs("NOTREACHED");
}

int isnotzero(uint32_t x){ return (x != 0); }
int iszero(uint32_t x){ return (x == 0); }
int unsupported(uint32_t x){ return 1; }
int noncheck(uint32_t x){ return 0; }

int main(int argc, char *argv[])
{
	AudioServer server;
	MessageInfo msg;
	uint32_t result;
	dputs("Audio server was started.\n");
	dprintf("I am %d\n", syscall_get_tid());
	while(1)
	{
		if( MonAPI::Message::receive(&msg) )
		{
			continue;
		}
		dprintf("msg.header = %d\n", msg.header);
		switch(msg.header)
		{
			case MSG_AUDIO_CREATE_CHANNEL:
				result = server.createChannel(msg.arg1);
				audio_message_reply(noncheck, &msg, result);
				break;
			case MSG_AUDIO_DESTROY_CHANNEL:
				result = server.destroyChannel(msg.arg1);
				audio_message_reply(noncheck, &msg, result);
				break;
			case MSG_AUDIO_SET_FORMAT:
				result = server.setFormat(msg.arg1, (struct audio_data_format*)&msg.str);
				audio_message_reply(isnotzero, &msg, result);
				break;
			case MSG_AUDIO_GET_FORMAT:
				result = server.getFormat(msg.arg1, (struct audio_data_format*)&msg.str);
				audio_message_reply(isnotzero, &msg, result, 0, msg.str);
				break;
			case MSG_AUDIO_SET_STREAM:
				result = server.setStream(msg.arg1, msg.arg2);
				audio_message_reply(isnotzero, &msg, result);
				break;
			case MSG_AUDIO_GET_STREAM:
				//result = server.getStream(msg.arg1);
				audio_message_reply(unsupported, &msg, MONA_FAILURE);
				//audio_message_reply(iszero, &msg, result);
				break;
			case MSG_AUDIO_SET_BLOCKSIZE:
				audio_message_reply(unsupported, &msg, MONA_FAILURE);
				break;
			case MSG_AUDIO_GET_BLOCKSIZE:
				audio_message_reply(unsupported, &msg, MONA_FAILURE);
				break;
			case MSG_AUDIO_START:
				result = server.start(msg.arg1);
				audio_message_reply(isnotzero, &msg, result);
				break;
			case MSG_AUDIO_STOP:
				result = server.stop(msg.arg1);
				audio_message_reply(isnotzero, &msg, result);
				break;
			case MSG_AUDIO_SET_VOLUME:
				result = server.setVolume(msg.arg1, msg.arg2);
				MonAPI::Message::reply(&msg, result);
				break;
			case MSG_AUDIO_GET_VOLUME:
				result = server.getVolume(msg.arg1);
				MonAPI::Message::reply(&msg, result);
				break;
			case MSG_AUDIO_WAIT_PLAYING:
				result = server.wait(msg.arg1);
				MonAPI::Message::reply(&msg, result);
				break;
			case MSG_AUDIO_GET_CHANNELS_LIST:
				MonAPI::Message::reply(&msg, MONA_FAILURE);
				break;
			default: break;
		}
	}
	return 0;
}
