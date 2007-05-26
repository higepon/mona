#include "drivers/audiodriver.h"
#include "Audio.h"

Channel::Channel()
{
}

Channel::~Channel()
{
}

bool Channel::init(struct driver_desc* desc)
{
	if( desc == NULL ) return false;
	driver = desc;
	channel = driver->create_channel();
	if( channel == -1 ) return false;
	return true;
}

void Channel::prepare(int samplerate, int bits, int isStereo)
{
	driver->prepare_channel(channel, samplerate, bits, isStereo);
}

void Channel::set_buffer(void *p, int size)
{
	driver->set_buffer(channel, p, size);
}

void Channel::start()
{
	driver->start_channel(channel, 0);
}

void Channel::stop()
{
	driver->stop_channel(channel);
}


