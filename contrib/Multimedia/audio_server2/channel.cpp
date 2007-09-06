#include "audio_server.h"
#include "debug.h"
#include <string.h>

Channel::Channel(int direction) : direction_(direction), stream_(NULL)
{
denter
	memset(&desc_, 0, sizeof(struct audio_channel_description));
dleave
}

Channel::~Channel()
{
denter
dleave
}

int Channel::setFormat(struct audio_data_format *format)
{
denter
	dprintf("&desc_.format = %x\n", &desc_.format);
	memcpy(&desc_.format, format, sizeof(struct audio_data_format));
dleave
	return 0;
}

int Channel::getFormat(struct audio_data_format *format)
{
denter
	memcpy(format, &desc_.format, sizeof(struct audio_data_format));
dleave
	return 0;
}

int Channel::setStream(uint32_t handle)
{
denter
	stream_ = MonAPI::Stream::FromHandle(handle);
	dprintf("stream_ = %x\n", stream_);
dleave
	return 0;
}

MonAPI::Stream *Channel::getStream()
{
denter
dleave
	return stream_;
}

int Channel::setVolume(int volume)
{
denter
	desc_.volume = volume;
dleave
	return 0;
}

int Channel::getVolume()
{
denter
dleave
	return desc_.volume;
}

/*
int direction_;
struct audio_channel_description desc_;
MonAPI::Stream stream_*;
*/
