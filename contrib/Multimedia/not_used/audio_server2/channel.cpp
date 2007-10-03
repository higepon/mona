#include "audio_server.h"
#include "debug.h"
#include <string.h>

Channel::Channel(int direction) : direction_(direction), stream_(NULL)
{
	memset(&desc_, 0, sizeof(struct audio_channel_description));
}

Channel::~Channel()
{
}

int Channel::setFormat(struct audio_data_format *format)
{
	dprintf("&desc_.format = %x\n", &desc_.format);
	desc_.format = *format;
	//memcpy(&desc_.format, format, sizeof(*format));
	dputs("The format was set.");
	return 0;
}

int Channel::getFormat(struct audio_data_format *format)
{
	*format = desc_.format;
//	memcpy(format, &desc_.format, sizeof(struct audio_data_format));
	return 0;
}

int Channel::setStream(uint32_t handle)
{
	stream_ = MonAPI::Stream::FromHandle(handle);
	dprintf("stream_ = %x\n", stream_);
	return 0;
}

MonAPI::Stream *Channel::getStream()
{
	return stream_;
}

int Channel::setVolume(int volume)
{
	desc_.volume = volume;
	return 0;
}

int Channel::getVolume()
{
	return desc_.volume;
}

