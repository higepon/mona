#include <stdio.h>
#include <monapi/Message.h>
#include <monapi/syscall.h>
#include <stdlib.h>
#include "servers/audio.h"
#include "audio_server.h"
#include "debug.h"

#if 0
class AudioServer
{
public:
	AudioServer();
	~AudioServer();

	int createChannel(int direction);
	int destroyChannel(int channel);
	int setFormat(int channel, struct audio_data_format *format);
	int getFormat(int channel, struct audio_data_format *format);
	int setStream(int channel, uint32_t handle);
	int setBlockSize(int channel, int blocksize);
	int getBlockSize(int channel);
	int start(int channel);
	int stop(int channel);
	int setVolume(int channel, int volume);
	int getVolume(int channel);
//private:
	struct audio_driver *driver_;
	handle_t device_;
	Channel *channel_;
	size_t blocksize_;
	MonAPI::Thread* mixer_;
};
#endif

#if 0

#endif

int audio_message_reply(int (*iserror)(uint32_t),
	MessageInfo *info, uint32_t arg2 = 0,
	uint32_t arg3 = 0, const char *str = NULL)
{
	dputs(__func__);
	if( iserror(arg2) )
		return MonAPI::Message::replyError(info, arg2, arg3, str);
	else
		return MonAPI::Message::reply(info, arg2, arg3, str);
}

int isnotzero(uint32_t x){ return (x != 0); }
int iszero(uint32_t x){ return (x == 0); }
int unsupported(uint32_t x){ return (x != x); }
int noncheck(uint32_t x){ return (x == x); }

int main(int argc, char *argv[])
{
	AudioServer server;
	MessageInfo msg;
	uint32_t result;
	printf("Audio server was started.\n");
	while(1)
	{
		if( MonAPI::Message::receive(&msg) )
		{
			continue;
		}
		switch(msg.header)
		{
			case MSG_AUDIO_CREATE_CHANNEL:
				result = server.createChannel(msg.arg1);
				MonAPI::Message::reply(&msg, result);
				//audio_message_reply(noncheck, &msg, result);
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
			case MSG_AUDIO_GET_CHANNELS_LIST:
				MonAPI::Message::reply(&msg, MONA_FAILURE);
				break;
			default: break;
		}
	}
	return 0;
}
