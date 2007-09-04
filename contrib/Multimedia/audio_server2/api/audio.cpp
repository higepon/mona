#include "audio.h"
#include "../servers/audio.h"
#include <monapi/Message.h>
#include <stdint.h>
#include <string.h>

static uint32_t server_id = THREAD_UNKNOWN;
static const char *server_name = "AUDIO.EX5";

uint32_t find_audio_server()
{
	return MonAPI::Message::lookupMainThread(server_name);
}

Audio::Audio(struct audio_data_format *format, int direction)
					: volume_(0), direction_(direction)
{
	MessageInfo msg;
	char buf[128];
	memcpy(&format_, format, sizeof(struct audio_data_format));
	if( server_id == THREAD_UNKNOWN )
	{
		server_id = find_audio_server();
	}
	MonAPI::Message::sendReceive(&msg, server_id, MSG_AUDIO_CREATE_CHANNEL, direction_);
	channel_ = msg.arg2;
	memcpy(buf, &format_, sizeof(struct audio_data_format));
	MonAPI::Message::sendReceive(&msg, server_id, MSG_AUDIO_SET_FORMAT,
							channel_, 0, 0, buf);
	stream_ = new MonAPI::Stream;
	MonAPI::Message::sendReceive(&msg, server_id, MSG_AUDIO_SET_STREAM,
						(uint32_t)channel_,
						(uint32_t)stream_->handle());
}

Audio::~Audio()
{
	MessageInfo msg;
	MonAPI::Message::sendReceive(&msg, server_id, MSG_AUDIO_DESTROY_CHANNEL,
							channel_);
	delete stream_;
}

int Audio::start()
{
	MessageInfo msg;
	MonAPI::Message::sendReceive(&msg,server_id, MSG_AUDIO_START, channel_);
	return msg.arg2;
}

int Audio::stop()
{
	MessageInfo msg;
	MonAPI::Message::sendReceive(&msg,server_id, MSG_AUDIO_STOP, channel_);
	return msg.arg2;
}

size_t Audio::write(void *buffer, size_t size)
{
	MessageInfo msg;
	if( direction_ == AUDIO_INPUT ) return (size_t)-1;
	return stream_->write((uint8_t*)buffer, (uint32_t)size);
}

